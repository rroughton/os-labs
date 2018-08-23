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
#include <fcntl.h>
#include <sys/stat.h>

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

int num_background = 0;
int num_history = 0;
char *args[MAX_ARGS];
int num_args = 0;
int flags[10]; // 0 = History, 1 = Background, 2 = echo, 3 = cd, 4 = piping, 5 = input, 6 = output
char *done_strs[MAX_ARGS] = {};
int num_done_strs = 0;
int pipe_locations[MAX_ARGS];
int num_pipes = 0;
int redirect_location;
char file_string[MAX_LINE];

int set_flags(void);
int run_args(void);
int execute(void);
void signal_done_background();
void print_done(void);
void remove_bg_elem(pid_t pid);
int run_parent(pid_t child, pid_t wpid);
int run_child(void);
void clear_args(void);
void execute_cd(void);
void clear_all(void);
void redirect(void);
void execute_piping(void); 


//

int main(void)
{
    
    char input[MAX_LINE];
    int should_run = 1;

	signal(SIGCHLD, signal_done_background);

	while (should_run)
	{
    	int i = 0;
		printf("rrsh> ");
        fflush(stdout);

		clear_args();
		clear_all();
			
		//Get input
		fgets(input, MAX_LINE, stdin);
		input[strcspn(input, "\n")] = '\0';
		char *tok = strtok(input, " ");
		while (tok != NULL)
		{
			args[i] = tok;
			i++;
			tok = strtok(NULL, " ");
		}
		args[i] = NULL;

		num_args = i;

		should_run = set_flags();
		if (should_run)
		{
			run_args();
		}
	}
	return 0;
}

// sets flags for special characters in input
int set_flags()
{
	int i = 0;

	if (strcmp(args[0], "exit") == 0)
	{ 
		printf("\nExiting program");
		fflush(stdout);
		return 0; // change should_run to 0
	}

	if (strcmp(args[0], "history") == 0)
	{
		flags[0] = 1;
	} else {
		flags[0] = 0;
	}

	if (strcmp(args[num_args - 1], "&") == 0)
	{
		// trim off & and lower amount of args
		args[num_args-1] = NULL;
		num_args--;
		flags[1] = 1;
	} else {
		flags[1] = 0;
	}

	char first_string[strlen(args[0])];
	strcpy(first_string,args[0]);
	char last_char = first_string[strlen(first_string)-2];
	if ( last_char == '!')
	{
		flags[2] = 1;
	} else {
		flags[2] = 0;
	}

	if (strcmp(args[0], "cd") == 0)
	{
		flags[3] = 1;
	} else {
		flags[3] = 0;
	}

	// loop for checking for pipes, and redirection
	for (i = 0; i < num_args; i++)
	{
		// checks if token is a pipe.
		if (strcmp(args[i], "|") == 0)
		{
			flags[4] = 1;
			pipe_locations[num_pipes];
			num_pipes++;

		} else {
			// don't reset flag just because the one token wasn't a pipe
			if (flags[4] != 1)
			{
				flags[4] = 0;
			}
		}

		// input

			
		if (strcmp(args[i], "<") == 0)
		{
			if (flags[5] == 1 || flags[6] == 1) 
			{
				printf("\nToo many redirects");
				fflush(stdout);
				return 0;
			} 	

			flags[5] = 1;
			redirect_location = i;
		} else {
			flags[5] = 0;
		}

		// output


		if (strcmp(args[i], ">") == 0)
		{
			if (flags[5] == 1 || flags[6] == 1) {
				printf("\nToo many redirects");
				fflush(stdout);
				return 0;
			}
			flags[6] = 1;
			redirect_location = i;
		} else {
			flags[5] = 0;
		}
	}
	return 1;
}

int run_args(void)
{
	if (flags[3]) // do cd
	{
		execute_cd();
		return 1;

	// do piping
	} else if (flags[4]){

		execute_piping();
		return 1;

	} else if (flags[5] || flags[6]) {

		redirect();
		return 1;
	} else {
		return execute();
		return 1;
	}
}

