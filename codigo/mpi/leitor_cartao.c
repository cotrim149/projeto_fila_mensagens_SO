#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "mpi.h"

#include "regex.h"

#define MAX_CARDS 10
#define SIZE_MSG 125
#define READER 0
#define REPLACER 1
#define PRINTER 2

#define NUM_CARDS 4

MPI_Datatype cardDatatype;

typedef struct{
	char *msg;
}Card;

void readCards();
void replaceAsterisk();
void printCards();
MPI_Datatype createNewType();

int main(int argc,char* argv[]){

	srand(time(NULL));

	int numtasks,rank;
	MPI_Status stats;

	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD,&numtasks);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	
	cardDatatype = createNewType();
	
		
	switch(rank){
	case READER:
		readCards();
	break;
	case REPLACER:
		replaceAsterisk();
	break;
	case PRINTER:
		printCards();
	break;
	}	
	
	MPI_Finalize();

return 0;
}

MPI_Datatype createNewType(){
	int count = 1;
	int lengths[1] = {SIZE_MSG};
	
	MPI_Aint offsets[1] = {0};
	MPI_Datatype types[1] = {MPI_CHAR};

	MPI_Datatype cardDatatype;
	MPI_Type_create_struct(count, lengths, offsets, types, &cardDatatype);
	MPI_Type_commit(&cardDatatype);
	
	return cardDatatype;
}

FILE* open_file(char* path_file, char* mode){

	FILE *file = fopen(path_file,mode);
	
	if(file == NULL){
		printf("%s nao pode ser aberto!\n",path_file);
	}else{
		printf("%s aberto com sucesso!\n",path_file);
	}
	
	return file;
}

void getRandomMessages(Card *cards, Card *cards_from_file){

	int numbers[NUM_CARDS];	
	int i;

	for (i = 0; i < NUM_CARDS; i++) {     // fill array
		numbers[i] = i;
	}

	for (i = 0; i < NUM_CARDS; i++) {    // shuffle array
		int temp = numbers[i];
		int randomIndex = rand() % NUM_CARDS;

		numbers[i] = numbers[randomIndex];
		numbers[randomIndex] = temp;
	}

	for (i = 0; i < NUM_CARDS; i++) {     // print array
		printf("%d ", numbers[i]);
	}
	printf("\n");
	for(i=0;i<NUM_CARDS;i++){
		cards[i] = cards_from_file[numbers[i]]; 
	}		

}

void receiveMessage(char* msg,int source,char* from, char* to){

	int tag = 0;
	int num_elements_msg = 1;
	MPI_Status stats;
	MPI_Recv(msg,num_elements_msg,cardDatatype, source, tag, MPI_COMM_WORLD,&stats);
	printf("Processo %s recebeu msg de %s.\n", from,to);
	
}

void sendMessage(char *msg, int dest, char* from, char* to){

	int tag = 0;
	int num_elements_msg = 1;

	MPI_Send(msg,num_elements_msg,cardDatatype,dest,tag,MPI_COMM_WORLD);
	printf("\nProcesso %s enviou msg para processo %s.\n",from,to);

}

void readCards(){
	
	Card cards_from_file[MAX_CARDS];
	int i;
	
	for(i=0;i<MAX_CARDS;i++){
		cards_from_file[i].msg = (char*)malloc(sizeof(char)*SIZE_MSG);
	}

	// open file cards.txt
	char path[] = "cards.txt";
	FILE *cards_file = open_file(path,"r");

	int countLine=0;
	
	for(countLine=0;countLine<MAX_CARDS;countLine++){
		fscanf(cards_file,"%s \n",cards_from_file[countLine].msg);	
	}

	fclose(cards_file);
	
	// pega cartoes aleatorios
	Card cards[NUM_CARDS];
	getRandomMessages(cards,cards_from_file);
		
	//leitura cartao
	for(countLine=0;countLine<NUM_CARDS;countLine++){
		printf("Cartao %d: %s\n",countLine,cards[countLine].msg);
	}
	
	int dest=1;
	char *from = "0 - Read cards";
	char *to = "1 - Replace asterisk";
	
	for(countLine=0;countLine<NUM_CARDS;countLine++){
		sendMessage(cards[countLine].msg,dest,from,to);	
	}
	
}

void replaceAsterisk(){
		
	Card cards[MAX_CARDS];	
	int i,countLine;
	
	//alocando memoria para variavel	
	for(i=0;i<MAX_CARDS;i++){
		cards[i].msg = (char*)malloc(sizeof(char)*SIZE_MSG);
	}

	// Recebendo msg
	int source = 0;
	char* from = "1 - Replace Asterisk";
	char* to = "0 - Read Cards";
	for(i=0;i<NUM_CARDS;i++){
		receiveMessage(cards[i].msg,source,from,to);	
	}

	for(countLine=0;countLine<NUM_CARDS;countLine++){
//	for(countLine=0;countLine<MAX_CARDS;countLine++){
		regex_match_replace("\\*\\*", cards[countLine].msg,countLine);	
//		printf("Cartao modificado %d: %s \n",countLine,cards[countLine].msg);	
	}

	int dest = 2;
	from = "1 - Replace Asterisk";
	to = "2 - Print Cards";

	for(i=0;i<NUM_CARDS;i++){
		sendMessage(cards[i].msg,dest,from,to);
	}
	
}

void printCards(){
		
	Card cards[MAX_CARDS];
	int i, countLine;
	for(i=0;i<MAX_CARDS;i++){
		cards[i].msg = (char*)malloc(sizeof(char)*SIZE_MSG);
	}

	// Recebendo msg
	int source =1; 
	char* from= "2 - Print cards";
	char* to= "1 - Replace Asterisk";

	for(i=0;i<NUM_CARDS;i++){
		receiveMessage(cards[i].msg, source,from,to);	
	}

	for(i=0;i<NUM_CARDS;i++){
		printf("Cartao %d: %s\n",i,cards[i].msg);
	}
}

