#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_CARDS 10
#define SIZE_MSG 125

typedef struct{
	char *msg;
}Card;

void readCards();
void replaceAsterisk();
void printCards();

int main(){
	Card cards[NUM_CARDS];
	int i;
	
	for(i=0;i<NUM_CARDS;i++){
		cards[i].msg = (char*)malloc(sizeof(char)*SIZE_MSG);
	}

	readCards(cards);
	
return 0;
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

void readCards(Card* cards){
	/*	
		gerar String de caracters com espaço em branco entre imagens
		colocar string em arquivo temporario 1
	*/
	// open file cards.txt
	char path[] = "cards.txt";
	FILE *cards_file = open_file(path,"r");

	int countLine=0;
	
	for(countLine=0;countLine<NUM_CARDS;countLine++){
		fscanf(cards_file,"%s \n",cards[countLine].msg);	
	}

	for(countLine=0;countLine<NUM_CARDS;countLine++){
		printf("Cartao %d: %s\n",countLine,cards[countLine].msg);
	}
	
	fclose(cards_file);

	char path_temp_1[] = "temp_1.txt";	
	FILE *temp_1 = open_file(path_temp_1,"a+");

	for(countLine=0;countLine<NUM_CARDS;countLine++){
		char *temp_card = cards[countLine].msg;
		printf("Escrevendo em temp_1: %s \n",temp_card);
		fprintf(temp_1,"%s ",temp_card);
	}

	fclose(temp_1);
	int i;
	for(i=0;i<NUM_CARDS;i++){
		free(cards[i].msg);
	}

}

void replaceAsterisk(){
	/*	
		Ler arquivo temporario 1
		trocar asteriscos por cerquilha
		colocar resultado em um arquivo temporario 2
	*/
	
	char *path_temp_1 = "temp_1.txt";
	FILE* temp_1 = open_file(path_temp_1,"r");
	
	Card cards[NUM_CARDS];	
	int i;
	
	for(i=0;i<NUM_CARDS;i++){
		cards[i].msg = (char*)malloc(sizeof(char)*SIZE_MSG);
	}

	
}

void printCards(){
	/*	
		Ler arquivo temporario 2
		imprimir em tela
	*/
}

