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
#define BUFITEMS_SIZE 4000

//Error logs
#define WRONG_COMMAND "COMMAND (%s) IS NOT CORRECT!\n"
#define WRONG_SINTAXE "COMMAND SINTAXE ERROR!\n"
#define FILE_ERROR "ERROR ON FILE OPENING!\n"
#define ERROR_QUIT 0
#define LOAD_ITEMS_ERROR "ERROR ON ITEMS LOADING!\n"
#define WRONG_VALUES "COMMAND (%s) VALUE ERROR!\n"
#define MAX_ITEMS 30

//Global Commands defualts
char *LIST[] = {"users", "list", "kick", "prom", "reprom", "cancel", "close"};
int LIST_INDEX[] = {0, 0, 1, 0, 0, 1, 0};
char SPACE[] = {" "};
int WRONG = 0;
int NITEMS = 6;

/*Setup all data structs*/
typedef struct client{
    int user_pid;
    char *name;
    int password;
    int *id_own_items;
    int balance;
    bool connection;
}client;

typedef struct items{
    int id;
    char *name;
    char *category;
    char *username_owner;
    char *username_best_option;
    int buy_now_price;
    int current_price;
    bool sell_state;
    int time_left;
}items;

typedef struct promoter
{
    char *category;
    int promotion;
    int duration;
}promoter;

int setup_command(char *command);
int run_promoter(int promId);
void init_env_var();
void list_items_to_sell(items *itemsList);
int load_items(items *itemsList);
#endif