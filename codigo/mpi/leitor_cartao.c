#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

#include "mpi.h"

#define NUM_CARDS 10
#define SIZE_MSG 125

typedef struct{
	char *msg;
}Card;

void readCards();
void replaceAsterisk();
void printCards();
MPI_Datatype createNewType();

int main(int argc,char* argv[]){
	Card cards[NUM_CARDS];
	int i;
	
	for(i=0;i<NUM_CARDS;i++){
		cards[i].msg = (char*)malloc(sizeof(char)*SIZE_MSG);
	}

	int numtasks,rank;
	int num_elements_msg = 1;
	MPI_Status stats;

	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD,&numtasks);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	
	MPI_Datatype cardDatatype = createNewType();
	
	switch(rank){
	case 0:
		printf("\nPROCESSO RANK 0 EM ACAO\n");
		readCards(cards,cardDatatype);
	break;
	case 1:
		printf("\nPROCESSO RANK 1 EM ACAO\n");
		replaceAsterisk(cardDatatype);
	break;
	case 2:
		printf("\nPROCESSO rank 2 EM ACAO\n");
		printCards(cardDatatype);
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

void readCards(Card* cards,MPI_Datatype cardDatatype){
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
	
	int dest = 1;
	int tag = 0;
	int num_elements_msg = 1;

	MPI_Send(cards[0].msg,num_elements_msg,cardDatatype,dest,tag,MPI_COMM_WORLD);
	MPI_Send(cards[1].msg,num_elements_msg,cardDatatype,dest,tag,MPI_COMM_WORLD);
	printf("Processo 0(Leitura de msg) enviou %s para %d(replaceAsterisk).\n", cards[0].msg, dest);

	
	// arquivo temporario 1
/*	char path_temp_1[] = "temp_1.txt";	*/
/*	FILE *temp_1 = open_file(path_temp_1,"a+");*/

/*	for(countLine=0;countLine<NUM_CARDS;countLine++){*/
/*		char *temp_card = cards[countLine].msg;*/
/*		printf("Escrevendo em temp_1: %s \n",temp_card);*/
/*		fprintf(temp_1,"%s ",temp_card);*/
/*	}*/

/*	fclose(temp_1);*/
	int i;
	for(i=0;i<NUM_CARDS;i++){
		free(cards[i].msg);
	}

}

/* mostra uma mensagem do erro usando regerror */
int er_error(int i, regex_t reg)
{
	size_t length;
	char *buffer=NULL;

	/* pega o tamanho da mensagen de erro */
	length = regerror (i, &reg, NULL, 0);

	/* aloca espaço para a mensagem de erro */
	if ((buffer = (char *)malloc (length)) == NULL) {
		fprintf(stderr, "error: malloc buffer\n");
		exit(1);
	}
	
	/* coloca em buffer a mensagem de erro */
	regerror (i, &reg, buffer, length);
	
	fprintf(stderr,"Erro: %s\n",buffer);
	free(buffer);
	exit(1);

}

void regex_match_replace(char *regex, char* info, int card_number)
{
	int count_match, start;
	int error;
	/* aloca espaço para a estrutura do tipo regmatch_t */
	regmatch_t match;
	/* aloca espaço para a estrutura do tipo regex_t */
	regex_t reg;

	/* compila a ER passada em argv[1]
	 * em caso de erro, a função retorna diferente de zero */
	if ((count_match=regcomp(&reg , regex, REG_EXTENDED)) != 0)
		/* imprime uma string do erro */
		er_error(count_match,reg);	

//	printf("\n********** CARTAO %d **********\n%s\n\n",card_number,info);
	count_match = start = 0;
	/* casa a ER com o input argv[2] 
	 * ^ marca início de linha */
	error = regexec(&reg, info, 1, &match, 0);
	/* tenta casar a ER mais vezes na string */
	while(error == 0) {
//		printf("início da string de pesquisa atual no caractere %d\n",start);
//		printf("string de pesquisa = \"%s\"\n",info+start);
//		printf("casou do caractere = %d ao %d\n\n",match.rm_so,match.rm_eo);
		
		int countChar=0;
		for(countChar=0; countChar < (match.rm_eo - match.rm_so); countChar++){
			if(countChar==0){

				info[start+match.rm_so] = '#';

/*				if(count_match == 0){*/
/*					info[match.rm_so] = '#';				*/
/*				}else{*/
/*				}*/
			}
			else{
				int j=0;
				for(j=(start+match.rm_so)+1;j < SIZE_MSG-1; j++){
					info[j] = info[j+1];
				}
			}
		}
		
		start +=match.rm_eo; /* atualize início de string */
		count_match++;
		/* casa a ER com o input argv[2].  ^ não casa mais início de linha */
		error = regexec(&reg, info+start, 1, &match, REG_NOTBOL);
	}
	
//	if (start !=0) printf("Número total de casamentos = %d\n",count_match);

	int num_times = count_match/2;	
	if(count_match > 0)
		info[(SIZE_MSG-1)-num_times] = '\0';

}

void replaceAsterisk(MPI_Datatype cardDatatype){
	
/*	char *path_temp_1 = "temp_1.txt";*/
/*	FILE* temp_1 = open_file(path_temp_1,"r");*/
	
	Card cards[NUM_CARDS];	
	int i,countLine;
	
	//alocando memoria para variavel	
	for(i=0;i<NUM_CARDS;i++){
		cards[i].msg = (char*)malloc(sizeof(char)*SIZE_MSG);
	}
	// passando info de arquivo para variavel
/*	for(countLine=0;countLine<NUM_CARDS;countLine++){*/
/*		fscanf(temp_1,"%s ",cards[countLine].msg);	*/
/*	}*/
	
/*	fclose(temp_1);*/

	// Recebendo msg
	int source =0; 
	int tag = 0;
	int num_elements_msg = 1;
	MPI_Status stats;
	MPI_Recv(cards[0].msg,num_elements_msg,cardDatatype, source, tag, MPI_COMM_WORLD,&stats);
	MPI_Recv(cards[1].msg,num_elements_msg,cardDatatype, source, tag, MPI_COMM_WORLD,&stats);
	printf("Processo 1(replaceAsterisk) recebeu %s de %d.\n", cards[0].msg, source);

	for(countLine=0;countLine<2;countLine++){
//	for(countLine=0;countLine<NUM_CARDS;countLine++){
		regex_match_replace("\\*\\*", cards[countLine].msg,countLine);	
		printf("Cartao modificado %d: %s \n",countLine,cards[countLine].msg);	
	}

	int dest = 2;

	MPI_Send(cards[0].msg,num_elements_msg,cardDatatype,dest,tag,MPI_COMM_WORLD);
	MPI_Send(cards[1].msg,num_elements_msg,cardDatatype,dest,tag,MPI_COMM_WORLD);
	printf("Processo 1(replaceAsterisk) enviou %s para %d(printCards).\n", cards[0].msg, dest);

	


	// abertura de arquivo temporario 2
/*	char* path_temp_2 = "temp_2.txt";*/
/*	FILE* temp_2 = open_file(path_temp_2,"w+");*/
/*	*/
/*	for(countLine=0;countLine<NUM_CARDS;countLine++){*/
/*		char *temp_card = cards[countLine].msg;*/
/*		fprintf(temp_2,"%s ",temp_card);*/
/*	}*/
/*	*/
/*	fclose(temp_2);*/
	
}

void printCards(MPI_Datatype cardDatatype){
		
	// abertura arquivo temporario 2
/*	char* path_temp_2 = "temp_2.txt";*/
/*	FILE* temp_2 = open_file(path_temp_2,"r");*/
	Card cards[NUM_CARDS];
	int i, countLine;
	for(i=0;i<NUM_CARDS;i++){
		cards[i].msg = (char*)malloc(sizeof(char)*SIZE_MSG);
	}

	// Recebendo msg
	int source =1; 
	int tag = 0;
	int num_elements_msg = 1;
	MPI_Status stats;
	MPI_Recv(cards[0].msg,num_elements_msg,cardDatatype, source, tag, MPI_COMM_WORLD,&stats);
	MPI_Recv(cards[1].msg,num_elements_msg,cardDatatype, source, tag, MPI_COMM_WORLD,&stats);
	printf("Processo 2(printCards) recebeu %s de %d(replaceAsterisk).\n", cards[0].msg, source);


/*	for(countLine=0;countLine<NUM_CARDS;countLine++){*/
/*		fscanf(temp_2,"%s ",cards[countLine].msg);	*/
/*	}*/

//	for(i=0;i<NUM_CARDS;i++){
	for(i=0;i<2;i++){
		printf("Cartao %d: %s\n",i,cards[i].msg);
	}
//	system("rm temp_1.txt temp_2.txt");
}

