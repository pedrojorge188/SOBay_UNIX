#include "backend.h"

extern char **environ;

int main(int argc, char *argv[], char **envp)
{
    char command[MSG_TAM];
    items itemsList[MAX_ITEMS];

    //Guarda o nome dos ficheiros nas variaveis de ambiente
    init_env_var();

    setbuf(stdout,NULL);

    //Load dos items do ficheiro 
    if(!load_items((items *)&itemsList))
        printf(LOAD_ITEMS_ERROR);


    // Verificão de funcionalidades da meta 1
    printf("Meta 1 -> Deseja testar que funcionalidade? (startProm,startUsers,execItems)\n");

    while (VALID)
    {

        int result_command;
        printf("<ADMIN> -> ");
        scanf("%[^\n]", command);

        if (strcmp(command, "startProm") == 0)

            run_promoter("./black_friday");

        else if (strcmp(command, "startUsers") == 0){

            run_users();
        }
        else if (strcmp(command,"execItems") == 0)

             list_items_to_sell((items *)&itemsList);

        else if (setup_command(command) == 0)

            printf(WRONG_SINTAXE);

        setbuf(stdin, NULL);
    }

    return 0;
}

int load_items(items *itemsList){

    char *itemsFileName = getenv("FITEMS");
    
    int fItems,size;
    char itemBuffer[BUF_SIZE];
    char *token;

    fItems = open(itemsFileName, O_RDONLY);

    if (fItems == -1)
        printf(FILE_ERROR);

    size = read(fItems,itemBuffer, sizeof(itemBuffer));
    itemBuffer[size] = '\0';

    close(fItems);

    token = strtok(itemBuffer,SPACE);

    for(int item=0;item<NITEMS;item++){
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
        token = strtok(NULL,SPACE);

    }

    return 1;
}

void list_items_to_sell(items *itemsList){

    for(int i=0;i<NITEMS;i++){
        printf("%d %s %s %d  %d  %d  %s %s \n",
        itemsList[i].id,
        itemsList[i].name,
        itemsList[i].category,
        itemsList[i].current_price,
        itemsList[i].buy_now_price,
        itemsList[i].time_left,
        itemsList[i].username_owner,
        itemsList[i].username_best_option);
    }

}

int run_promoter(char *promoterName)
{

    char *promoFileName = getenv("FPROMOTERS");
    int fprom, size, value;
    int counter = 0;
    char promoBuffer[300];
    char *execPromo;
    promoter this_promoter;

    /*First Reading of promnoter*/
    fprom = open(promoFileName, O_RDONLY);

    if (fprom == -1)
    {
        printf(FILE_ERROR);
    }

    size = read(fprom,promoBuffer, sizeof(promoBuffer));
    promoBuffer[size] = '\0';

    close(fprom);

    while(promoBuffer[counter] != '\n'){
        execPromo[counter] = promoBuffer[counter];
        counter++;
    }

    execPromo[counter - 1] = '\0';

    printf("Executing Promoter(%s)\n", execPromo);

    execlp(execPromo, execPromo, NULL);
    
}

int run_users(){
    int opcao = -1, resultado, sal;
    char *user, *pass;

    char *userFileName = getenv("FUSERS");

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
                if(resultado == -1){
                    printf("%s\n", getLastErrorText());
                }else if(resultado == 0){
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