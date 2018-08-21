/* Rebecca Roughton
	Project 3
	Thurday Lab Section
*/

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <signal.h>

#define MAX_LINE    80
#define MAX_ARGS    MAX_LINE/2+1
#define MAX_BG		10

struct history_element
{
	char *full_command[MAX_ARGS];
	int number;
};

struct background_element
{
	pid_t pid;
	int number;
	char *full_command[MAX_ARGS];
};

struct history_element history_list[10];
struct background_element background_list[MAX_BG];

int num_background;
int num_history;
char *args[MAX_ARGS];
int num_args;
int flags[10];
char *done_strs[MAX_ARGS];
int num_done_strs = 0;



int set_flags(void);
int run_args(void);
int execute(void);
void handle_done_background(void);
void print_done(void);
void remove_bg_elem(pid_t pid);

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
			printf("args at 0 : %s", args[0]);
			fflush(stdout);
			run_args();
		}
	}
	return 0;
}

int set_flags()
{
	printf("\nargs[0]: %s", args[0]);
	fflush(stdout);
	fflush(stdout);
	if (strcmp(args[0], "exit\n") == 0)
	{ 
		printf("\nExiting program");
		fflush(stdout);
		return 0; // change should_run to 0
	}

	if (strcmp(args[0], "history\n") == 0)
	{
		printf("\nDo History");
		fflush(stdout);
		flags[0] = 1;
	} else {
		flags[0] = 0;
	}

	if (strcmp(args[num_args - 1], "&\n") == 0)
	{
		printf("\nRun in Background");
		fflush(stdout);
		args[num_args-1] = NULL;
		flags[1] = 1;
	} else {
		flags[1] = 0;
	}

	char first_string[strlen(args[0])];
	strcpy(first_string,args[0]);
	char last_char = first_string[strlen(first_string)-2];
	if ( last_char == '!')
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

	return execute();

}

int execute()
{
	pid_t child, wpid;

	child = fork();

	if(child == 0){
	//if is child
		handle_child();
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
	if(flags[1])
	{
		num_background++;
		struct background_element bg_elem = { .pid = child, .number = num_background, .full_command = args};
		background_list[num_background-1] = bg_elem;
		printf("[%d]\t%d\n", bg_elem.number, bg_elem.pid);
		print_done(); 
	} else {
		do{
			// uses the waitpid method to wait for the child process to finish
			wpid = waitpid(child, &status, WUNTRACED);
		} while(!WIFEXITED(status) && !WIFSIGNALED(status));
	}
}

// goes down child path
int handle_child()
{
	int i = 0;
	// Handles issues with executing the child arguments
	for (i = 0; i < num_args; i++)
	{
		args[i];
	}

	if(execvp(args[0], args) == -1){
			printf("\nChild isn't working");			
			return 0;
		}	
	exit(EXIT_FAILURE);
}

// Creates the string for a done process
void handle_done_background()
{
    int status = 0; //status of exit
    pid_t wpid; //pid of exiting process

    wpid = waitpid(-1, &status, WNOHANG);

    if (wpid > 0){
        char str[100];
        int i;       

        for (i = 0; i < num_background; i++)
        {
            if (background_list[i].pid == wpid){
                break;
            }
        }

        sprintf(str, "[%d]\tDone\t%s\n", background_list[i].number, background_list[i].pid); 
        strcpy(done_strs[num_done_strs], str);
        num_done_strs++;

		// Fixes array since element needs to be removed
        remove_bg_elem(wpid);   
    }
}

// reorders list after one background element finishes
void remove_bg_elem(pid_t pid)
{
    int i;
    char temp_array[MAX_LINE];
    char *temp = &temp_array[0];

    for (i = 0; i < num_background; i++)
    {
        if (background_list[i].pid == pid)
        {
            while(i < num_background)
			{
				// shuffle everything over one
				background_list[i] = background_list[i+1];
				i++;
			}
			num_background--;      
        }
    }
    return;
}

// basic print method for dealing with background processes as they finish
void print_done()
{
    int i;
    for(i = 0; i < num_done_strs; i++)
    {
        printf("%s", done_strs[i]);
    } 
    num_done_strs = 0;
}