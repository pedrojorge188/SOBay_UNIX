
#include "frontend.h"

static void* sendLife(void* data){

    char *heartbeat = getenv("HEARTBEAT");

    int myPid = getpid();

    int fd = open(FIFO_BEAT,O_RDWR);

    if(fd == -1){
        printf("ERROR OPENING FIFO!");
        unlink(FIFO_BEAT);
        exit(1);
    }

    while(out == 0){

        write(fd,&myPid,sizeof(int));
        sleep(atoi(heartbeat));
    }

    pthread_exit(NULL);
}

void handle_quit(int sig){

    char fifo_cli[50];
    sprintf(fifo_cli,FIFO_CLI,getpid());
    unlink(fifo_cli);
    exit(1);

}

void backend_sigs(int sig){

    char fifo_cli[50];

    printf("\n<SERVER> %s",KICK_LOG);
    sprintf(fifo_cli,FIFO_CLI,getpid());
    unlink(fifo_cli);
    exit(1);
}

int main(int argc, char **argv) {
    
    setenv("HEARTBEAT","5", 0);

    notification api;
    info api_recive;
    fd_set fds;
    user MyAccount;

    pthread_t threads;

    int fc,fs,nBytes,i;
    char command[MSG_TAM];
    char fifo_cli[50];
    
    setbuf(stdout,NULL);

    if(argc != 3){
         printf(WRONG_INPUT);
         exit(1);
    }

    
    if(CONNECTED_USERS <= MAX_USERS){

        api.status = LOGIN_INFO;
        api.pid = getpid();
        
        strcpy(api.login.username,argv[1]);

        strcpy(api.login.psw,argv[2]);


        sprintf(fifo_cli,FIFO_CLI,api.pid);
        if(access(fifo_cli,F_OK) != 0)
            mkfifo(fifo_cli,0666);

        fs = open(FIFO_SRV, O_WRONLY);

        if(fs == -1){
            printf("SERVER NOT RUNNING!\n");
            unlink(fifo_cli);
            exit(EXIT_FAILURE);
        }

        nBytes = write(fs,&api,sizeof(notification));

        close(fs);

        // open fifo to read server response

        fc = open(fifo_cli,O_RDWR);

        if(fc == -1){
            printf("ERROR TO OPEN FIFO SERVER\n");
            unlink(fifo_cli);
            exit(1);
        }

        nBytes = read(fc,&MyAccount,sizeof(user));
        

        if(MyAccount.status == USER_LOGIN_SUCCESS){

            printf("<SERVER>WELCOME(Name:%s | Balance: %d)\n",MyAccount.name,MyAccount.money);

        }else if(MyAccount.status == USER_NOT_FOUND){
            
            printf("<SERVER> USER DATA INVALID\n");
            unlink(fifo_cli);
            exit(EXIT_FAILURE);

        }else{

            unlink(fifo_cli);
            exit(EXIT_FAILURE);

        }

        close(fc);
        
    }else{
        printf("MAX USERS REACHED!");
        exit(EXIT_FAILURE);
    }
   
    if(pthread_create(&threads,NULL,sendLife,0) != 0)
        printf("ERROR ON SENDLIFE THREAD CREATION\n");


    do{
        signal(SIGINT,handle_quit);
        signal(SIGQUIT,backend_sigs);

        fc = open(fifo_cli,O_RDWR);

        if(fc == -1){
            printf("ERROR TO OPEN SERVER PIPE");
            exit(EXIT_FAILURE);
        }

        FD_ZERO(&fds); //reset fds pointer 
        FD_SET(0,&fds);  //define input as stdin
        FD_SET(fc,&fds); //define pipe input (in this case we'r refering to fd)

        int res = select(fc+1,&fds,NULL,NULL,NULL);


        if(res > 0 && FD_ISSET(0,&fds)){

            char a_command[MSG_TAM];

            fgets(command,MSG_TAM-1,stdin);
            command[strlen(command)-1] = '\0';

            strcpy(a_command,command);

            if (setup_command(command) == 0){

                printf(WRONG_COMMAND,command);

            }else{

                api.status = COMMAND_INFO;
                api.pid = getpid();

                strcpy(api.cmd.command,a_command);
                strcpy(api.cmd.name,command);

                fs = open(FIFO_SRV,O_WRONLY);

                if(fs == -1){
                    printf("ERROR TO OPEN SERVER PIPE");
                    exit(EXIT_FAILURE);
                }
                
                write(fs,&api,sizeof(notification));

                close(fs);
            }

        }else if(res > 0 && FD_ISSET(fc,&fds)){

            nBytes = read(fc,&api_recive,sizeof(info));

            if(api_recive.status == INFO){
                
                printf("%s\n",api_recive.message);

            }else if(api_recive.status == ITEM_INFO){

                printf("%s\n",api_recive.message);

                for(int i=0;i<MAX_ITEMS;i++){

                    if(api_recive.items[i].sell_state == true){

                        printf("(ID: %d) ",api_recive.items[i].id);
                        printf("(NAME: %s) ",api_recive.items[i].name);
                        printf("(CAT: %s) ",api_recive.items[i].category);
                        printf("(PRICE: %d) ",api_recive.items[i].current_price);
                        printf("(CURRENT PRICE: %d) ",api_recive.items[i].buy_now_price);
                        printf("(TIME LEFT: %d) ",api_recive.items[i].time_left);
                        printf("(OWNER: %s) ",api_recive.items[i].username_owner);
                        printf("(BEST OFFER: %s) \n",api_recive.items[i].username_best_option);
                    }
                }

            }
        }

        WRONG = 0;
        setbuf(stdin,NULL);
        close(fc);

    }while(strcmp(command,"exit") != 0);

    out = 1;

    pthread_join(threads,NULL);

    unlink(fifo_cli);

    return 0;
}

