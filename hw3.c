#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

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

//
//// freeCommands: frees memory used for arrays in **commands
//

void freeCommands(char **commands)
{
    // loop through arrays & free each
    int i;

    for (i = 0; i < 20; i++)
    {
        free(commands[i]);
    }
    
    return;
}

//////////////////////////////////////////////////////////////////////////

//
//// checkExitStatus: exit shell if input == "exit"
//

void checkExitStatus(char *input, char **commands)
{
    if (strcmp(input, "exit") == 0)
    {
       free(input);
       freeCommands(commands);   //free arrays in **commands
       free(commands);           //free array of arrays (**commands)
       exit(0);
    }
    else
    {
        return;
    }
}


//////////////////////////////////////////////////////////////////////////

//
//// processInput: converts input string into an array of strings and 
//// returns. Sets  
//

char **processInput(char *input, char** commands)
{

    // strtok each command into token and malloc token into the next
    // array position in **commands
    char *token = strtok(input, " ");
    int i = 0;

    while(token)
    {
        commands[i] = (char*)malloc(sizeof(char)*(strlen(token)+1));
        strcpy(commands[i], token);
        commands[i][(strlen(token)+1)] = '\0';

        token = strtok(NULL, " ");
        i = i + 1;
    }
    
    //add null character to next array position in **commands
    commands[(i+1)] = (char*)malloc(sizeof(char));
    commands[(i+1)][0] = '\0';
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


int main()
{
    // malloc space to hold user input and bash commands
    char *input = (char*)malloc(sizeof(char)*300);
    char **commands = (char**)malloc(sizeof(char*)*20);
    //flags
    //int chainFlag = 0;
    //int pipeFlag = 0;
    
    //while loop runs shell until "exit" is input
	while(1)
    {  
        input = shellPrompt(input);
        checkExitStatus(input, commands);
        
        commands = processInput(input, commands);
        processCommand(commands);
        
        freeCommands(commands);
    }
}
