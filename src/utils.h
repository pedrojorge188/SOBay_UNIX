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

//Const's Fifos (Pipes)
#define FIFO_CLI "./tmp/cli_%d"
#define FIFO_SRV "./tmp/server_api"
#define USER_NOT_FOUND -1
#define USER_LOGIN_SUCCESS 1
#define CONNECT_TRUE 1
#define COMMAND_INFO 1
#define LOGIN_INFO 2

//Globals
#define MAX_USERS 20

int CONNECTED_USERS = 0;
int helper = 0;

typedef struct{
    int duration;
    int category;
    int promotion;
}promoter_info;

typedef struct{
    char name [15];
    char full_cmd [50];
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

typedef struct{
    int status; //1-command 2-login
    int pid;
    tryLogin login;
    command_info cmd;

}notification;

#endif