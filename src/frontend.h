#ifndef FRONTEND_H
#define FRONTEND_H

#include "utils.h"


#define FOREVER 1
#define NUMBER_OF_COMMANDS 11
#define HEARTBEAT_TIMER 20;
#define MSG_TAM 50

//Error logs
#define LOGIN_ERROR "THE DATA ENTERED IS NOT CORRECT!\n"
#define KICK_LOG "YOU HAVE BEEN KICKED FROM YOUR SESSION!\n"
#define WRONG_INPUT "ENTER THE CORRECT INPUT (file name password)!\n"
#define WRONG_COMMAND "COMMAND (%s) IS NOT CORRECT!\n"
#define WRONG_VALUES "COMMAND (%s) VALUE ERROR!\n"


//Global Commands defualts
char *LIST[]= {"sell", "list", "licat", "lisel", "lival", "litime", "time", "buy", "cash", "add", "exit"};
int LIST_INDEX[] = {5, 0, 1, 1, 1, 1, 0, 2, 0, 1, 0};
char SPACE[] = {" "};
int WRONG = 0;

int setup_command(char *command);
int connect_server(tryLogin login,user MyAccount,char **argv);

#endif

