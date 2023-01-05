#include "backend.h"
extern char **environ;

void signal_handler(int sig){}

void handle_quit(int sig){

    unlink(FIFO_SRV);
    unlink(FIFO_BEAT);
    disconnect_users();
    puts("\n<SERVER> CLOSING ...");

    out = 1;
    outProm = 1;

    sleep(10);
    exit(1);
}

static void* timeKill(void* data){
    client *args = (client *)data;
    char *heartbeat = getenv("HEARTBEAT");
    int timeKill = 0; 

    while(out == 0){

        if(timeKill == 0){
            timeKill = TIME + atoi(heartbeat);
        }

        if(TIME == timeKill){
            for(int i = 0; i < MAX_USERS; i++){
                if(args[i].signal == 0){
                    args[i].connection = false;
                }
            }

            for(int i = 0; i< MAX_USERS; i++){
                args[i].signal = 0;
            }

            timeKill = TIME + atoi(heartbeat);
        }
    }

    pthread_exit(NULL);
}

static void* checkLife(void* data){

    args_thread *args = (args_thread *)data;

    int pid = 0;
    int fd = open(args->fifo_name,O_RDWR);

    if(fd == -1) {
        unlink(args->fifo_name);
        exit(1);
    }

    while(out == 0){

        read(fd,&pid,sizeof(int));

        for(int i = 0; i< MAX_USERS; i++){
            if(args->users[i].pid == pid){
                args->users[i].signal = 1;
            }
        }
    }

    pthread_exit(NULL);

}

static void* actionTurn(void* data){

    args_thread *args = (args_thread *)data;

    int ind = -1;
    int fMsg;
    char fifo_cli[50];
    char msg[60];
    info api;

    while(out == 0){
        sleep(1);
        for(int i = 0; i < MAX_ITEMS ; i++){
            
            if(args->item[i].time_left == 0 && args->item[i].sell_state == true){

                for(int j=0;j<MAX_USERS;j++)
                    if(strcmp(args->users[j].name,args->item[i].username_best_option) == 0)
                        ind = j;
    
                
                if(ind != -1){
                    args->users[ind].balance = args->users[ind].balance - args->item[i].current_price;

                    sprintf(fifo_cli,FIFO_CLI,args->users[ind].pid);
                    sprintf(msg,"<SERVER> YOU WIN THE BID (PRODUCT:%s)",args->item[i].name);

                    fMsg = open(fifo_cli,O_WRONLY);
                    api.status = INFO;
                    strcpy(api.message,msg);
                    write(fMsg,&api,sizeof(api));
                    close(fMsg);
                }

                args->item[i].sell_state = false;

            }else if(args->item[i].time_left > 0 && args->item[i].sell_state == true){

                args->item[i].time_left --;

                if(args->item[i].time_promotion > 0){

                    args->item[i].time_promotion--;

                }else if(args->item[i].time_promotion == 1){

                    args->item[i].buy_now_price = args->item[i].buy_now_price + args->item[i].value_promotion;
                    
                }
            }

            
        }
    }
    pthread_exit(NULL);

}

static void* Timer(void *data){
    while(out == 0){
        sleep(1);
        TIME++;
    }
    
    pthread_exit(NULL);
}

static void* promoterAction(void *data){

    char *promoter1;
    int f1;
    char mensage_promoter[20];

    fd_set fds;
    args_promoter *args = (args_promoter *)data;

    printf("<SERVER> PROMOTERS RUNNING\n");

        promoter1 = "./promotor_oficial";

        int tube[2];
        
        pipe(tube);

        if(fork() == 0){
            
            close(1);
            dup(tube[WR]);
            close(tube[WR]);
            close(tube[RD]);
            
            execlp(promoter1,promoter1,NULL);
            
        }else{

            close(tube[WR]);

            while(outProm == 0){
                char *token;
                char cat[20];
                int promotion,time;

                if( (f1 = read(tube[RD],mensage_promoter,sizeof(mensage_promoter))) >= 0){
                    
                    token = strtok(mensage_promoter,SPACE);
                    strcpy(cat,token);
                    token = strtok(NULL,SPACE);
                    promotion = atoi(token);
                    token = strtok(NULL,"\0");
                    time = atoi(token);

                    for(int i = 0; i < MAX_ITEMS;i ++){

                        if((strcmp(args->item[i].category,cat) == 0) && (args->item->sell_state == true)){


                            args->item[i].buy_now_price = args->item[i].buy_now_price - promotion;
                            args->item[i].time_promotion = time;
                            args->item[i].value_promotion = promotion;

                            if(args->item[i].buy_now_price < 0)
                                args->item[i].buy_now_price = 0;

                        }
                    }
                }

            }
            
            close(tube[RD]);
        }

    }

