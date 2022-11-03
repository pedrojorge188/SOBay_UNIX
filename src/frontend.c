#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

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

    for(;;){
        
        fflush(stdin);

        int result_command;
        printf("cmd > : ");
        scanf("%[^\n]", command);
        
        result_command = setup_command(command);

        break;    
    }
    return 0;
}

int setup_command(char *command){
        
        char *token;
        int ind = 0;
        int counter = 1;

        token = strtok(command, DELIM);
        //printf ("%s\n", token);

        for (int i = 0; i < 10; i++){
            if(strcmp(token, LIST[i]) ==0 ){
                ind = i;
            }
        }

        for(int i = 0; i < LIST_INDEX[ind]; i++ ){
            token = strtok(NULL, DELIM);
            if(token != NULL)
                counter ++;
        }

        if(counter < LIST_INDEX[ind]){
             printf(WRONG_COMMAND, LIST[ind]);
             return 0;
        }else{
            /*Send to backend the command*/
            return 1;
        }
        
                   
}
