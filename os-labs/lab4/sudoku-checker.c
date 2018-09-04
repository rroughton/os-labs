/* Rebecca Roughton
Operating Systems
Thursday Lab Section
*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_OF_GROUPS 27
#define NUM_IN_GROUP 9
#define NUM_IN_SUDOKU NUM_IN_GROUP*NUM_IN_GROUP

int is_num(char dig);
void * row_check(void * arg);
static void * col_check(void * arg);
static void * grid_check(void * arg);

// parameters to go to thread
typedef struct{
	int beginning_index; // where to start reading
	int * result_address; // location of where a thread will put results
} parameters;

static int results[NUM_OF_GROUPS]; // results from threads
static int sudoku_array[NUM_IN_SUDOKU]; // sudoku as a 1D array


int main(int argc, char * argv[]){
	
	pthread_t threadIDArray[NUM_OF_GROUPS];
	int threadID = 0;	
	int resultIndex = 0;

	if(argc != 2){
		printf("Argument incorrect");
		return 1;
	}
	FILE * file = fopen(argv[1], "r");
	if(file == 0){
		printf("File reading error");
		return 1;
	}

	int counter = 0;
	char current_num;
	// read file
	while(counter < NUM_IN_SUDOKU)
	{
		current_num = fgetc(file);
		
		if(current_num == '\n')
		{
			// skips newlines
		}
		else{
			// check if all inputs are numbers
			int val = is_num(current_num);
			if(val == -1)
			{
				printf("Invalid input");
				return 1;
			}
			sudoku_array[counter] = val;
			counter++;
		}				
	}

	int index;

	for(index = 0; index < 9;index++)
	{
		parameters *set = (parameters *) malloc(sizeof(parameters));
		
		set->beginning_index = index * NUM_IN_GROUP;
		set->result_address = &results[resultIndex++];
		pthread_create(&threadIDArray[threadID++] , NULL, row_check, set);
	}

	for(index = 0; index < NUM_IN_GROUP;index++)
	{
		parameters *set = (parameters *) malloc(sizeof(parameters));
		
		set->beginning_index = index;
		set->result_address = &results[resultIndex++];
		pthread_create(&threadIDArray[threadID++] , NULL, col_check, set);
	}

	
	// Subgrid thread

	int inner;
	int outer;
	int start = 0;
	
	for(outer = 0; outer < 3;outer++)
	{
		start = outer * 27;
		for(inner = 0; inner < 3; inner++)
		{	
			parameters *set = (parameters *) malloc(sizeof(parameters));	
			set->beginning_index = start + (3*inner);
			set->result_address = &results[resultIndex++];
			pthread_create(&threadIDArray[threadID++] , NULL, grid_check, set);
		}
	}
	
	
	// threads finish
	for(index = 0; index < 27; index++)
	{
		pthread_join(threadIDArray[index], NULL);
	}
	
	
	// check results
	int error = 0;

	// row check
	for(index = 0;index < NUM_IN_GROUP; index++)
	{
		if(results[index] != 0)
		{
			printf("There are two %d's at row %d\n", results[index], index);
			error = 1;
		}
	}

	// col check
	for(index = index; index < NUM_IN_GROUP*2;index++)
	{
		if(results[index] != 0)
		{
			printf("There are two %d's at column %d\n", results[index], index%9);
			error = 1;
		}
	}
	
	// grid check
	for(index = index; index < NUM_IN_GROUP*3;index++)
	{
		if(results[index] != 0)
		{
			printf("There are two %d's at subgrid %d\n", results[index], index%9);
			error = 1;
		}
	}
	
	if(error == 0)
	{
		printf("Sudoku was correct!\n");
	}	

	close(file);
	return 0;
}

// goes through and checks if a char is a digit, then casts it to its int value
int is_num(char dig)
{
	if(dig >= '0' && dig <= '9')
	{
		return dig - '0';
	} else{
		return -1;
	}
}


// thread for checking rows. This format will be repeated for the next few methods as well
void * row_check(void * arg)
{
	parameters * input = (parameters*) (arg);
	int index;
	int repeat = 0; // if there is a repeat, the number will increase
	int check[9] = {0};

	for(index = input->beginning_index; index < input->beginning_index + 9; index++)
	{
		int val = sudoku_array[index];
		if(val != 0)
		{
			if(check[val - 1] == 1){
				repeat = val;
			}
			else{
				check[val - 1] = 1;
			}
		}
	}
	
	*input->result_address = repeat;
	
	free(input);
	pthread_exit(NULL);
}


// checks column, basically the same as before

static void * col_check(void * arg)
{
	parameters * input = (parameters*) (arg);	
	int index;
	int repeat = 0;
	int check[9] = {0};

	for(index = input->beginning_index; index < input->beginning_index + 73; index=index+9)
	{
		int val = sudoku_array[index];

		if(val != 0)
		{
			if(check[val - 1] == 1)
			{
				repeat = val;
			} else {
				check[val - 1] = 1;
			}
		}
	}
	
	*input->result_address = repeat;
	
	free(input);
	pthread_exit(NULL);	
}

// repeat for subgrid
// outer and inner refer to the location in the subgrid
static void * grid_check(void * arg)
{
	parameters * input = (parameters*) (arg);	
	int inner;
	int outer;
	int index = 0;
	int repeat = 0;
	int check[9] = {0};

	for(outer = 0; outer < 3; outer++)
	{
		index = input->beginning_index + (outer * 9);
		
		for(inner = 0; inner < 3; inner++)
		{
			int val = sudoku_array[index+inner];
			if(val != 0)
			{
				if(check[val - 1] == 1)
				{
					repeat = val;
				}
				else
				{
					check[val - 1] = 1;
				}
			}
		}
	}
	
	*input->result_address = repeat;

	free(input);
	pthread_exit(NULL);
}