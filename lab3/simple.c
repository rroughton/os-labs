#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <signal.h>

#define MAX_LINE    80
#define MAX_ARGS    MAX_LINE/2+1

char *args[MAX_ARGS];
int num_args;

int parse_input(char line[MAX_ARGS]);//Parse user input

int main(void)
{
    
    char input[MAX_LINE];
    int should_run = 1;
    int i;

    printf("\nrrsh>");
	fflush(stdout);
		
	//Get input
	fgets(input, MAX_LINE, stdin);
	//input[strcspn(input, "\n")] = '\0';

	//Parse input
	parse_input(strdup(input));
    for (i = 0; i < num_args; i++)
	{
		printf("\nAt position: %d, \t %s", i, args[i]);
		fflush(stdout);
	}

}


//Returns array with the input parsed out
int parse_input (char userInput[MAX_LINE]){
	
	char *command; 
	int counter = 0;
	int i = 0;

	command = strtok(userInput, " ");
	
	while (command != NULL)
	{
		args[i] = command;
		command = strtok(NULL, " ");
		printf("\n%d", i);
		printf("\n%s", command);
		fflush(stdout);
		i++
	}

    num_args = counter;	

	return 0;	
}