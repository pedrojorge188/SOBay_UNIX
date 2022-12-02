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
#define FIFO_SRV "./tmp/adminPipe"
#define USER_NOT_FOUND 0
#define USER_LOGIN_SUCCESS 1
#define LIMIT_USERS -1


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