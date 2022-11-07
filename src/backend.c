#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include "backend.h"

int main(){

    char command[MSG_TAM];

    while(VALID){

        int result_command;
        printf("<ADMIN> -> ");
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
        int helper[15];
        token = strtok(command, SPACE);
        
        if(strcmp(token,LIST[NUMBER_OF_COMMANDS-1]) == 0){
             sleep(1);
             printf("Closing everything...\n");
             exit(1);
        }

        for (int i = 0; i < NUMBER_OF_COMMANDS; i++){
            if(strcmp(token, LIST[i]) == 0){
                ind = i;
            }
        }

        for(int i = 0; i < LIST_INDEX[ind]; i++ ){
            token = strtok(NULL, SPACE);
            if(token != NULL)
                counter1 ++;
        }

        if(counter1 < LIST_INDEX[ind] || strtok(NULL, SPACE) != NULL || ind == -1){ 

            printf(WRONG_COMMAND, strtok(command,SPACE));
            return 0;

        }else{
            printf("Executing command...\n");
            return 1;
        }                
}

