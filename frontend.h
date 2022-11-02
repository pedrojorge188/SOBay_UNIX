#ifndef FRONTEND_H
#define FRONTEND_H

#define HEARTBEAT_TIMER 20;
#define LOGIN_ERROR "THE DATA ENTERED IS NOT CORRECT!\n";
#define KICK_LOG "YOU HAVE BEEN KICKED FROM YOUR SESSION!\n";
#define WRONG_INPUT "ENTER THE CORRECT INPUT (file name password)!\n"

/*Setup all data structs*/
typedef struct user{
    char **name;
    int money;
    int id_items;
}user;

#endif

