#include "backend.h"

extern char **environ;

int main(int argc, char *argv[], char **envp)
{
    char command[MSG_TAM];
    items itemsList[MAX_ITEMS];

    init_env_var();

    setbuf(stdout,NULL);


    while (FOREVER)
    {

        int result_command;
        
        printf("<ADMIN> -> ");

        fgets(command,MSG_TAM-1,stdin);

        command[strlen(command)-1] = '\0';

        if (strcmp(command, "startProm") == 0)

            run_promoter("./black_friday");

        else if (strcmp(command, "startUsers") == 0){

            run_users();
        }
        
        else if (strcmp(command,"execItems") == 0)
             
             load_items((items *)&itemsList);

        else if (setup_command(command) == 0)

            printf(WRONG_SINTAXE);

        setbuf(stdin, NULL);
    }

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

int getUsers(){
    char *usersFileName = getenv("FUSERS");
    
    int fUsers,size;
    char UserBuffer[BUF_SIZE];
    char *token;

    fUsers = open(usersFileName, O_RDONLY);

    if (fUsers == -1)
        printf(FILE_ERROR);

    size = read(fUsers,UserBuffer, sizeof(UserBuffer));
    UserBuffer[size] = '\0';

    close(fUsers);

    token = strtok(UserBuffer,SPACE);

    for(int i=0; i<NUSERS; i++){
        nameUsers[i] = token; // store Users name
        token = strtok(NULL,"\n"); //Get to the end of line
        token = strtok(NULL,SPACE); //Get users name
    }
    return 0;
}

int run_users(){
    int opcao = -1, resultado, sal;
    char *user, *pass;

    char *userFileName = getenv("FUSERS");

    getUsers();

    if(loadUsersFile(userFileName) == -1){
        printf("%s\n", getLastErrorText());
        exit(1);
    }

    do{
        printf("\nDigite numero da função pretendida:\n0 -> validarUser, 1 -> SaldoUser, 2 -> SaldoUpdateUser, 3 -> Sair\n");
        scanf("%d", &opcao);

        switch(opcao){
            case 0:
                printf("Qual o user e password a validar (user pass)?\n");
                scanf("%s %s", user, pass);

                resultado=isUserValid(user, pass);
                if(resultado == -1){
                    printf("%s\n", getLastErrorText());
                }else if(resultado == 0){
                    printf("User ou password erradas...\n");
                }else if(resultado == 1){
                    printf("O User existe e a password está certa\n");
                }

                break;

            case 1:
                printf("Qual o user a verificar saldo?\n");
                scanf("%s", user);

                resultado=getUserBalance(user);
                if(resultado == -1){
                    printf("%s\n", getLastErrorText());
                }else{
                    printf("O saldo do User é %d\n", resultado);
                }

                break;

            case 2:
                printf("Qual o user e valor a alterar no saldo (user value)?\n");
                scanf("%s %d", user, &sal);

                resultado=updateUserBalance(user, sal);
                if(resultado == 0){
                    printf("%s\n", getLastErrorText());
                }else if(resultado == -1){
                    printf("Saldo atualizado com sucesso\n");
                }
                break;

            case 3:
                resultado=saveUsersFile(userFileName);
                if(resultado == -1){
                    printf("%s\n", getLastErrorText());
                }else if(resultado == 0){
                    printf("Dados guardados com sucesso\n");
                }
                break;  

            default:
                break;
        }

        for (int i = 0; i < NUSERS; i++)
        {
            updateUserBalance(nameUsers[i], getUserBalance(nameUsers[i])-1);
        }

    }while(opcao != 3);
    
    return 0;
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
        exit(1);
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