int main(int argc, char *argv[], char **envp)
{      
    init_env_var();

    int res,nBytes,fd,nPromos,nItems;
    char command[MSG_TAM],fifo_cli[50];
    
    items itemsList[MAX_ITEMS];
    notification api;
    info api_return;
    user userData;
    fd_set fds;
    client users[MAX_USERS];
    promoter promo[MAX_PROMOS];

    args_thread args;
    args_promoter args_promo;
    pthread_t threadId[N_THREADS];
    pthread_t promtoThread;

    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sigaction(SIGINT,&sa,NULL);
    sigaction(SIGUSR1,&sa,NULL);

    fill_users(users);
    fill_items(itemsList);
    fill_promos(promo);
    
    nItems = load_items(itemsList);
    nPromos = getPromoters(promo);

    setbuf(stdout,NULL);

    if(access(FIFO_SRV, F_OK) != 0) {
        mkfifo(FIFO_SRV, 0666);
        printf("<SERVER> Running...\n");
    }else{
        printf("Backend already in use!\n");
        exit(EXIT_FAILURE);
    }

    if(access(FIFO_BEAT,F_OK) != 0)
        mkfifo(FIFO_BEAT,0666);
    

    if(pthread_create(&threadId[0],NULL,Timer,0) !=0 )
        printf("Error on timer thread creation\n");

    args.users = users;
    args.item = itemsList;
    strcpy(args.fifo_name,FIFO_BEAT);


    if(pthread_create(&threadId[1],NULL,checkLife,(void *)&args) != 0)
        printf("Error on checkLife thread creation\n");

    if(pthread_create(&threadId[2],NULL,timeKill,(void *)&users) != 0)
        printf("Error on timeKill thread creation\n");

    if(pthread_create(&threadId[3],NULL,actionTurn,(void*)&args) != 0)
         printf("Error on actionTurn thread creation\n");
    
    args_promo.users = users;
    args_promo.item = itemsList;
    args_promo.prom = promo;

    pthread_create(&promtoThread,NULL,promoterAction,(void*)&args_promo);


    do{ 

        signal(SIGINT,handle_quit);
        save_file_items(itemsList);
        save_users_file();

        fd = open(FIFO_SRV,O_RDWR);

        if(fd == -1){
            printf("ERROR TO OPEN SERVER PIPE");
            unlink(FIFO_SRV);
            exit(EXIT_FAILURE);
        }

        FD_ZERO(&fds); //reset fds pointer 
        FD_SET(0,&fds);  //define input as stdin
        FD_SET(fd,&fds); //define pipe input (in this case we'r refering to fd)

        res = select(fd+1,&fds,NULL,NULL,NULL);

        if(res == 0){

                 printf("<SERVER> ANY DATA TO READ!");

        }else if(res > 0 && FD_ISSET(0,&fds)){

                fgets(command,MSG_TAM-1,stdin);
                command[strlen(command)-1] = '\0';

                if (setup_command(command) == 0)

                    printf(WRONG_SINTAXE);

                else{

                    if(strcmp(command,"users") == 0)
                        list_users((client *)&users);
                    
                    if(strcmp(command,"list") == 0)
                        list_items((items *)&itemsList,nItems);


                    if(strcmp(command,"kick") == 0)
                        kick_user(command + 5,(client *)&users);
                    
                    if(strcmp(command, "prom") == 0)
                        listPromos(promo);
                    
                    if(strcmp(command, "reprom") == 0){
                        fill_promos(promo);
                        nPromos = getPromoters(promo);
                    }

                    if(strcmp(command, "cancel") == 0){
                        // int promoter_to_close = command+7;
                        outProm = 1;
                    }

                    

                }
                
        }else if(res > 0 && FD_ISSET(fd,&fds)){

            nBytes = read(fd,&api,sizeof(notification));

            sprintf(fifo_cli,FIFO_CLI,api.pid);


            if(api.status == LOGIN_INFO){
                
                char *userFileName = getenv("FUSERS");
                int user_balance;

                if(loadUsersFile(userFileName) == -1){
                    printf("%s\n", getLastErrorText());
                    exit(1);
                }

                int i;

                int validation = isUserValid(api.login.username,api.login.psw);

                if(validation == 1 ){ //password or username correct
                    
                    int fr = open(fifo_cli,O_WRONLY);

                    if(fr == -1){

                        printf(FILE_ERROR);

                    }else{
                        
                        int ind = get_ind((client * )&users);

                        if(ind > -1){

                            for(i = 0; i < strlen(api.login.username);i++)
                                userData.name[i] = api.login.username[i];
                        
                            userData.name[i] = '\0';

                            userData.money = getUserBalance(userData.name);
                            
                            userData.status = CONNECT_TRUE;

                            nBytes = write(fr,&userData,sizeof(user));

                            pidUsers[helper] = api.pid; // pid's log

                            for(i = 0; i < strlen(api.login.username)+1;i++)
                                    users[ind].name[i] = userData.name[i];
                            

                            users[ind].pid = api.pid;
                            users[ind].balance = userData.money;
                            users[ind].connection = true;

                            helper ++;
                            CONNECTED_USERS++;

                        }else{
                            
                            printf("<SERVER>MAX USERS REACHED!\n");

                        }
                    
                        
                    }
                    close(fr);

                }else if(validation == 0){ //password or username invalid

                    sprintf(fifo_cli,FIFO_CLI,api.pid);
                    int fr = open(fifo_cli,O_WRONLY);

                    if(fr == -1){

                        printf(FILE_ERROR);

                    }else{

                        for(i = 0; i < strlen(api.login.username);i++)
                            userData.name[i] = api.login.username[i];
                        
                        userData.name[i] = '\0';

                        userData.money = 0;
                        userData.status = -1;

                        nBytes = write(fr,&userData,sizeof(user));

                    }

                    close(fr);

                }else{

                    printf("ERROR USING (USERS) LIBRARY!");
                    unlink(FIFO_SRV);
                    exit(1);

                }

            }else if(api.status == COMMAND_INFO){
                
                fill_items((struct items*)&api_return.items);

                int fr = open(fifo_cli,O_WRONLY);


                if(strcmp(api.cmd.name,"sell") == 0){
                    
                    char *token;
                    char name[50],category[50],owner[50];
                    int current_price,buy_now_price,time_left;
                    int ind;

                    for(int i=0;i<MAX_USERS;i++)
                        if(users[i].pid == api.pid)
                            strcpy(owner,users[i].name);
                    

                    token = strtok(api.cmd.command,SPACE);
                    token = strtok(NULL,SPACE);
                    strcpy(name,token);
                    token = strtok(NULL,SPACE);
                    strcpy(category,token);
                    token = strtok(NULL,SPACE);
                    current_price = atoi(token);
                    token = strtok(NULL,SPACE);
                    buy_now_price = atoi(token);
                    token = strtok(NULL,"\0");
                    time_left = atoi(token);

                    ind = get_ind_items((items*)&itemsList);

                    itemsList[ind].id = ind+1;
                    strcpy(itemsList[ind].name,name);
                    strcpy(itemsList[ind].category,category);
                    strcpy(itemsList[ind].username_best_option,"none");
                    strcpy(itemsList[ind].username_owner,owner);
                    itemsList[ind].current_price = current_price;
                    itemsList[ind].buy_now_price = buy_now_price;
                    itemsList[ind].time_left = time_left;
                    itemsList[ind].sell_state = true;

                    api_return.status = INFO;
                    strcpy(api_return.message,"<SERVER>ITEM ADDED!");
                    write(fr,&api_return,sizeof(info));

                }else if(strcmp(api.cmd.name,"licat") == 0){
                    
                    char category[20] ;
                    int counter = 0;
                    strcpy(category,api.cmd.command+6);

                    api_return.status = ITEM_INFO;
                    strcpy(api_return.message,"<SERVER>ITEMS OF CATEGORY!");
                    
                     for(int i=0;i<MAX_ITEMS;i++){
            
                        if(strcmp(itemsList[i].category,category) == 0){
                            api_return.items[i] = itemsList[i];
                            counter++;
                        }
                     }

                     if(counter == 0)
                         strcpy(api_return.message,NONE_ITEMS);

                    write(fr,&api_return,sizeof(info));

                }else if(strcmp(api.cmd.name,"lisel") == 0){

                    char name_owner[20] ;
                    int counter = 0;
                    strcpy(name_owner,api.cmd.command+6);

                    api_return.status = ITEM_INFO;
                    strcpy(api_return.message,"<SERVER>ITEMS (username)!");

                    for(int i=0;i<MAX_ITEMS;i++){
            
                        if(strcmp(itemsList[i].username_owner,name_owner) == 0){
                            api_return.items[i] = itemsList[i];
                            counter ++;
                        }
                        
                    }

                    if(counter == 0)
                         strcpy(api_return.message,NONE_ITEMS);

                    write(fr,&api_return,sizeof(info));

                }else if(strcmp(api.cmd.name,"lival") == 0){

                    char max_val[20] = "0";
                    int counter = 0;
                    strcpy(max_val,api.cmd.command+6);

                    api_return.status = ITEM_INFO;
                    strcpy(api_return.message,"<SERVER>ITEMS (value)!");

                    int value = atoi(max_val);


                    for(int i=0;i<MAX_ITEMS;i++){
            
                        if(itemsList[i].current_price <= value){
                            api_return.items[i] = itemsList[i];
                            counter ++;
                        }
                        
                    }

                    if(counter == 0)
                         strcpy(api_return.message,NONE_ITEMS);

                    write(fr,&api_return,sizeof(info));

                }else if(strcmp(api.cmd.name,"litime") == 0){

                    char time[20] = "0";
                    int counter = 0;
                    strcpy(time,api.cmd.command+6);

                    api_return.status = ITEM_INFO;
                    strcpy(api_return.message,"<SERVER>ITEMS (time)!");

                    int value = atoi(time);

                    for(int i=0;i<MAX_ITEMS;i++){
            
                        if(itemsList[i].time_left <= value){
                            api_return.items[i] = itemsList[i];
                            counter ++;
                        }
                        
                    }

                    if(counter == 0)
                         strcpy(api_return.message,NONE_ITEMS);

                    write(fr,&api_return,sizeof(info));

                }else if(strcmp(api.cmd.name,"time") == 0){

                    char msg[MSG_TAM];

                    sprintf(msg,"<SERVER>CURRENT TIME -> %d seconds",TIME);
                    api_return.status = INFO;
                    strcpy(api_return.message,msg);
                    write(fr,&api_return,sizeof(info));

                }else if(strcmp(api.cmd.name,"buy") == 0){
                    
                    char *token;
                    int id_item,value,cash,item_ind,user_ind,counter = 0;
                    char cli_state[30];

                    token = strtok(api.cmd.command,SPACE);
                    token = strtok(NULL,SPACE);
                    id_item = atoi(token);
                    token = strtok(NULL,"\0");
                    value = atoi(token);

                    cash = get_cash_by_pid((client *)&users,api.pid);

                    for(int i=0;i<MAX_ITEMS;i++){
                            if(itemsList[i].id == id_item && itemsList[i].sell_state == true){
                                item_ind = i;
                                counter++;
                            }
                    }

                    for(int i=0;i<MAX_USERS;i++)
                        if(users[i].pid == api.pid)
                            user_ind = i;

                    if(cash > 0){
                        
                        if(counter == 0){

                            strcpy(api_return.message,"<SERVER>ITEM ID INVALID!");

                        }else if(value >= itemsList[item_ind].buy_now_price){
                            
                            if((cash - value) >= 0){

                                users[user_ind].balance -= value;
                                strcpy(itemsList[item_ind].username_owner,users[user_ind].name);
                                updateUserBalance(users[user_ind].name,users[user_ind].balance);
                                strcpy(itemsList[item_ind].username_best_option,users[user_ind].name); 
                                itemsList[item_ind].sell_state = false;

                                strcpy(api_return.message,"<SERVER>YOU BOUGHT ITEM!");

                            }else{
                                strcpy(api_return.message,"<SERVER>YOU DONT HAVE ENOUGHT MONEY!");
                            }

                        }else if(value > itemsList[item_ind].current_price){

                             strcpy(itemsList[item_ind].username_best_option,users[user_ind].name);
                             itemsList[item_ind].current_price = value;
                             strcpy(api_return.message,"<SERVER>YOU MADE A BID");
                        }

                    }else{
                         strcpy(api_return.message,"<SERVER>YOU DONT HAVE ANY CASH!");
                    }

                    api_return.status = INFO;
                    write(fr,&api_return,sizeof(info));

                }else if(strcmp(api.cmd.name,"cash")== 0) {
                    
                    int user_cash;
                    char msg[MSG_TAM];

                    user_cash = get_cash_by_pid((client *)&users,api.pid);

                    api_return.status = INFO;
                    sprintf(msg,"<SERVER>YOUR CASH -> %d €",user_cash);
                    strcpy(api_return.message,msg);
                    write(fr,&api_return,sizeof(info));

                }else if(strcmp(api.cmd.name,"add") == 0 ){
                    
                    char total_to_add[20];
                    int value;

                    strcpy(total_to_add,api.cmd.command+4);
                    value = atoi(total_to_add);

                    for(int i=0;i <MAX_USERS;i++){
                         if(users[i].pid == api.pid){
                            users[i].balance += value;
                            updateUserBalance(users[i].name,users[i].balance);
                         }

                    }
                    
                    api_return.status = INFO;
                    strcpy(api_return.message,"<SERVER>ADDED CASH TO YOUR ACCOUNT!");
                    write(fr,&api_return,sizeof(info));

                }else if(strcmp(api.cmd.name,"list") == 0){

                    api_return.status = ITEM_INFO;
                    strcpy(api_return.message,ALL_ITEMS_INFO);

                    for(int i=0;i<MAX_ITEMS;i++)
                        api_return.items[i] = itemsList[i];
                    

                    write(fr,&api_return,sizeof(info));

                }

                close(fr);

            }

        }else{
            printf("\nSELECT ERROR!\n");
            unlink(FIFO_SRV);
            exit(EXIT_FAILURE);
        }

        setbuf(stdin, NULL);
        close(fd);

        
    }while(command != "close");
     
    out = 1;

    pthread_kill(promtoThread,SIGUSR1);
    pthread_join(promtoThread,NULL);

    for(int i=0;i<N_THREADS;i++)
        pthread_kill(threadId[i],SIGUSR1);
    

    for(int i=0;i<N_THREADS;i++)
        pthread_join(threadId[i],NULL);
    

    unlink(FIFO_SRV);

    return 0;
}
void save_file_items(items *item){

    char *itemsFileName = getenv("FITEMS");
    FILE *save;
    save = fopen(itemsFileName,"w");
    
    if(save == NULL)
        printf(FILE_ERROR);


    for(int i=0;i<MAX_ITEMS;i++){

        if(item[i].sell_state == true){

            fprintf(save,"%d %s %s %d %d %d %s %s\n",
                    item[i].id,
                    item[i].name,
                    item[i].category,
                    item[i].current_price,
                    item[i].buy_now_price,
                    item[i].time_left,
                    item[i].username_owner,
                    item[i].username_best_option
            );

        }
    }

    fclose(save);

} 
void save_users_file(){

    char *fileName = getenv("FUSERS");
    saveUsersFile(fileName);

}
int load_items(items *itemsList){
    char *itemsFileName = getenv("FITEMS");
    
    FILE   *fItems;
    char itemBuffer[150];
    int counter = 0;
    char *token;

    fItems = fopen(itemsFileName,"r");

    if (!fItems)
        printf(FILE_ERROR);

    char line[100];

    int item = 0;

    while(fgets(itemBuffer,sizeof(itemBuffer),fItems)){
        
        token = strtok(itemBuffer,SPACE);
        itemsList[item].id = atoi(token); // id
        token = strtok(NULL,SPACE);
        strcpy(itemsList[item].name,token); // nome do item
        token = strtok(NULL,SPACE);
        strcpy(itemsList[item].category,token); // categoria do item
        token = strtok(NULL,SPACE);
        itemsList[item].current_price = atoi(token); // valor inicial/licitacao mais elevada
        token = strtok(NULL,SPACE);
        itemsList[item].buy_now_price = atoi(token); //valor da compra imediata
        token = strtok(NULL,SPACE);
        itemsList[item].time_left = atoi(token); //duração/tempo restante do leilao
        token = strtok(NULL,SPACE);
        strcpy(itemsList[item].username_owner,token); // username do vendedor
        token = strtok(NULL,"\n");
        strcpy(itemsList[item].username_best_option,token); //username da melhor opcao de comprador

        itemsList[item].sell_state = true;

        item++;

    }

    printf("\n");

    fclose(fItems);

    return item;
}

