#ifndef BACKEND_H
#define BACKEND_H

#include <stdbool.h>
#define VALID 1
#define MSG_TAM 50
#define NUMBER_OF_COMMANDS 7

//Error logs
#define WRONG_COMMAND "COMMAND (%s) IS NOT CORRECT!\n"
#define WRONG_SINTAXE "COMMAND SINTAXE ERROR!\n"

//Global Commands defualts
char *LIST[] = {"users", "list", "kick", "prom", "reprom", "cancel", "close"};
int LIST_INDEX[] = {0, 0, 1, 0, 0, 1, 0};
char SPACE[] = {" "};

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

#endif