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
        int counter2 = 0;

        token = strtok(command, DELIM);
        
        if(strcmp(token,"close") == 0){
             sleep(1);
             printf("Closing everything...\n");
             exit(1);
        }

        for (int i = 0; i < strlen((char *)&LIST); i++){
            if(strcmp(token, LIST[i]) == 0){
                ind = i;
                any++;   //Verify if command even existes
            }
        }

        for(int i = 0; i < LIST_INDEX[ind]; i++ ){
            token = strtok(NULL, DELIM);
            if(token != NULL)
                counter1 ++;
        }
    
        if(counter1 < LIST_INDEX[ind] || strtok(NULL, DELIM) != NULL){ //Limite   inferior / superior

             if(any == 0)
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

