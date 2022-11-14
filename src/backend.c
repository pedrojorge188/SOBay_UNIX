#include "backend.h"

extern char **environ;

int main(int argc, char *argv[], char **envp)
{

    char command[MSG_TAM];
    items itemsList[MAX_ITEMS];

    //Guarda o nome dos ficheiros nas variaveis de ambiente
    init_env_var();

    //Load dos items do ficheiro 
    if(!load_items((items *)&itemsList))
        printf(LOAD_ITEMS_ERROR);

   // list_items_to_sell((items *)&itemsList);

    // Verificão de funcionalidades da meta 1
    printf("Meta 1 -> Deseja testar que funcionalidade? (startProm,startUsers,execItems)\n");

    while (VALID)
    {

        int result_command;
        printf("<ADMIN> -> ");
        scanf("%[^\n]", command);

        if (strcmp(command, "startProm") == 0)

            run_promoter(1);

        else if (setup_command(command) == 0)

            printf(WRONG_SINTAXE);

        setbuf(stdin, NULL);
    }
    return 0;
}

int load_items(items *itemsList){

    char *itemsFileName = getenv("FITEMS");
    
    int fItems,size;
    char itemBuffer[BUFITEMS_SIZE];
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

int run_promoter(int promId)
{

    char *promoFileName = getenv("FPROMOTERS");

    int fprom, size, resp;
    int counter = 0;
    char promoBuffer[250];
    char execPromo[80];
    char stdout_mensage[80];

    promoter this_promoter;

    /*First Reading of promnoter*/
    fprom = open(promoFileName, O_RDONLY);
    if (fprom == -1)
    {
        printf(FILE_ERROR);
    }

    size = read(fprom, promoBuffer, sizeof(promoBuffer));
    promoBuffer[size] = '\0';

    close(fprom);

    while (promoBuffer[counter] != '\n')
    {
        execPromo[counter] = promoBuffer[counter];
        counter++;
    }

    execPromo[counter - 1] = '\0';
    printf("Executing Promoter:\t%s\n", execPromo);
    execlp(execPromo, execPromo, NULL);
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
    setenv("FITEMS","items.txt",0);
}