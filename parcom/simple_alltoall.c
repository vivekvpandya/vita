#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

int main(int argc, char **argv) {

	const int tag = 0;
	int size, rank;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	
	int *dataToSend = (int *)malloc(sizeof(int) * 8);
	for (int i = 0 ; i < 4; i++) {
		if(rank == i) {
			for( int j = 0; j < 8; j++) {
				dataToSend[j] = rank * j;			
			}		
		}	
	}
   
	printf("Vector befor sending \n");
	for (int i = 0 ; i < 4; i++) {
		if(rank == i) {
			printf("Rank : %d => ",rank);
			for( int j = 0; j < 8; j++) {
				printf("%d ",dataToSend[j]);			
			}		
			printf("\n");
		}	
	}
	int *dataRecv = (int *)malloc(sizeof(int) * 8);
	
	int status = MPI_Alltoall(dataToSend,2,MPI_INT,dataRecv,2,MPI_INT,MPI_COMM_WORLD);
	
	if( status != 0) {
		printf("Error in MPI_Alltoall");
		return -1;	
	}	

	printf("Vector after sending \n");
	for (int i = 0 ; i < 4; i++) {
		if(rank == i) {
			printf("Rank : %d => ",rank);
			for( int j = 0; j < 8; j++) {
				printf("%d ",dataRecv[j]);			
			}		
			printf("\n");
		}	
	}
	
	return 0;
}
