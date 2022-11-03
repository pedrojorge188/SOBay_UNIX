#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include "backend.h"


int main() {
    char command[250];
    char *list[] = {"users", "list", "kick", "prom", "reprom", "cancel", "close"};
    int number[] = {0, 0, 1, 0, 0, 1, 0};
    const char delim[] = {" "};
    char *token;
    int ind = 0;

    printf("Espeta ai o comando: \n");
    scanf("%[^\n]", command);

    token = strtok(command, delim);
    printf ("%s\n", token);

    for (int i = 0; i < 7; i++){
        if(strcmp(token, list[i])==0){
            ind = i;
        }
    }

    for(int i = 0; i < number[ind]; i++ ){
        token = strtok(NULL, delim);
        printf ("%s\n", token);

    }



    return 0;
}