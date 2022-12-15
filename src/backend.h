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
#define MAX_PROMOS 10
#define NPROMOTERS 10
#define RANGE 500

//MAX USERS -> utils.h ...

//Global Commands defualts
char *LIST[] = {"users", "list", "kick", "prom", "reprom", "cancel", "close"};
int LIST_INDEX[] = {0, 0, 1, 0, 0, 1, 0};
char SPACE[] = {" "};

int WRONG = 0;
int out = 0;

/*Reading Defauls*/
int pidUsers[RANGE];
char *namePromoters[NPROMOTERS];

/*Setup all data structs*/
typedef struct client{
    int pid;
    char name[50];
    int balance;
    bool connection;
}client;


typedef struct items{
    int id;
    char name[20];
    char category[20];
    char username_owner[20];
    char username_best_option[20];
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
void list_users(client *users);
void list_items_to_sell(items *itemsList);
int load_items(items *itemsList);
void list_items(items *itemsList,int nItems);
void disconnect_users();
void kick_user(char *username,client *users);
void fill_users(client *users);
int get_ind(client *users);

#endif