#ifndef BACKEND_H
#define BACKEND_H

#include "utils.h"
#include "users_lib.h"

#define FOREVER 1
#define RD 0
#define WR 1
#define MSG_TAM 50
#define NUMBER_OF_COMMANDS 7
#define BUF_SIZE 4000
#define NUM_PROMO 2

//Error logs
#define WRONG_COMMAND "COMMAND (%s) IS NOT CORRECT!\n"
#define WRONG_SINTAXE "COMMAND SINTAXE ERROR!\n"
#define FILE_ERROR "ERROR ON FILE OPENING!\n"
#define ERROR_QUIT 0
#define LOAD_ITEMS_ERROR "ERROR ON ITEMS LOADING!\n"
#define WRONG_VALUES "COMMAND (%s) VALUE ERROR!\n"

//Const's files
#define MAX_ITEMS 30
#define MAX_USERS 20
#define MAX_PROMOS 10
#define NUSERS 10
#define NPROMOTERS 10



//Global Commands defualts
char *LIST[] = {"users", "list", "kick", "prom", "reprom", "cancel", "close"};
int LIST_INDEX[] = {0, 0, 1, 0, 0, 1, 0};
char SPACE[] = {" "};
int WRONG = 0, CONNECTED_USERS = 0;


/*Reading Defauls*/
char *nameUsers[NUSERS];
char *namePromoters[NUSERS];

/*Setup all data structs*/
typedef struct client{
    char name[50];
    int password;
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

typedef struct promoter{
    char *category;
    int promotion;
    int duration;
}promoter;

int setup_command(char *command);
int run_promoter(char *promoterName);
void init_env_var();
void list_items_to_sell(items *itemsList);
int load_items(items *itemsList);
#endif