void list_items(items *itemsList,int nItems){

    printf("<SERVER>%d ITEMS\n",nItems);

    for(int i=0;i<MAX_ITEMS;i++){

        if(itemsList[i].sell_state == true){

            printf("(ID: %d) ",itemsList[i].id);
            printf("(NAME: %s) ",itemsList[i].name);
            printf("(CAT: %s) ",itemsList[i].category);
            printf("(PRICE: %d) ",itemsList[i].current_price);
            printf("(CURRENT PRICE: %d) ",itemsList[i].buy_now_price);
            printf("(TIME LEFT: %d) ",itemsList[i].time_left);
            printf("(OWNER: %s) ",itemsList[i].username_owner);
            printf("(BEST OFFER: %s) \n",itemsList[i].username_best_option);

        }
    }
}

int getPromoters(promoter *promo){

    char *promoFileName = getenv("FPROMOTERS");
    FILE * f;
    int prom = 0;
    char Buffer[120];
    
    f = fopen(promoFileName,"r");

    if (!f)
        printf(FILE_ERROR);
    
    while(fgets(Buffer,sizeof(Buffer),f)){

        strcpy(promo[prom].name,Buffer);
        promo[prom].signal = 1;
        prom++;

    }

    fclose(f);
    
    return prom;
}

void list_users(client *users){


    if(CONNECTED_USERS > 0){

        printf("\n<SERVER> USERS CONNECTED\n");

        for(int i=0;i<MAX_USERS;i++){

            if(users[i].connection == true){

                printf("PID:%d\t",users[i].pid);
                printf("NAME:%s\t",users[i].name);
                printf("BALANCE:%d\n",users[i].balance);
             }
        }

    }else
        printf("\n<SERVER> ANY USER TO LIST\n");
    

}


