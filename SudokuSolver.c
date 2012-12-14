/*
 ============================================================================
 Name        : SudokuSolver.c
 Author      : Marek Kunz
 Version     : 1.0.0
 Copyright   : Manas Coding 
 Description : Sudoku solver in C.
 ============================================================================
 */

/**
Usage:
Filename of the file with the sudoku problem.

Example Input:


000000070
006800004
800090061
005006040
000524000
010700500
520060009
400003800
030000000


Compilation Info:

Just compile it using a C compiler like gcc. Nothing special.
ex. gcc -o SudokuSolver SudokuSolver.c

Run: 

In unix environment (ex. Linux) type ./SudokuSolver filename

Algorithm: 

We first take in the input and create a board. A board consists of
[9][9] array of ints representing the sudoku values. A value of 
zero means blank. After that we solve the sudoku by for each value 
getting the possible moves for that area. If number of moves is one
we take that move and update our board. If no single moves board is
found we must guess. To take a guess we duplicate a board and take a 
guess. This guess is taken from an area where there are the least 
number possible moves to take. We duplicate the board and put the
old board on a stack. We again try to find possible moves for this new
board with a guess. We reapeat this process until solved or until we
find no possible moves for an empty spot. In this case we pop a board
from the stack and take a different guess. If no possible moves are
given for an area we either pop from stack or it's an invalid board if
stack is null. Even the hardest boards will take around a second to 
solve depending the computer.  

*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*The moves we already tried so we don't repeat them.*/
typedef struct {
	int x;
	int y;
	int value;
	struct tried * next;

}tried;

/*The available moves for a spot will be stored here.*/
typedef struct {
	int num;
	int * array;
} moves;

/*
The board holds a 9*9 array of positions and the available 
moves for each position
*/
typedef struct{
	int arr[9][9];
	moves * moves[9][9];
	tried * tried;
} board;

/*
The stack holds a board an a pointer to a next node in the stack.
*/
typedef struct{
	board * value;
	struct stack * next;

}stack;


//The current board
board * current;
//The top of the stack used to hold the boards
stack * head;

/*
Print the available moves given a moves struct
*/
void printMove(moves ** move){
	moves * temp = *move;
	int loop;
	fprintf(stdout, "Moves Available: ");
	
	for(loop = 0; loop < temp->num; loop ++){
		fprintf(stdout, " %d,", temp->array[loop]);
	}
	
	printf("\n");
}

