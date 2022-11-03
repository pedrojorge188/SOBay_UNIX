#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#include "frontend.h"


int main(int argc, char **argv) {


    tryLogin login;
    char *command[MSG_TAM];
    //setup input verification

    if(argc < 3){
         printf(WRONG_INPUT);
         exit(1);
    }


    login.my_pid = getpid();
    login.username = argv[1];
    login.psw = argv[2];
    

    while(VALID){

        fflush(stdin);
        printf("->");
        scanf("%s",&command);

        if(strcmp(*command,"list") == 0)
            list_users(&command);
    }

    return 0;
}