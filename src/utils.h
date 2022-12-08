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
#define FIFO_SRV "./tmp/fifo_login"
#define USER_NOT_FOUND -1
#define USER_LOGIN_SUCCESS 1
#define CONNECT_TRUE 1

//Globals
#define MAX_USERS 20

int CONNECTED_USERS = 0;
int server_pid = 0;

typedef struct{
    char name [50];
    int money;
    int status;
}user;

typedef struct{
    int my_pid;
    char username [50];
    char psw [50];
}tryLogin;

#endif