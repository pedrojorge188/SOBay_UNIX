#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>
#include <signal.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<sys/select.h>
#include <pthread.h>

//Const's Fifos (Pipes)
#define FIFO_CLI "./tmp/cli_%d"
#define FIFO_SRV "./tmp/server_api"
#define USER_NOT_FOUND -1
#define USER_LOGIN_SUCCESS 1
#define CONNECT_TRUE 1

#define COMMAND_INFO 1
#define ITEM_INFO 1
#define PROMOTOR_INFO 2
#define LOGIN_INFO 2
#define INFO 3
#define N_THREADS 2

//Globals
#define MAX_USERS 20

int CONNECTED_USERS = 0;
int helper = 0;
int TIME = 0;
<<<<<<< Updated upstream
=======
int out = 0;
>>>>>>> Stashed changes

typedef struct{
    int id;
    char *name;
    char *category;
    char *username_owner;
    char *username_best_option;
    int buy_now_price;
    int current_price;
    bool sell_state;
    int time_left;
}item_list;

typedef struct{
    int duration;
    int category;
    int promotion;
}promoter_info;

typedef struct{
    char name[10];
    char command[50];
}command_info;

typedef struct{
    char name [50];
    int money;
    int status;
}user;

typedef struct{
    char username [50];
    char psw [50];
}tryLogin;

//frontend to backend comunication

typedef struct{

    int status; //1-command 2-login
    int pid;
    tryLogin login;
    command_info cmd;

}notification;

//backend to frontend comunication

typedef struct{

    int status; //1 - itemInformation 2-promotor_info 3-other
    char message[50]; 
    item_list items;
    promoter_info promo;

}info;
#endif