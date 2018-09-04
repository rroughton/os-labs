/* Rebecca Roughton
Operating Systems
Thursday Lab Section
*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NO_SUBGROUP 27  //No of subgroups (row, col, grid)
#define DIG_SUBGROUP 9 //Digits in a subgroup
#define DIG_SUDOKU DIG_SUBGROUP*DIG_SUBGROUP //Total no of values in sudoku

int chkrtnDigit(char dig);//Checks if char is a digit then returns as int

//This struct outline what parameters get sent to each thread
typedef struct{
	int startInd;//The starting location of where to begin reading values
	int * resultAddr;//Address of where a thread will put it's resluts
} parameters;

static int resultArray[NO_SUBGROUP];//contains results from each thread
static int sudokuArray[DIG_SUDOKU];//Array containing every number in the sudoku

//Thread method to check a row
void * row_check(void * arg){
	parameters * data = (parameters*) (arg);//Casts argument as parameters
	int index;
	int repeat = 0;//return value
	int check[9] = {0};//determines whether or not we've seen this value

	for(index = data->startInd; index < data->startInd + 9; index++){
		int val = sudokuArray[index];
		if(val == 0){
			//Do nothing
		}
		else{
			if(check[val - 1] == 1){
				repeat = val;//This number is repeated
			}
			else{//Seeing this number for the first time
				check[val - 1] = 1;//We've seen this number
			}
		}
	}
	
	//Adjust result array according to repeat
	*data->resultAddr = repeat;
	
	//0 is no dupe
	free(data);
	pthread_exit(NULL);
}

//Thread method to check a column
static void * col_check(void * arg){
	parameters * data = (parameters*) (arg);	
	int index;
	int repeat = 0;
	int check[9] = {0};

	for(index = data->startInd; index < data->startInd + 73; index=index+9){
		int val = sudokuArray[index];
		//printf("%d: INDEX:%d VALUE:%d\n", data->startInd,index, val);
		if(val == 0){
			//Do nothing
		}
		else{
			if(check[val - 1] == 1){
				repeat = val;//This number is repeated
			}
			else{//Seeing this number for the first time
				check[val - 1] = 1;//We've seen this number
			}
		}
	}
	
	//Adjust result array according to repeat
	*data->resultAddr = repeat;
	
	//0 is no dupe
	free(data);
	pthread_exit(NULL);	
}

//Thread method to check a subgrid
static void * grid_check(void * arg){
	parameters * data = (parameters*) (arg);	
	int inner;
	int outer;
	int index = 0;
	int repeat = 0;
	int check[9] = {0};

	for(outer = 0; outer < 3; outer++){
		index = data->startInd + (outer * 9);
		for(inner = 0; inner < 3; inner++){
			int val = sudokuArray[index+inner];
			if(val == 0){
				//Do nothing
			}
			else{
				if(check[val - 1] == 1){
					repeat = val;//This number is repeated
				}
				else{//Seeing this number for the first time
					check[val - 1] = 1;
				}
			}
		}
	}
	
	//Adjust result array according to repeat
	*data->resultAddr = repeat;
	
	//0 is no dupe
	free(data);
	pthread_exit(NULL);
}


int main(int argc, char * argv[]){
	
	pthread_t threadIDArray[NO_SUBGROUP];//Stores all thread identifiers
	int threadID = 0;	
	int resultIndex = 0;//Will track what index a thread will return their answer to

	if(argc != 2){//Check correct user arguments
		printf("Invalid arguments\n%s filename", argv[0]);
		return 1;
	}
	FILE * file = fopen(argv[1], "r");//Open the file
	if(file == 0){//check for success
		printf("Could not open file\n");
		return 1;
	}
	
	//Start Reading the File

	int counter = 0;
	char cDig;
	while(counter < DIG_SUDOKU){
		cDig = fgetc(file);//Read the next character
		
		if(cDig == '\n'){
			//We ignore this character	
		}
		else{
			int val = chkrtnDigit(cDig);
			if(val == -1){//Not a valid character in input
				printf("Invalid input");
				return 1;
			}
			sudokuArray[counter] = val;
			counter++;
		}				
	}

	int index;//Used for the bunches of loops

	//Start Row Threads
	for(index = 0; index < 9;index++){
		//Set memory for thread
		parameters *set = (parameters *) malloc(sizeof(parameters));
		
		set->startInd = index * DIG_SUBGROUP;//Next starting index
		set->resultAddr = &resultArray[resultIndex++];//Move to next result
		pthread_create(&threadIDArray[threadID++] , NULL, row_check, set);//Begin thread
	}

	//Start Column Threads
	for(index = 0; index < DIG_SUBGROUP;index++){
		parameters *set = (parameters *) malloc(sizeof(parameters));
		
		set->startInd = index;//Next starting index
		set->resultAddr = &resultArray[resultIndex++];//Move to next result
		pthread_create(&threadIDArray[threadID++] , NULL, col_check, set);//Begin thread
	}

	//Start SubGrid Threads
	int inner;
	int outer;
	int start = 0;
	
	for(outer = 0; outer < 3;outer++){
		start = outer * 27;
		for(inner = 0; inner < 3; inner++){	
			parameters *set = (parameters *) malloc(sizeof(parameters));	
			set->startInd = start + (3*inner);//Next starting index
			set->resultAddr = &resultArray[resultIndex++];//Move to next result
			pthread_create(&threadIDArray[threadID++] , NULL, grid_check, set);
		}
	}
		
	//Wait for all threads to finish
	for(index = 0; index < 27; index++){
		pthread_join(threadIDArray[index], NULL);
	}
	
	//Review result Array
	int error = 0;//1 if error present

	//Row
	for(index = 0;index < DIG_SUBGROUP; index++){
		if(resultArray[index] != 0){
			printf("There are two %d's at row %d\n", resultArray[index], index);
			error = 1;
		}
	}
	//Col
	for(index = index; index < DIG_SUBGROUP*2;index++){
		if(resultArray[index] != 0){
			printf("There are two %d's at column %d\n", resultArray[index], index%9);
			error = 1;
		}
	}
	//Grid
	for(index = index; index < DIG_SUBGROUP*3;index++){
		if(resultArray[index] != 0){
			printf("There are two %d's at subgrid %d\n", resultArray[index], index%9);
			error = 1;
		}
	}
	
	if(error == 0){
		printf("There were no errors with this sudoku\n");
	}	
	close(file);
	return 0;
}
//checks if a char is a digit and returns the respective int
int chkrtnDigit(char dig){

	if(dig >= '0' && dig <= '9'){
		return dig - '0';//Returns numerical value of digit
	}
	else{
		return -1;//Failed
	}

}
