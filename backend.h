#ifndef BACKEND_H
#define BACKEND_H
#include <stdbool.h>

#define ROOTNAME "admin";

/*Setup all data structs*/
typedef struct client{
    int user_pid;
    char **name;
    int password;
    int *id_own_items;
    int money;
    bool connection;
    client *pnext;
}client;

typedef struct items{
    int id;
    char **name;
    char **category;
    int buy_now_price;
    int main_price;
    int id_owner;
    bool sell_state;
    int time_left;
    items *pnext;
}items;


#endif