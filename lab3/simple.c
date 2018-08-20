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

char * parse_input(char * line);//Parse user input

int main(void)
{
    
    char input[MAX_LINE];
    int should_run = 1;

    printf("\nrrsh>");
	fflush(stdout);
		
	//Get input
	fgets(input, MAX_LINE, stdin);
	input[strcspn(input, "\n")] = '\0';

	//Parse input
	parse_input(strdup(input));
    int i;
    for (i = 0; i < num_args; i++)
	{
		printf("\nAt position: %d, \t %s", i, args[i]);
	}

}


//Returns array with the input parsed out
int parse_input (char * userInput){
	
	char *command; 
	int counter = 0;
	command = strtok(userInput, " ");

	for(counter = 0; counter < 41; counter++)
	{
		args[counter] = NULL;
	}	
	
	counter = 0;

	//
	while(command!= NULL)
	{
		args[counter] = command;
		command = strtok(NULL, " ");
		counter++;
	}

	counter--;
    num_args = counter;	

	return 0;	
}