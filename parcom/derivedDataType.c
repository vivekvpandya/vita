//program to show the implementation of basic derived data types of MPI
#include <mpi.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

//Test structure
typedef struct test{
	int one;
	double two;
	char news[100];
} Test;

int main(int argc, char *argv[]){
	const int tag = 0;
	int size, rank;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	//number of items inside structure Test
	const int nitems = 3;

	//count of item of each type inside Test in order
	int blocklengths[3] = {1, 1, 1};

	MPI_Datatype mpi_string;

	MPI_Type_contiguous(100,MPI_CHAR,&mpi_string);
	MPI_Type_commit(&mpi_string);

	//data types present inside Test in order
	MPI_Datatype types[3] = {MPI_INT, MPI_DOUBLE, mpi_string};

	//name of derived data type
	MPI_Datatype mpi_test_type;

	//array to store starting address of each item inside Test
	MPI_Aint offsets[3];

	//offset of each item in Test with respect to base address of Test
	offsets[0] = offsetof(Test, one);
	offsets[1] = offsetof(Test, two);
	offsets[2] = offsetof(Test, news);

	//create the new derived data type
	MPI_Type_create_struct(nitems, blocklengths, offsets, types, &mpi_test_type);

	//commit the new data type
	MPI_Type_commit(&mpi_test_type);

	//get rank of current process
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if(rank == 0){
		Test send;
		send.one = 1;
		send.two = 2.0;
		strncpy(send.news,"This is simple news.",sizeof(send.news));
		const int dest = 1;
		MPI_Send(&send, 1, mpi_test_type, dest, tag, MPI_COMM_WORLD);
		printf("\nRank %d sending %d, %lf , %s\n", rank, send.one, send.two, send.news);
	}
	if(rank == 1){
		MPI_Status status;
		const int src = 0;
		Test recv;
		MPI_Recv(&recv, 1, mpi_test_type, src, tag, MPI_COMM_WORLD, &status);
		printf("\nRank %d received %d %lf %s \n", rank, recv.one, recv.two, recv.news);
	}

	//free the derived data type
	MPI_Type_free(&mpi_test_type);
	MPI_Finalize();
	return 0;
}