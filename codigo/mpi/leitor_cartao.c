#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mpi.h"

#include "regex.h"

#define NUM_CARDS 10
#define SIZE_MSG 125

MPI_Datatype cardDatatype;

typedef struct{
	char *msg;
}Card;

void readCards();
void replaceAsterisk();
void printCards();
MPI_Datatype createNewType();

int main(int argc,char* argv[]){

	int numtasks,rank;
	int num_elements_msg = 1;
	MPI_Status stats;

	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD,&numtasks);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	
	cardDatatype = createNewType();

	switch(rank){
	case 0:
		readCards();
	break;
	case 1:
		replaceAsterisk();
	break;
	case 2:
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

void sendMessage(char *msg, int dest, char* from, char* to){

	int tag = 0;
	int num_elements_msg = 1;

	MPI_Send(msg,num_elements_msg,cardDatatype,dest,tag,MPI_COMM_WORLD);
	printf("\nProcesso %s enviou msg para processo %s.\n",from,to);

}

void readCards(){
	
	Card cards[NUM_CARDS];
	int i;
	
	for(i=0;i<NUM_CARDS;i++){
		cards[i].msg = (char*)malloc(sizeof(char)*SIZE_MSG);
	}

	// open file cards.txt
	char path[] = "cards.txt";
	FILE *cards_file = open_file(path,"r");

	int countLine=0;
	
	for(countLine=0;countLine<NUM_CARDS;countLine++){
		fscanf(cards_file,"%s \n",cards[countLine].msg);	
	}
	
	//leitura cartao
	for(countLine=0;countLine<2;countLine++){
//	for(countLine=0;countLine<NUM_CARDS;countLine++){
		printf("Cartao %d: %s\n",countLine,cards[countLine].msg);
	}
	
	fclose(cards_file);
	int dest=1;
	char *from = "0 - Read cards";
	char *to = "1 - Replace asterisk";
	
	sendMessage(cards[0].msg,dest,from,to);	
	sendMessage(cards[1].msg,dest,from,to);	
			
	for(i=0;i<NUM_CARDS;i++){
		free(cards[i].msg);
	}

}

void receiveMessage(char* msg,int source,char* from, char* to){

	int tag = 0;
	int num_elements_msg = 1;
	MPI_Status stats;
	MPI_Recv(msg,num_elements_msg,cardDatatype, source, tag, MPI_COMM_WORLD,&stats);
	printf("Processo %s recebeu msg de %s.\n", from,to);
	
}

void replaceAsterisk(){
		
	Card cards[NUM_CARDS];	
	int i,countLine;
	
	//alocando memoria para variavel	
	for(i=0;i<NUM_CARDS;i++){
		cards[i].msg = (char*)malloc(sizeof(char)*SIZE_MSG);
	}

	// Recebendo msg
	int source = 0;
	char* from = "1 - Replace Asterisk";
	char* to = "2 - printCards";
	receiveMessage(cards[0].msg,source,from,to);
	receiveMessage(cards[1].msg,source,from,to);

	for(countLine=0;countLine<2;countLine++){
//	for(countLine=0;countLine<NUM_CARDS;countLine++){
		regex_match_replace("\\*\\*", cards[countLine].msg,countLine);	
//		printf("Cartao modificado %d: %s \n",countLine,cards[countLine].msg);	
	}

	int dest = 2;
	from = "1 - Replace Asterisk";
	to = "2 - printCards";
	sendMessage(cards[0].msg,dest,from,to);
	sendMessage(cards[1].msg,dest,from,to);
	
}

void printCards(){
		
	Card cards[NUM_CARDS];
	int i, countLine;
	for(i=0;i<NUM_CARDS;i++){
		cards[i].msg = (char*)malloc(sizeof(char)*SIZE_MSG);
	}

	// Recebendo msg
	int source =1; 
	char* from= "2 - Print cards";
	char* to= "1 - Replace Asterisk";
	receiveMessage(cards[0].msg, source,from,to);
	receiveMessage(cards[1].msg, source,from,to);	


//	for(i=0;i<NUM_CARDS;i++){
	for(i=0;i<2;i++){
		printf("Cartao %d: %s\n",i,cards[i].msg);
	}
}