int setup_command(char *command)
{

    char *token, sub[10];
    int ind = -1;
    int any = 0;
    int counter = 0;


    token = strtok(command, SPACE);

    if (strcmp(token, LIST[NUMBER_OF_COMMANDS - 1]) == 0)
    {
        sleep(1);
        unlink(FIFO_SRV);
        disconnect_users();
        exit(EXIT_SUCCESS);
    }


    for (int i = 0; i < NUMBER_OF_COMMANDS; i++)
    {
        if (strcmp(token, LIST[i]) == 0)
        {
            ind = i;
        }
    }

    switch (ind)
    {
    case 2:
        token = strtok(NULL, SPACE);
        if (token == NULL)
            break;
        strcpy(sub, token);
        if (atoi(sub) != 0)
        {
            WRONG++;
            break;
        }
        counter++;


        break;

    case 5:
        token = strtok(NULL, SPACE);
        if (token == NULL)
            break;
        strcpy(sub, token);
        if (atoi(sub) != 0)
        {
            WRONG++;
            break;
        }
        counter++;
        break;

    default:
        // Nothing to be Verified
    }

    if (counter < LIST_INDEX[ind] || strtok(NULL, SPACE) != NULL || ind == -1 || WRONG == 1)
    { // Limite   inferior / superior

        if (WRONG > 0)
            printf(WRONG_VALUES, strtok(command, SPACE));
        else
            printf(WRONG_COMMAND, strtok(command, SPACE));
        return 0;
    }
    else
    {   
        return 1;
    }
}