/*
Get the moves available given an x and y position of the board.
*/
moves * getmoves (int x, int y){

	int loop;
	int contains[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
	moves * temp = malloc(sizeof(moves));
	
	//Check which values are already used
	for(loop = 0; loop <9; loop++){
		int xval,yval;
		xval = current->arr[x][loop];
		
		if(xval != 0 && loop != y){
			contains[xval - 1] = 0;
		}
		
		yval = current->arr[loop][y];
		if(yval != 0 && loop != x){
			contains[yval - 1] = 0;
		}
	}

	int inn;
	//Check the 3*3 grid
	
	for(loop = 0; loop <3; loop ++){
		
		for(inn = 0; inn < 3; inn++){
			
			int zval;
			zval = current->arr[x/3 * 3 + loop][y/3 * 3 + inn];
			
			if(zval != 0 && x/3 + loop != x && y/3 + inn != y){
				contains[zval - 1] = 0;
			}
		}

	}
	loop = 0;
	
	for(inn = 0 ; inn <9; inn++){
		
		if(contains[inn] != 0) {
			loop ++;
		}
	}
	int index = 0;
	temp->num = loop;
	temp->array = malloc(sizeof(int) * loop);
	
	for(inn = 0 ; inn <9; inn++){
		
		if(contains[inn] != 0) {
			temp->array[index] = contains[inn];
			index++;
		}
	}

	return temp;
}

/*
Create a new board and copy the values over from current board. 
*/
board * boardcopy(){
	board * newb = (board *) malloc(sizeof(board));
	//copy the values
	int x, y;
	
	for(x = 0; x < 9; x++){
		
		for(y = 0; y < 9; y++){
			newb->arr[x][y] = current->arr[x][y];
		}
	}
	newb->tried = NULL;

	return newb;
}

/*
Free a board.
*/
void freeboard(board ** boardfree){
	board * temp = *boardfree;
	
	if(temp->moves != NULL){
		int x, y;
		
		for(x = 0; x < 9; x++){
			
			for(y = 0; y < 9; y++){
				moves * mtemp = temp->moves[x][y];
				free(mtemp);
			}
		}
		
		
	}

	if(temp->tried != NULL){
		tried * ttemp = temp->tried;
		tried * t2;

		while(ttemp != NULL){
			t2 = ttemp->next;
			free(ttemp);
			ttemp = t2;
		}
	}

	free(temp);
}

/*
Solves the board. Loop until either finished or board is invalid.
*/
void solveboard(){

	int solved = 0;
	int taken = 0;
	int x, y;
	int smallx, smally, small;
	int loop = 0;

	while(!solved){
		Done:
		small = 10;//The shortest move set to zero so if there is a move it's shorter
		solved = 1;
		taken = 0;
		
		//Find possible moves for every zero
		for(x = 0; x < 9; x++){

			for(y = 0; y <9; y++){

				if(current->arr[x][y] ==0){
					current->moves[x][y] = getmoves(x, y);

					if(current->moves[x][y]->num == 1){
						current->arr[x][y] = current->moves[x][y]->array[0];
						taken = 1;

					}
					else if(current->moves[x][y]->num == 0){
						//Invalid solution pop of stack and repeat
						if(head == NULL){
							fprintf(stdout, "Invalid Board!\n");
							exit(0);
						}
						else{
							//printf("Popping Stack\n");
							freeboard(&current);
							current = head->value;
							stack * temps = head;
							head = head->next;
							free(temps);
							goto Done;

						}
					}
					else{
						solved = 0;
						if(current->moves[x][y]->num < small){
							small = current->moves[x][y]->num;
							smallx = x;
							smally = y;
						}

					}
				}
			}
		}
		
		if(!solved && !taken){
			//We must venture a guess
			int guess, rand;
			rand = 0;
			
			do{
				if(rand == small){

					if(head == NULL){
						fprintf(stdout, "Invalid Board!\n");
						exit(0);
					}
					else{
						//printf("Popping Stack\n");
						freeboard(&current);
						current = head->value;
						stack * temps = head;
						head = head->next;
						free(temps);

					}
					goto Done;
				}
				guess = current->moves[smallx][smally]->array[rand];

				rand++;

			}while(contains(smallx, smally, guess));
			
			//Add this guess to the current list of guesses
			tried * temp = malloc(sizeof(tried));
			temp->value = guess;
			temp->x = smallx;
			temp->y = smally;
			printf("Guessing: (%d,%d) Value %d: Available Options %d\n", temp->x + 1, temp->y + 1, temp->value, small);
			
			if(current->tried == NULL){
				current->tried = temp;
				temp->next = NULL;
			}
			else{
				temp->next = current->tried;
				current->tried = temp;
			}
			
			//create new board with this guess and add it to the a new node in the stack
			board * newb = boardcopy();
			stack * nstack = malloc(sizeof(stack));
			nstack->value = current;
			current = newb;
			current->arr[smallx][smally] = guess;


			if(head == NULL){
				head = nstack;
				head->next = NULL;
			}
			else{
				nstack->next = head;
				head = nstack;
			}



		}
		/*
		 * Debbuging Loop
		loop++;
		if(loop == 100){
			//break;
		}*/


	}
}

/*
Print the board.
*/
void printboard( board ** curr){
	board * temp = *curr;
	int x, y;
	
	for(x =0; x< 18; x++){
		fprintf(stdout, "__");
	}
	
	fprintf(stdout, "\n");
	
	for(x = 0; x < 9; x++){
		
		for(y = 0; y < 9; y++){
			fprintf(stdout, "| %d ", ((*temp).arr)[x][y]);
		}
		fprintf(stdout, "|\n");

	}
	
	for(x =0; x< 18; x++){
		fprintf(stdout, "__");
	}
	
	fprintf(stdout, "\n");

}

/*
Check if the tried moves contain a certain guess so we do not repeat guessing
the same values.
*/
int contains(x, y, guess){
	
	if(current->tried == NULL){

		return 0;
	}
	
	tried * temp = current->tried;
	
	do{
		if(temp->value == guess && temp->x == x && temp->y == y){

			return 1;
		}

	}while((temp = temp->next)!= NULL);

	return 0;

}

/*
Main function
*/
int main(int argc, char ** argv) {

	if(argc != 2){
		fprintf(stdout, "Usage: filename\n");	
		exit(0);	
	}	
	
	time_t start,end;
	time (&start);
	double dif;
	head = NULL;
	FILE * fp;
	fp = fopen(argv[1],"r");
	
	if(fp == NULL){
		fprintf(stdout, "Invalid File!\n");	
		exit(0);
	}

	int total = 0;
	char char_read;
	current = (board *) malloc(sizeof(board));
	current->tried = NULL;
	// Read the input from the file.
	
	while(total != 81){

		char_read = fgetc(fp);
		if(char_read >= '0' && char_read <= '9'){			
			current->arr[total/9][total%9] = char_read - '0';
			total ++;
		}
	}

	/*Print initial board*/
	printboard(&current);
	/*Solve the board*/
	solveboard();
	puts("\n");
	/*Print finished board*/
	printboard(&current);
	fclose(fp);
	freeboard(&current);

	time (&end);
	dif = difftime (end,start);
	fprintf (stdout, "It took %.1lf seconds to solve.\n", dif );
	return EXIT_SUCCESS;
}
