#ifndef BACKEND_H
#define BACKEND_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

#define VALID 1
#define MSG_TAM 50
#define NUMBER_OF_COMMANDS 7

//Error logs
#define WRONG_COMMAND "COMMAND (%s) IS NOT CORRECT!\n"
#define WRONG_SINTAXE "COMMAND SINTAXE ERROR!\n"
#define FILE_ERROR "ERROR ON FILE OPENING!\n"
#define ERROR_QUIT 0
#define WRONG_VALUES "COMMAND (%s) VALUE ERROR!\n"

//Global Commands defualts
char *LIST[] = {"users", "list", "kick", "prom", "reprom", "cancel", "close"};
int LIST_INDEX[] = {0, 0, 1, 0, 0, 1, 0};
char SPACE[] = {" "};
int WRONG = 0;

/*Setup all data structs*/
typedef struct client{
    int user_pid;
    char *name;
    int password;
    int *id_own_items;
    int balance;
    bool connection;
    //client *pnext;
}client;

typedef struct items{
    int id;
    char *name;
    char *category;
    int buy_now_price;
    int main_price;
    int id_owner;
    bool sell_state;
    int time_left;
    //items *pnext;
}items;

int setup_command(char *command);
int run_promoter(int promID);
#endif