// fork argument and go through child and parent code
int execute()
{
	pid_t child, wpid;
	int i;
	int status; // Used to keep track of status for waitpid()
	
	child = fork();

	if(child == 0){
	//if is child
		i = 0;

		if(execvp(args[0], args) == -1){
			printf("\nChild isn't working");
			fflush(stdout);			
			return 0;
		}	
		exit(EXIT_FAILURE);
			///
	}else {
	//if is parent
	
		if(flags[1])
		{
			num_background++;

			struct background_element bg_elem = { .pid = child, .number = num_background};

			for (i = 0; i < num_args; i++) 
			{
				int str_len = strlen(args[i]);
				bg_elem.full_command[i] = calloc(str_len + 1, sizeof(char));
				strcpy(bg_elem.full_command[i], args[i]);
			}

			bg_elem.full_command[i++] = NULL;

			background_list[num_background-1] = bg_elem;
			for(i = 0; i < num_done_strs; i++)
			{
				printf("%s", done_strs[i]);
				fflush(stdout);
			} 
			num_done_strs = 0;

		} else {
			for(i = 0; i < num_done_strs; i++)
			{
				printf("%s", done_strs[i]);
				fflush(stdout);
			} 
			num_done_strs = 0;
			do{
				// wait for child to finish
				wpid = waitpid(child, &status, WUNTRACED);
			} while(!WIFEXITED(status) && !WIFSIGNALED(status));
		}
	}
	return 1;
}

// Prints the string for a done process
void signal_done_background()
{
	int status = 0; //status of exit
    pid_t wpid; //pid of exiting process
    wpid = waitpid(-1, &status, WNOHANG);

    if (wpid > 0){
        char done_str[150] = { };
        int i;       

       	while (background_list[i].pid != wpid)
		{
			i++;
		}

        sprintf(done_str, "[%d]\tDone\t%d\n", background_list[i].number, background_list[i].pid); 
		done_strs[num_done_strs] = done_str;

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
	fflush(stdout);
}

// runs cd method. Primarily utilizes chdir() method.
void execute_cd()
{
	if (args[1] == NULL)
	{
		chdir(getenv("HOME"));
	} else {
		if (chdir(args[1]) == -1)
		{
			printf("\ncd failed\n");
			fflush(stdout);
		}
	}
}

void clear_args()
{
	int i = 0;
	for(i = 0; i < MAX_ARGS; i++)
	{
		args[i] = NULL;
	}
	num_args = 0;
}

void clear_all()
{
	int i = 0;
	for (i = 0; i < 10; i++)
	{
		//history_list[i] == NULL;
		//background_element[i] == NULL;
		//flags[i] = 0;
	}

	for (i = 0; i < MAX_ARGS; i++)
	{
		args[i] = NULL;
		done_strs[i] = NULL;
		pipe_locations[i] = 0;
	}

	num_background = 0;
	num_history = 0;
	num_args = 0;
	num_done_strs = 0;
	num_pipes = 0;
	redirect_location = 0;
}

void execute_piping()
{

	// base case, you're too the left of the left most pipe

	// recursive case, there's another pipe to the left


}



// does both output and input redirection
void redirect()
{
	printf("\nreached here!");
	fflush(stdout);	

	pid_t child, wpid;
	int status;
	int i = 0;
	for (i = redirect_location; i < num_args; i++)
	{
		args[i] = NULL;
	}
	
	child = fork();
	if(child == -1){
		printf("Fork Error");
	}

	// go down child path
	else if(child == 0){
		if(flags[5]){//Input
			int file_desc_in = open(file_string, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
			dup2(file_desc_in, STDIN_FILENO);
			close(file_desc_in);
			if(execvp(args[0], args) == -1)
			{
				printf("\nChild execution failed");	
			}
		}

		// do output
		else if(flags[6])
		{
			int file_desc_out = open(file_string, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
			dup2(file_desc_out, 1);
			dup2(file_desc_out, 2);
			close(file_desc_out);
			if(execvp(args[0], args) == -1)
			{
				printf("\nChild execution failed");	
			}
		}
		
		exit(EXIT_FAILURE);
	}
	else{
		// parent will always wait for child to finish
		do
		{
			wpid = waitpid(child, &status, WUNTRACED);
		} while(!WIFEXITED(status) && !WIFSIGNALED(status));	
	}		
	
}