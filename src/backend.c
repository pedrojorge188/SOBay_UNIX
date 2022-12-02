#include "backend.h"
extern char **environ;
 
int main(int argc, char *argv[], char **envp)
{   
    int res,nBytes,fd;
    char command[MSG_TAM],fifo_cli[50];
    
    items itemsList[MAX_ITEMS];
    tryLogin login;
    client users[MAX_USERS];
    fd_set fds;
    
    init_env_var();
    printf("<SERVER> Running....");

    setbuf(stdout,NULL);

    if(access(FIFO_SRV, F_OK) != 0) {
        mkfifo(FIFO_SRV, 0666);
    }

    do{

        fd = open(FIFO_SRV,O_RDWR);

        if(fd == -1){
            printf("ERROR TO OPEN SERVER PIPE");
            unlink(FIFO_SRV);
            exit(EXIT_FAILURE);
        }

        FD_ZERO(&fds); //reset fds pointer 
        FD_SET(0,&fds);  //define input as stdin
        FD_SET(fd,&fds); //define pipe input (in this case we'r refering to fd)

        printf("\n<ADMIN>");

        res = select(fd+1,&fds,NULL,NULL,NULL);

        if(res == 0){

                 printf("<SERVER> ANY DATA TO READ!");

        }else if(res > 0 && FD_ISSET(0,&fds)){


                fgets(command,MSG_TAM-1,stdin);
                command[strlen(command)-1] = '\0';

                if (setup_command(command) == 0)
                    printf(WRONG_SINTAXE);

        }else if(res > 0 && FD_ISSET(fd,&fds)){

                char *userFileName = getenv("FUSERS");

                nBytes = read(fd,&login,sizeof(tryLogin));

                char try_psw[50],try_username[50];
                int user_balance;

                for (int i=0; login.username[i] != '\0'; i++)
                     try_username[i] = login.username[i];

                for(int i=0;login.psw[i] != '\0';i++)
                      try_psw[i] = login.psw[i];

                if(loadUsersFile(userFileName) == -1){
                    printf("%s\n", getLastErrorText());
                    exit(1);
                }

                if(isUserValid(try_psw,try_username) == 1)

                    printf("\n(pid:%d)->Login\n",login.my_pid);

                else {

                    printf("\n(pid:%d)->Login Failed\n",login.my_pid);

                }

        }else{

            printf("ERROR (SELECT)\n");
            unlink(FIFO_SRV);
            exit(EXIT_FAILURE);
        }

        setbuf(stdin, NULL);
        close(fd);

    }while(command != "close");

    unlink(FIFO_SRV);

    return 0;
}

int load_items(items *itemsList){

    char *itemsFileName = getenv("FITEMS");
    
    FILE   *fItems;
    char itemBuffer[150];
    int counter = 0;
    char *token;

    fItems = fopen(itemsFileName,"r");

    if (!fItems)
        printf(FILE_ERROR);

    char line[100];

    int item = 0;

    while(fgets(itemBuffer,sizeof(itemBuffer),fItems)){

        printf("-%s-",itemBuffer);
        token = strtok(itemBuffer,SPACE);
        itemsList[item].id = atoi(token); // id
        token = strtok(NULL,SPACE);
        itemsList[item].name = token; // nome do item
        token = strtok(NULL,SPACE);
        itemsList[item].category = token; // categoria do item
        token = strtok(NULL,SPACE);
        itemsList[item].current_price = atoi(token); // valor inicial/licitacao mais elevada
        token = strtok(NULL,SPACE);
        itemsList[item].buy_now_price = atoi(token); //valor da compra imediata
        token = strtok(NULL,SPACE);
        itemsList[item].time_left = atoi(token); //duração/tempo restante do leilao
        token = strtok(NULL,SPACE);
        itemsList[item].username_owner = token; // username do vendedor
        token = strtok(NULL,"\n");
        itemsList[item].username_best_option = token; //username da melhor opcao de comprador

        item++;

    }

    printf("\n");

    fclose(fItems);

    return 1;
}

int getPromoters(){

    char *promoFileName = getenv("FPROMOTERS");
    FILE * f;
    int prom = 0;
    char Buffer[120];
    
    f = fopen(promoFileName,"r");

    if (!f)
        printf(FILE_ERROR);
    
    while(fgets(Buffer,sizeof(Buffer),f)){

        namePromoters[prom] = Buffer;
        prom++;

    }

    fclose(f);
    
    return 1;
}

int run_promoter(char *promoterName)
{
    char *promoter1;
    char mensage_promoter[20];

    //getPromoters(); // Na meta 2 vamos usar para ler o ficheiro promoters

    promoter1 = "./black_friday";

    int tube[2];
    
    pipe(tube);

    if(fork() == 0){
        
        close(1);
        dup(tube[WR]);
        close(tube[WR]);
        close(tube[RD]);
        
        execlp(promoter1,promoter1,NULL);
        
    }else{

        close(tube[WR]);

        while(1){

            if(read(tube[RD],mensage_promoter,sizeof(mensage_promoter)) >= 0){

                for(int i=0;i<strlen(mensage_promoter)-1;i++)
                    printf("%c",mensage_promoter[i]);

                printf("->PROMOTER RECIVED!\n");
            }

        }
        
        close(tube[RD]);
    }
    
}

int setup_command(char *command)
{

    char *token, sub[10];
    int ind = -1;
    int any = 0;
    int counter = 0;

    token = strtok(command, SPACE);

    if (strcmp(token, LIST[NUMBER_OF_COMMANDS - 1]) == 0)
    {
        sleep(1);
        printf("Closing everything...\n");
        exit(EXIT_SUCCESS);
    }

    for (int i = 0; i < NUMBER_OF_COMMANDS; i++)
    {
        if (strcmp(token, LIST[i]) == 0)
        {
            ind = i;
        }
    }

    switch (ind)
    {
    case 2:
        token = strtok(NULL, SPACE);
        if (token == NULL)
            break;
        strcpy(sub, token);
        if (atoi(sub) != 0)
        {
            WRONG++;
            break;
        }
        counter++;
        break;

    case 5:
        token = strtok(NULL, SPACE);
        if (token == NULL)
            break;
        strcpy(sub, token);
        if (atoi(sub) != 0)
        {
            WRONG++;
            break;
        }
        counter++;
        break;

    default:
        // Nothing to be Verified
    }

    if (counter < LIST_INDEX[ind] || strtok(NULL, SPACE) != NULL || ind == -1 || WRONG == 1)
    { // Limite   inferior / superior

        if (WRONG > 0)
            printf(WRONG_VALUES, strtok(command, SPACE));
        else
            printf(WRONG_COMMAND, strtok(command, SPACE));
        return 0;
    }
    else
    {
        // end to backend the command
        printf("Executing command...\n");
        return 1;
    }
}

void init_env_var(){
    setenv("FPROMOTERS", "promo.txt", 0);
    setenv("FITEMS","items.txt", 0);
    setenv("FUSERS","users.txt", 0);
}