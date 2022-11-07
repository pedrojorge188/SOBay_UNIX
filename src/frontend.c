#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include "frontend.h"

int main(int argc, char **argv) {


    tryLogin login;
    char command[MSG_TAM];
    

    //setup input verification

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

        setbuf(stdin,NULL);
    }

    return 0;
}

int setup_command(char *command){
        
        char *token;
        int ind = -1;
        int any = 0;
        int counter1 = 0;
        int counter2 = 0;

        token = strtok(command, DELIM);
        
        if(strcmp(token,LIST[NUMBER_OF_COMMANDS-1]) == 0){
             sleep(1);
             printf("Closing everything...\n");
             exit(1);
        }

        for (int i = 0; i < NUMBER_OF_COMMANDS; i++){
            if(strcmp(token, LIST[i]) == 0){
                ind = i;
                //any++;   //Verify if command even existes
            }
        }


        for(int i = 0; i < LIST_INDEX[ind]; i++ ){
            token = strtok(NULL, DELIM);
            if(token != NULL)
                counter1 ++;
        }
    
        if(counter1 < LIST_INDEX[ind] || strtok(NULL, DELIM) != NULL){ //Limite   inferior / superior

             if(ind == -1)
                printf(WRONG_COMMAND, strtok(command, DELIM));
             else
                printf(WRONG_COMMAND, LIST[ind]);
             
             return 0;

        }else{
            /*Send to backend the command*/
            printf("Executing command...\n");
            return 1;
        }
        
                   
}