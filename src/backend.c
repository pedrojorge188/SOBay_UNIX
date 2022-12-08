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
    tryLogin login;
    user userData;
    fd_set fds;
    client users[MAX_USERS];
    
    init_env_var();

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
                }
                
        }else if(res > 0 && FD_ISSET(fd,&fds)){

            if(CONNECTED_USERS < MAX_USERS){

                    char *userFileName = getenv("FUSERS");

                    nBytes = read(fd,&login,sizeof(tryLogin));

                    int user_balance;

                    if(loadUsersFile(userFileName) == -1){
                        printf("%s\n", getLastErrorText());
                        exit(1);
                    }

                    int i;

                    int validation = isUserValid(login.username,login.psw);

                    if(validation == 1){ //password or username correct
                        
                        sprintf(fifo_cli,FIFO_CLI,login.my_pid);
                        int fr = open(fifo_cli,O_WRONLY);

                        if(fr == -1){

                            printf(FILE_ERROR);

                        }else{
                            

                            for(i = 0; i < strlen(login.username);i++)
                                userData.name[i] = login.username[i];
                            
                            userData.name[i+1] = '\0';

                            userData.money = getUserBalance(userData.name);
                            
                            userData.status = CONNECT_TRUE;

                            nBytes = write(fr,&userData,sizeof(user));

                            pidUsers[CONNECTED_USERS] = login.my_pid;

                            for(i = 0; i < strlen(login.username)+1;i++)
                                 users[CONNECTED_USERS].name[i] = userData.name[i];
                            
                            users[CONNECTED_USERS].pid = login.my_pid;
                            users[CONNECTED_USERS].balance = userData.money;
                            users[CONNECTED_USERS].connection = true;

                            CONNECTED_USERS++;
                            
                        }

                        close(fr);

                    }else if(validation == 0){ //password or username invalid

                        sprintf(fifo_cli,FIFO_CLI,login.my_pid);
                        int fr = open(fifo_cli,O_WRONLY);

                        if(fr == -1){

                            printf(FILE_ERROR);

                        }else{

                             for(i = 0; i < strlen(login.username);i++)
                                userData.name[i] = login.username[i];
                            
                            userData.name[i+1] = '\0';

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
            }else
                printf("\n<SERVER> MAX USERS REACHED!\n");
            

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

    while(fgets(itemBuffer,sizeof(itemBuffer),fItems)){

        printf("-%s-",itemBuffer);
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

        for(int i=0;i<CONNECTED_USERS;i++){

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
        // end to backend the command
        printf("Executing command...\n");
        
        return 1;
    }
}

void disconnect_users(){

    int pid_to_kill;

    for(int i=0;i<CONNECTED_USERS;i++){
        
        pid_to_kill = pidUsers[i];

        kill(pid_to_kill,SIGQUIT);

        printf("\n<SERVER> %d DISCONNECTED!\n",pid_to_kill);

    }
}
void init_env_var(){
    setenv("FPROMOTERS", "promo.txt", 0);
    setenv("FITEMS","items.txt", 0);
    setenv("FUSERS","users.txt", 0);
}