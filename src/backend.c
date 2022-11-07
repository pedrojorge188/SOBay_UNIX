#include "backend.h"

extern char **environ;

int main(int argc, char *argv[], char **envp){

   char command[MSG_TAM];
   char *usersFileName = getenv("FUSERS");
   char *itemsFileName = getenv("FITEMS");
   
   // Verificão de funcionalidades da meta 1
   printf("Meta 1 -> Deseja testar que funcionalidade? (startProm,startUsers,execItems)\n");

    while(VALID){

        int result_command;
        printf("<ADMIN> -> ");
        scanf("%[^\n]", command);
        
        //verificação para a meta 1:
        if(strcmp(command,"startProm") == 0)
            run_promoter(1);
        else{
            if(setup_command(command) == 0)
            printf(WRONG_SINTAXE);
        }    
        
        setbuf(stdin,NULL);
    }
    return 0;
}

int run_promoter(int promID){

   char *promoFileName = getenv("FPROMOTERS");

   int fprom;
   char promoBuffer[70];

   fprom = open(promoFileName,O_RDONLY);

   if(fprom == -1){
      printf(FILE_ERROR);
      exit(ERROR_QUIT);
   }

   read(fprom,promoBuffer,6);

   promoBuffer[69] = '\0';

   printf("Executing Promoter:%s\n",promoBuffer);

   execl(promoBuffer,promoBuffer,NULL);

   close(fprom);

}

int setup_command(char *command){
        
        char *token;
        int ind = -1;
        int any = 0;
        int counter1 = 0;
        int helper[15];
        
        token = strtok(command, SPACE);
        
        if(strcmp(token,LIST[NUMBER_OF_COMMANDS-1]) == 0){
             sleep(1);
             printf("Closing everything...\n");
             exit(ERROR_QUIT);
        }

        for (int i = 0; i < NUMBER_OF_COMMANDS; i++){
            if(strcmp(token, LIST[i]) == 0){
                ind = i;
            }
        }

        for(int i = 0; i < LIST_INDEX[ind]; i++ ){
            token = strtok(NULL, SPACE);
            if(token != NULL)
                counter1 ++;
        }

        if(counter1 < LIST_INDEX[ind] || strtok(NULL, SPACE) != NULL || ind == -1){ 

            printf(WRONG_COMMAND, strtok(command,SPACE));
            return 0;

        }else{
            printf("Executing command...\n");
            return 1;
        }                
}

