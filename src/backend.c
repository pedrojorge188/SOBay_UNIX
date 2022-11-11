#include "backend.h"

extern char **environ;

int main(int argc, char *argv[], char **envp){

   char command[MSG_TAM];
   setenv("FPROMOTERS","promo.txt",0);
   char *usersFileName = getenv("FUSERS");
   char *itemsFileName = getenv("FITEMS");
   
   // Verificão de funcionalidades da meta 1
   printf("Meta 1 -> Deseja testar que funcionalidade? (startProm,startUsers,execItems)\n");

    while(VALID){

        int result_command;
        printf("<ADMIN> -> ");
        scanf("%[^\n]", command);
        
        //Primeira Meta Validação da entrafa
        //Mais tarde esta funcionalidade ira desaparecer e os promotores serao executados automaticamente
        if(strcmp(command,"startProm") == 0)
            run_promoter(1);
        else if(setup_command(command) == 0)
            printf(WRONG_SINTAXE);
          
        setbuf(stdin,NULL);
    }
    return 0;
}

int run_promoter(int promId){
  
   char *promoFileName = getenv("FPROMOTERS");

   int fprom,size,resp;
   int counter=0;
   char promoBuffer[250];
   char execPromo[80];
   char stdout_mensage[80];

   promoter this_promoter;

  /*First Reading of promnoter*/
   fprom = open(promoFileName,O_RDONLY);
   if(fprom == -1){
      printf(FILE_ERROR);
   }

   size = read(fprom,promoBuffer,sizeof(promoBuffer));
   promoBuffer[size] = '\0';

   close(fprom);

    while(promoBuffer[counter] != '\n'){
        execPromo[counter] = promoBuffer[counter];
        counter++;
    }

   execPromo[counter-1] = '\0';
   printf("Executing Promoter:\t%s\n",execPromo);
   execlp(execPromo,execPromo,NULL);
}

int setup_command(char *command){
    
    char *token, sub[10];
    int ind = -1;
    int any = 0;
    int counter = 0;


    token = strtok(command, SPACE);

    if(strcmp(token, LIST[NUMBER_OF_COMMANDS-1]) == 0){
            sleep(1);
            printf("Closing everything...\n");
            exit(1);
    }

    for (int i = 0; i < NUMBER_OF_COMMANDS; i++){
        if(strcmp(token, LIST[i]) == 0){
            ind = i;
        }
    }

   switch(ind){
        case 2:
            token = strtok(NULL, SPACE);
            if (token == NULL)
                break;
            strcpy(sub, token);
            if(atoi(sub)!=0) {
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
            if(atoi(sub)!=0) {
                WRONG++;
                break;
            }
            counter++;
            break;

        default:
        //Nothing to be Verified
    }

    if(counter < LIST_INDEX[ind] || strtok(NULL, SPACE) != NULL || ind == -1 || WRONG == 1){ //Limite   inferior / superior

            if(WRONG > 0)
                printf(WRONG_VALUES, strtok(command, SPACE));
            else
                printf(WRONG_COMMAND, strtok(command, SPACE));
            return 0;
    }else{
        //end to backend the command
        printf("Executing command...\n");
        return 1;
    }

}
