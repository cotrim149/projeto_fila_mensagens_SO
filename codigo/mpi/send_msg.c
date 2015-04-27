#include <stdio.h>
#include "mpi.h"

#define STRING_LENGTH 3

typedef struct {
	int var;
	char string[STRING_LENGTH];
	double foo;
} Bar;

MPI_Datatype createNewType(){
	int count = 3;
	int lengths[3] = {1, STRING_LENGTH, 1};
	
	MPI_Aint offsets[3] = {0, sizeof(int), sizeof(int) + STRING_LENGTH};
	MPI_Datatype types[3] = {MPI_INT, MPI_CHAR, MPI_DOUBLE};

	MPI_Datatype barDatatype;
	MPI_Type_create_struct(count, lengths, offsets, types, &barDatatype);
	MPI_Type_commit(&barDatatype);
	
	return barDatatype;
}

int main(int argc, char* argv[]){
	int numtasks,rank;
	int tag;
//	MPI_Datatype type_msg = MPI_INT;
	int num_elements_msg = 1;
	MPI_Status stats;

	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD,&numtasks);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	
	MPI_Datatype barDatatype = createNewType();
	
	if(rank == 0){
		//int msg_num = 42;
		int dest = 1;
		tag = 0;
		Bar bar;
		bar.var = 12;
		bar.string[0]='m';
		bar.string[1]='a';
		bar.string[2]='n';
		bar.foo = 15;
//		MPI_Send(&msg_num,num_elements_msg,type_msg,dest,tag,MPI_COMM_WORLD);
		MPI_Send(&bar,num_elements_msg,barDatatype,dest,tag,MPI_COMM_WORLD);
		printf("Processo %d enviou %s para %d.\n", rank, bar.string, dest);
	}
	
	if(rank == 1){
		int source =0; 
		tag = 0;
//		int msg_rcv=0;
		Bar bar;
//		MPI_Recv(&msg_rcv,num_elements_msg,type_msg, source, tag, MPI_COMM_WORLD,&stats);
		MPI_Recv(&bar,num_elements_msg,barDatatype, source, tag, MPI_COMM_WORLD,&stats);
		printf("Processo %d recebeu %s de %d.\n", rank, bar.string, source);
/*		int dest = 2;*/
/*		MPI_Send(&msg_rcv,num_elements_msg,type_msg,dest,tag,MPI_COMM_WORLD);*/
/*		printf("Processo %d enviou %d para %d.\n", rank, msg_rcv, dest);*/
	}
	
/*	if(rank == 2){*/
/*		int source=1;*/
/*		tag =0;*/
/*		int msg_rcv=0;*/
/*		MPI_Recv(&msg_rcv,num_elements_msg,type_msg,source,tag, MPI_COMM_WORLD,&stats);*/
/*		printf("Processo %d recebeu %d de %d.\n", rank, msg_rcv, source);*/
/*	}*/
	
	MPI_Finalize();
	return 0;
}
