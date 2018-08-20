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
int flags[10];

int set_flags(void);
int run_args(void);

int main(void)
{
    
    char input[MAX_LINE];
    int should_run = 1;
    int i = 0;

	while (should_run)
	{
		printf("\nrrsh>");
		fflush(stdout);
			
		//Get input
		fgets(input, sizeof input, stdin);
		char *tok = strtok(input, " ");

		while (tok != NULL)
		{
			args[i] = tok;
			tok = strtok(NULL, " ");
			i++;
		}
		num_args = i;

		should_run = set_flags();
		if (should_run)
		{
			run_args();
		}
	}
	return 0;
}

int set_flags()
{
	if (strcmp(args[0], "exit") == 0)
	{ 
		printf("\nExiting program");
		fflush(stdout);
		return 0; // change should_run to 0
	}

	if (strcmp(args[0], "history") == 0)
	{
		printf("\nDo History");
		fflush(stdout);
		flags[0] = 1;
	} else {
		flags[0] = 0;
	}

	if (strcmp(args[num_args - 1], "&") == 0)
	{
		printf("\nRun in Background");
		fflush(stdout);
		flags[1] = 1;
	} else {
		flags[1] = 0;
	}

	char first_string[strlen(args[0])];
	strcpy(first_string,args[0]);
	if ( strcmp(first_string[(strlen(first_string)-1)], "!") == 0)
	{
		printf("\nExecute history command");
		fflush(stdout);
		flags[2] = 1;
	} else {
		flags[2] = 0;
	}

	return 1;
}

int run_args(void)
{

}