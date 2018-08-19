/* Rebecca Roughton
	2018 Summer
	Operating Systems
	Dr. Vineyard
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>

#define MAX_LINE 80
#define MAX_HIST 10


int run_args(char **  args);//Process user input
int execute (char ** args, int doRunInBG);// Executes normal command
int history_execute (char * arg);//history or ! entered
char ** parse_input(char * line);//Parse user input
int handle_child(char ** args);
int handle_parent(pid_t child, pid_t wpid);
static char* historyList[MAX_HIST];//List of history inputs
static int historyFirst; // first entry in history list, will get overwritten with more history inputs
static int historyCount;//number of history inputs
int backgroundCounter = 0; //increments and decrements with background processes
int doRunInBG = 0; //flag to run in background

int main(void)
{

	char **args; /* command line arguments*/
	char input[80];
	int shouldRun = 1; /* flag to determine when to exit program */
	int index;
	historyCount = 0;

	while(shouldRun){
				
		printf("\nrrsh>");
		fflush(stdout);
		
		//Get input
		fgets(input, MAX_LINE, stdin);
		input[strcspn(input, "\n")] = '\0';

		//Parse input
		args = parse_input(strdup(input));
		
		//Add to History List to call later
		if(strcmp(args[0], "history") != 0 && strstr(args[0], "!") == NULL){
			historyList[historyCount] = args[0];//input;
			historyCount++;
		}		

		//Run Command
		shouldRun = run_args(args);
		
	}
	return 0;
}

//
int run_args(char ** args){

	// Exit Command
	if(strcmp(args[0], "exit") == 0){
		return 0;
	}

	// History Command
	else if(strcmp(args[0], "history") == 0 || strstr(args[0], "!") != NULL){
		return history_execute(args[0]);

	}

	// Change doRunInBG flag so that program will run in background
	if(args[1] != NULL && strcmp(args[1], "&") == 0 ){
			doRunInBG = 1;
	}

	return execute(args, doRunInBG);
}


//Executes command
int execute (char ** args, int doRunInBG){
	pid_t child, wpid;

	child = fork();
	if(child == 0){
	//if is child
		handle_child(args);
	}
	else {
	//if is parent
		handle_parent(child, wpid);	
	}
	return 1;
}

// goes through parent path
int handle_parent(pid_t child, pid_t wpid)
{
	int status; // Used to keep track of status for waitpid()
	if(doRunInBG){
			backgroundCounter++;
			printf("\n%d\t%d" ,backgroundCounter, getpid());

	}else{
		do{
			// uses the waitpid method to wait for the child process to finish
			wpid = waitpid(child, &status, WUNTRACED);
		} while(!WIFEXITED(status) && !WIFSIGNALED(status));
	}
}

int handle_child(char ** args)
{
	// Handles issues with executing the child arguments
	if(execvp(args[0], args) == -1){
			printf("\nChild Error");			
			exit(EXIT_FAILURE);
	}
	if (doRunInBG)
	{	
		// Now that background task is finished, decriment the counter
		backgroundCounter--;
		printf("\ndone"); // background task done
	}
	exit(EXIT_FAILURE);
}


//Handles history
int history_execute(char * arg){
	char ** newArgs;
	int noArgs;
	int index;
	
	// checks iff the argument is the "history" command
	if(strcmp(arg, "history") == 0){
		int numInList = historyCount;	
		if(strcmp(arg, "history") == 0){
			for(index =historyFirst;index<historyFirst+MAX_HIST;index++){
				if(numInList > 0){
					printf("\n%d: %s", numInList, historyList[abs(index)%10]);
					numInList--;
				}	
			}
			printf("\n");
		}	
	}
	else if(strcmp(arg, "!!") == 0){
		newArgs = parse_input(strdup(historyList[historyFirst]));
	
		// By looping through, can count the number of arguments
		for(noArgs = 0;newArgs[noArgs] !=NULL;noArgs++){		
		}
		
		return run_args(newArgs);
	}
	else if(arg[0] == '!'){
		arg++;
		int num = atoi(arg);// Grabs the number to compare to the list in history
		if(num <= historyCount && num >= historyCount - MAX_HIST){
			num = historyCount - num;

			newArgs = parse_input(strdup(historyList[abs(num+historyFirst)%10]));
			// Loops through again to count
			for(noArgs = 0;newArgs[noArgs] !=NULL;noArgs++){		
			}
			return run_args(newArgs);

		}
		else{
			// Error handling for improper inputs
			printf("\nNumber Input Invalid");
			return 1;
		}
		return run_args(newArgs);
	}
	else{
		printf("\nInput Invalid");
	}
	return 1;
	
}

//Returns array with the input parsed out
char ** parse_input (char * userInput){
	
	// Will be assigned to args
	char **commands = (char**) malloc(sizeof(char*) * (10));
	
	char *command; 
	int counter = 0;
	command = strtok(userInput, " ");

	for(counter = 0; counter < 10; counter++)
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
	return commands;		
}