void disconnect_users(){

    int pid_to_kill;

    for(int i=0;i<helper;i++){
        
        pid_to_kill = pidUsers[i];

        kill(pid_to_kill,SIGQUIT);

    }

}

void kick_user(char *username,client *users){

    int pid_to_kill = 0;

    for(int i = 0;i < CONNECTED_USERS;i++){
        
        if(strcmp(users[i].name,username) == 0){

            pid_to_kill = users[i].pid;
            users[i].connection = false;

        }
    }

    if(pid_to_kill != 0){

        kill(pid_to_kill,SIGQUIT);
        printf("\n<SERVER>USER KICKED!\n");

        CONNECTED_USERS--;

    }else{

        printf("\n<SERVER>ANY USER TO KICK!\n");
    }

}

int get_ind(client *users){

    for(int i=0;i < MAX_USERS;i++){
        if(users[i].connection == false)
            return i;    
    }

    return -1;
}

int get_ind_items(items *items){

    for(int i=0;i < MAX_ITEMS;i++){
        if(items[i].sell_state == false)
            return i;    
    }

    return -1;
}

void fill_items(items *items){

    for(int i = 0;i<MAX_ITEMS;i++){
        items[i].id = 0;
        items[i].name[0] = 'd';
        items[i].buy_now_price = 0;
        items[i].category[0] = 'd';
        items[i].current_price = 0;
        items[i].time_left = 0;
        items[i].username_owner[0] = 'd';
        items[i].username_best_option[0] = 'd';
        items[i].sell_state = false;
        items[i].time_promotion = 0;
        items[i].value_promotion = 0;
    }

}

void fill_promos( promoter *promo){

    for(int i= 0;i <MAX_PROMOS;i++){
        promo[i].name[0] = 'd';
        promo[i].signal = 0;
    }
}

void fill_users(client *users){
    int helper = 0;

    for(int i = 0;i < MAX_USERS;i++){

        users[i].name[0] = 'd';
        users[i].pid = 0;
        users[i].connection = false;
        users[i].balance = 0;
        users[i].signal = 0;
    }
}

int get_cash_by_pid(client *users, int pid){

    for(int i = 0;i<MAX_USERS;i++)
        if(users[i].pid == pid)
            return users[i].balance;

    return 0;
}

void listPromos(promoter *promo){


    printf("<SERVER> ACTIVE PROMOTERS\n");

    for(int i=0;i<MAX_PROMOS;i++){

        if(promo[i].signal == 1){
            printf("->%s\n",promo[i].name);
        }

    }
}

void init_env_var(){
    setenv("FPROMOTERS", "promo.txt", 0);
    setenv("FITEMS","items.txt", 0);
    setenv("FUSERS","users.txt", 0);
    setenv("HEARTBEAT","5", 0);
}
