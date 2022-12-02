
#include "frontend.h"
 
int main(int argc, char **argv) {

    tryLogin login;
    int fc,fs,nBytes,i;
    char command[MSG_TAM];
    char fifo_cli[50];
    
    setbuf(stdout,NULL);

    if(argc < 3){
         printf(WRONG_INPUT);
         exit(1);
    }


    login.my_pid = getpid();

    for(i = 0; i < strlen(argv[1]);i++){
        login.username[i] = argv[1][i];
    }
        login.username[i] = '\0';

      for(i = 0; i < strlen(argv[2]);i++){
        login.psw[i] = argv[2][i];
    }
        login.psw[i] = '\0';


    sprintf(fifo_cli,FIFO_CLI,login.my_pid);

    //Create fifo for client
    if(access(fifo_cli,F_OK) != 0)
        mkfifo(fifo_cli,0666);

    fs = open(FIFO_SRV, O_WRONLY);

    if(fs == -1){
        printf("SERVER NOT RUNNING!\n");
        unlink(fifo_cli);
        exit(EXIT_FAILURE);
    }

    nBytes = write(fs,&login,sizeof(tryLogin));

    close(fs);

    /*fc = open(fifo_cli,O_RDONLY);

    if(fc == -1){
        printf("ERROR FIFO SERVER\n");
        unlink(fifo_cli);
        exit(1);
    }

    nBytes = read(fc,&MyAccount,sizeof(user));
    
    if(MyAccount.status == 0){
        printf("USER DATA INVALID\n");
        unlink(fifo_cli);
        exit(EXIT_FAILURE);
    }else
        printf("(Name:%s | Balance:%d)\n",MyAccount.name,MyAccount.money);

    close(fc);
    */

    while(FOREVER){

        int result_command;
        
        fgets(command,MSG_TAM-1,stdin);
        command[strlen(command)-1] = '\0';
        
        result_command = setup_command(command);

        WRONG = 0;
        setbuf(stdin,NULL);
        
    }

    return 0;
}

int setup_command(char *command){
    
    char *token, sub[10];
    int ind = -1;
    int any = 0;
    int counter = 0;

    token = strtok(command, SPACE);

    if(strcmp(token, LIST[NUMBER_OF_COMMANDS-1]) == 0){
            sleep(1);
            printf("Closing everything...\n");
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
        //end to backend the command
        printf("Executing command...\n");
        return 1;
    }

}