#include "backend.h"
extern char **environ;
 
void handle_quit(int sig){

    unlink(FIFO_SRV);
    
    puts("\n<SERVER> CLOSING ...");

    disconnect_users();

    exit(1);
}

int main(int argc, char *argv[], char **envp)
{   
    int res,nBytes,fd;
    char command[MSG_TAM],fifo_cli[50];
    
    items itemsList[MAX_ITEMS];
    notification api;
    user userData;
    fd_set fds;
    client users[MAX_USERS];
    
    init_env_var();
    fill_users((client *)&users);

    setbuf(stdout,NULL);


    if(access(FIFO_SRV, F_OK) != 0) {
        mkfifo(FIFO_SRV, 0666);
        printf("<SERVER> Running...\n");
    }else{
        printf("Backend already in use!\n");
        exit(EXIT_FAILURE);
    }

    do{

        signal(SIGINT,handle_quit);

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
                        load_items((items *)&itemsList);


                    if(strcmp(command,"kick") == 0)
                        kick_user(command + 5,(client *)&users);


                }
                
        }else if(res > 0 && FD_ISSET(fd,&fds)){

             nBytes = read(fd,&api,sizeof(notification));

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
                    
                    sprintf(fifo_cli,FIFO_CLI,api.pid);

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

                printf("<SERVER> COMMAND RECIVED!");

            }

        }else{
            printf("\nSELECT ERROR!\n");
            unlink(FIFO_SRV);
            exit(EXIT_FAILURE);
        }

        setbuf(stdin, NULL);
        close(fd);

    }while(command != "close");

    unlink(FIFO_SRV);

    return 0;
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

    printf("\n<SERVER>ITEMS!\n");
    while(fgets(itemBuffer,sizeof(itemBuffer),fItems)){

        printf("%s",itemBuffer);
        token = strtok(itemBuffer,SPACE);
        itemsList[item].id = atoi(token); // id
        token = strtok(NULL,SPACE);
        itemsList[item].name = token; // nome do item
        token = strtok(NULL,SPACE);
        itemsList[item].category = token; // categoria do item
        token = strtok(NULL,SPACE);
        itemsList[item].current_price = atoi(token); // valor inicial/licitacao mais elevada
        token = strtok(NULL,SPACE);
        itemsList[item].buy_now_price = atoi(token); //valor da compra imediata
        token = strtok(NULL,SPACE);
        itemsList[item].time_left = atoi(token); //duração/tempo restante do leilao
        token = strtok(NULL,SPACE);
        itemsList[item].username_owner = token; // username do vendedor
        token = strtok(NULL,"\n");
        itemsList[item].username_best_option = token; //username da melhor opcao de comprador

        item++;

    }

    printf("\n");

    fclose(fItems);

    return 1;
}

int getPromoters(){

    char *promoFileName = getenv("FPROMOTERS");
    FILE * f;
    int prom = 0;
    char Buffer[120];
    
    f = fopen(promoFileName,"r");

    if (!f)
        printf(FILE_ERROR);
    
    while(fgets(Buffer,sizeof(Buffer),f)){

        namePromoters[prom] = Buffer;
        prom++;

    }

    fclose(f);
    
    return 1;
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

int run_promoter(char *promoterName)
{
    char *promoter1;
    char mensage_promoter[20];

    //getPromoters(); // Na meta 2 vamos usar para ler o ficheiro promoters

    promoter1 = "./black_friday";

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

        while(1){

            if(read(tube[RD],mensage_promoter,sizeof(mensage_promoter)) >= 0){

                for(int i=0;i<strlen(mensage_promoter)-1;i++)
                    printf("%c",mensage_promoter[i]);

                printf("->PROMOTER RECIVED!\n");
            }

        }
        
        close(tube[RD]);
    }
    
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

void fill_users(client *users){
    int helper = 0;

    for(int i = 0;i < MAX_USERS;i++){

        users[i].name[0] = 'd';
        users[i].pid = 0;
        users[i].connection = false;
        users[i].balance = 0;

    }
}

void init_env_var(){
    setenv("FPROMOTERS", "promo.txt", 0);
    setenv("FITEMS","items.txt", 0);
    setenv("FUSERS","users.txt", 0);
}