#ifndef FRONTEND_H
#define FRONTEND_H



#define VALID 1
#define NUMBER_OF_COMMANDS 11

#define HEARTBEAT_TIMER 20;
#define MSG_TAM 50

//Error logs
#define LOGIN_ERROR "THE DATA ENTERED IS NOT CORRECT!\n";
#define KICK_LOG "YOU HAVE BEEN KICKED FROM YOUR SESSION!\n";
#define WRONG_INPUT "ENTER THE CORRECT INPUT (file name password)!\n"
#define WRONG_COMMAND "COMMAND (%s) IS NOT CORRECT!\n"
#define WRONG_VALUES "COMMAND (%s) VALUE ERROR!\n"

//Global Commands defualts
char *LIST[]= {"sell", "list", "licat", "lisel", "lival", "litime", "time", "buy", "cash", "add", "exit"};
int LIST_INDEX[] = {5, 0, 1, 1, 1, 1, 0, 2, 0, 1, 0};
char SPACE[] = {" "};
int WRONG = 0;

/*Setup all data structs*/
typedef struct user{
    char *name;
    int money;
    int id_items;
}user;

typedef struct tryLogin{
    int my_pid;
    char *username;
    char *psw;
}tryLogin;

int setup_command(char *command);

#endif

