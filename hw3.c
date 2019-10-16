#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#define BUFFERSIZE 300

/*
 *	CS 361 Homework 3: The ch2shell
 *  Cody Hasty
 *  chasty2
 *  663068521
 *	Written on Ubuntu 18.04 with vim 8 on systems1.cs.uic.edu 
 *  UIC Fall 2019
 */

//////////////////////////////////////////////////////////////////////////

//
//// shellPrompt: prints command prompt and returns string of user input.
//

char *shellPrompt(char *input)
{
    printf("CS361 >");
    fgets(input, BUFFERSIZE, stdin);
    input[strcspn(input, "\r\n")] = '\0';   // strip EOL characters

    return input;
}

//////////////////////////////////////////////////////////////////////////
/*
//
//// sigint_handler: function for catching SIGINT
//

void sigint_handler(int sig)
{
  char catchMessage[] = "\ncaught sigint\n";
  char prompt[] = "CS361 >";
  write(1,catchMessage,sizeof(catchMessage));
  write(1,prompt,sizeof(prompt));
}

//////////////////////////////////////////////////////////////////////////

//
//// sigstp_handler: function for catching SIGTSTP
//

void sigtstp_handler(int sig)
{
  char catchMessage[] = "\ncaught sigstp\n";
  char prompt[] = "CS361 >";
  write(1,catchMessage,sizeof(catchMessage));
  write(1,prompt,sizeof(prompt));
}
*/
//////////////////////////////////////////////////////////////////////////


//
//// freeCommands: frees memory used for arrays in **commands
////               
//

void freeCommands(char **commands, int size)
{
    /*
    NOTE: if ptr is null, no free occurs. This is a problem because
    I set commands[size+1][0] = '\0', and it is not being freed.
    This causes errors downstream. So that is why it is important 
    to set malloc'ed null pointers to a value before freeing them
    */

    //commands[*size][0] = 'c';

    // loop through arrays & free each
    int i;
    for (i = 0; i < size; i++)
    {
        free(commands[i]);
    }
    return;
}

//////////////////////////////////////////////////////////////////////////

//
//// resetFlags: sets size, chainFlag, and pipeFlag to zero
//

void resetFlags(int *size, int *chainFlag, int *pipeFlag)
{
    *size = 0;
    *chainFlag = 0;
    *pipeFlag = 0;
}

//////////////////////////////////////////////////////////////////////////


//
//// freeMemoryAndExit: frees all malloc'd memory and exits shell
//

void freeMemoryAndExit(char *input, char **commands)
{
    free(input);
    free(commands);
    exit(0);
}

//////////////////////////////////////////////////////////////////////////

//
//// checkExitStatus: exit shell if input == "exit"
//

int checkExitStatus(char *input)
{
    if (strcmp(input, "exit") == 0)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}


//////////////////////////////////////////////////////////////////////////

//
//// processInput: converts input string into an array of strings and 
//// returns. Sets  
//

char **processInput(char *input, char** commands, int *size,
                                int *chainFlag, int *pipeFlag)
{

    // strtok each command into token and malloc token into the next
    // array position in **commands
    char *token = strtok(input, " ");
    int i;

    while(token)
    {
        commands[i] = (char*)malloc(sizeof(char)*(strlen(token)+1));
        strcpy(commands[i], token);
        commands[i][(strlen(token)+1)] = '\0';
        
        // flag ; and | characters
        if (strchr(token, ';') != NULL)
        {
            *chainFlag = i;
        }
        else if (strchr(token, '|') != NULL)
        {
            *pipeFlag = i;
        }

        token = strtok(NULL, " ");
        i++;
    }
    
    //add null character to next array position in **commands
    // NOTE: next position is null character by default
    //commands[(i)] = (char*)malloc(sizeof(char));
    //commands[(i)][0] = '\0';

    *size = i;
    return commands;
}

//////////////////////////////////////////////////////////////////////////

//
//// processCommand: executes a single bash command. Accepts flags
//

void processCommand(char **commands)
{
    pid_t pid = fork();
    // child process executes command
    if (pid == 0)   
    {
        execv(commands[0], commands);
        _exit(EXIT_SUCCESS);
    }
    //parent process prints status child
    else
    {
        int status;
        wait(&status);
        printf("pid:%d status:%d\n", pid, WEXITSTATUS(status));
    }

    return;
}

//////////////////////////////////////////////////////////////////////////

//
//// processChain: splits **commands at ; into two arrays, and calls 
////               processCommand on each
//

void processChain(char **commands, int chainFlag, int size)
{
   //declare array sizes
   int array1Size = chainFlag + 1;
   int array2Size = size - chainFlag;

   //malloc each array
   char **array1 = (char**)malloc(sizeof(char*)*(array1Size));
   char **array2 = (char**)malloc(sizeof(char*)*(array2Size));
   
   //populate array1
   int i;
   for(i = 0; i < (array1Size - 1); i++)
   {
        array1[i] = (char*)malloc(sizeof(char)*strlen(commands[i]));
        strcpy(array1[i],commands[i]);
   }
    
   //populate array2 
   int j = array1Size;
   for( i = 0; i < (array2Size - 1); i++)
   {
        array2[i] = (char*)malloc(sizeof(char)*strlen(commands[j]));
        strcpy(array2[i],commands[j]);
        j++;
   }

   //run commands
   processCommand(array1);
   processCommand(array2);

   //free strings
   freeCommands(array1, (array1Size - 1));
   freeCommands(array2, (array2Size - 1));

   //free arrays of pointers
   free(array1);
   free(array2);

   return;

}


//////////////////////////////////////////////////////////////////////////



int main()
{
    //signals
    //signal(SIGINT, sigint_handler);
    //signal(SIGTSTP, sigtstp_handler);

    // malloc space to hold user input and array of command pointers
    char *input = (char*)malloc(sizeof(char)*300);
    char **commands = (char**)malloc(sizeof(char*)*20);
    
    //size counter for commands array
    int size = 0;

    // flags
    int chainFlag = 0;
    int pipeFlag = 0;
    
    //while loop runs shell until "exit" is input
	while(1)
    {   
        input = shellPrompt(input);
        
        if (checkExitStatus(input) == 0)
        {
            freeMemoryAndExit(input, commands);
        }
        else
        {
            commands = processInput(input, commands, &size,
                                    &chainFlag, &pipeFlag);
            if (chainFlag != 0)
            {
                processChain(commands, chainFlag, size);
            }
            else
            {
                processCommand(commands);
            }

        }

        freeCommands(commands, size);
        resetFlags(&size, &chainFlag, &pipeFlag); 
    }
}
