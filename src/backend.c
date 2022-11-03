#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include "backend.h"

int cmmChecker (char cmm){
    comdev Dev;

    char *token;
    int indx = -1;
    
    token = strtok(cmm, SPACE);

    for (int i = 0; i < 7; i++){
        if(strcmp(token, Dev.list[i])==0){
            indx = i;
        }
    }

    switch (indx){
        case (-1):
            printf("Introduced command was not recognized!");
            return 2; //command list error
            break;

        case (0):
            printf("Introduced command was not recognized!");
            return 2; //command list error
            break;

        case (1):
            printf("Introduced command was not recognized!");
            return 2; //command list error
            break;

        case (2):
            printf("Introduced command was not recognized!");
            return 2; //command list error
            break;

        case (3):
            printf("Introduced command was not recognized!");
            return 2; //command list error
            break;

        case (4):
            printf("Introduced command was not recognized!");
            return 2; //command list error
            break;

        case (5):
            printf("Introduced command was not recognized!");
            return 2; //command list error
            break; 

        case (6):
            printf("Introduced command was not recognized!");
            return 2; //command list error
            break;
    }

}



int main() {
    char CMM[250];

    printf("Espeta ai o comando: \n");
    scanf("%[^\n]", CMM);





    return 0;
}