int setup_command(char *command){
    
    char *token, sub[10];
    int ind = -1;
    int any = 0;
    int counter = 0;
    char fifo_cli[50];

    token = strtok(command, SPACE);

    if(strcmp(token, LIST[NUMBER_OF_COMMANDS-1]) == 0){
            sleep(1);
            printf("Closing...\n");
            sprintf(fifo_cli,FIFO_CLI,getpid());
            unlink(fifo_cli);
            exit(1);
    }

    for (int i = 0; i < NUMBER_OF_COMMANDS; i++){
        if(strcmp(token, LIST[i]) == 0){
            ind = i;
        }
    }

   switch(ind){
        case 0:
            for(int i = 0; i < LIST_INDEX[ind]; i++ ){
                token = strtok(NULL, SPACE);
                if (token == NULL)
                    break;
                strcpy(sub, token);
                if(i<2 && atoi(sub)!=0){   //Se for passado um nome para numero
                    WRONG++;
                    break;
                }
                if(i>=2 && atoi(sub)==0){
                    WRONG++;
                    break;
                }
                counter++;
            }
            break;

        case 2:
            token = strtok(NULL, SPACE);
            if (token == NULL)
                break;
            strcpy(sub, token);
            if(atoi(sub)!=0) {
                WRONG++;
                break;
            }
            counter++;
            break;

        case 3:
            token = strtok(NULL, SPACE);
            if (token == NULL)
                break;
            strcpy(sub, token);
            if(atoi(sub)!=0) {
                WRONG++;
                break;
            }
            counter++;
            break;

        case 4:
            token = strtok(NULL, SPACE);
            if (token == NULL)
                break;
            strcpy(sub, token);
            if(atoi(sub)==0) {
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
            if(atoi(sub)==0) {
                WRONG++;
                break;
            }
            counter++;
            break;

        case 7:
            for(int i = 0; i < LIST_INDEX[ind]; i++ ){
                token = strtok(NULL, SPACE);
                if (token == NULL)
                    break;
                strcpy(sub, token);
                if(atoi(sub)==0){   //Se for passado um nome para numero
                   WRONG++;
                   break;
                }
                counter++;
            }
            break;

        case 9:
            token = strtok(NULL, SPACE);
            if (token == NULL)
                break;
            strcpy(sub, token);
            if(atoi(sub)==0) {
                WRONG++;
                break;
            }
            counter++;
            break;

    }

    if(counter < LIST_INDEX[ind] || strtok(NULL, SPACE) != NULL || ind == -1 || WRONG == 1){ //Limite   inferior / superior

            if(WRONG > 0)
                printf(WRONG_VALUES, strtok(command, SPACE));
            else
                printf(WRONG_COMMAND, strtok(command, SPACE));
            return 0;
    }else{
        return 1;
    }

}