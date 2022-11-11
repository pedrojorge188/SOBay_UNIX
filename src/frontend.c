
#include "frontend.h"

int main(int argc, char **argv) {

    tryLogin login;
    char command[MSG_TAM];
    
    if(argc < 3){
         printf(WRONG_INPUT);
         exit(1);
    }

    login.my_pid = getpid();
    login.username = argv[1];
    login.psw = argv[2];
    
    printf("ACCOUNT(pid:%d|username:%s|password:%s) \n",
    login.my_pid, login.username, login.psw );

    while(VALID){

        int result_command;
        printf("<CLIENT: %s> : ",login.username);
        scanf("%[^\n]", command);
        
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

        default:
        //Nothing to be Verified
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