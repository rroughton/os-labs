#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <signal.h>

#define MAX_LINE    80
#define MAX_ARGS    MAX_LINE/2+1



int num_args;
char * parse_input(char * line);//Parse user input

int main(void)
{
    char *args[MAX_ARGS];
    char input[MAX_LINE];
    int should_run = 1;

    printf("\nrrsh>");
	fflush(stdout);
		
	//Get input
	fgets(input, MAX_LINE, stdin);
	input[strcspn(input, "\n")] = '\0';

	//Parse input
	args = parse_input(strdup(input));
    int i;
    for (i = 0; i < )

}


//Returns array with the input parsed out
char * parse_input (char * userInput){
	
	// Will be assigned to args
	char *commands = (char*) malloc(sizeof(char*) * (10));
	
	char *command; 
	int counter = 0;
	command = strtok(userInput, " ");

	for(counter = 0; counter < 41; counter++)
	{
		commands[counter] = NULL;
	}	
	
	counter = 0;

	//
	while(command!= NULL)
	{
		commands[counter] = command;
		command = strtok(NULL, " ");
		counter++;
	}

	counter--;
	// checks if & was the last input
	if (strcmp(commands[counter], "&") != 0)
	{
		doRunInBG == 1;
	}

    num_args = counter;
	return commands;		
}