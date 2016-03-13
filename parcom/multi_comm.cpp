#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

int main(int argc , char** argv){
	
	MPI_Init(&argc, &argv);

	int world_size, world_rank;

	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

	if(world_rank == 0 && world_size != 7){
		printf("Please specify 7 number of nodes\n");
		MPI_Abort(MPI_COMM_WORLD,1);
		exit(1);
	}

	
	MPI_Group world_group, two_group,three_group, editor_group;
    MPI_Comm two_comm,world_comm, three_comm, editor_comm;

    int two_rep_grp[2] = {2,3};
    int three_rep_grp[3] = {4,5,6};
    int editor_grp[2] = {0,1};

    int rank;

    MPI_Comm_dup(MPI_COMM_WORLD, &world_comm);
    MPI_Comm_group(world_comm, &world_group);

    //Create two_rep_grp
    MPI_Group_incl(world_group, 2, two_rep_grp, &two_group);
    MPI_Comm_create(world_comm, two_group, &two_comm);

    //Create three_rep_grp
    MPI_Group_incl(world_group, 3, three_rep_grp, &three_group);
    MPI_Comm_create(world_comm, three_group, &three_comm);

    //Create editor_grp
    MPI_Group_incl(world_group, 2, editor_grp, &editor_group);
    MPI_Comm_create(world_comm, editor_group, &editor_comm);

    if(world_rank == 0 || world_rank == 1){
    	//group of editors

    	MPI_Comm_rank(editor_comm, &rank);
    	printf("Editor Rank %d\n", rank);
    }else if(world_rank == 2 || world_rank == 3){
    	//two_group reporters
    	MPI_Comm_rank(two_comm, &rank);
    	printf("Group 1 Reporter Rank %d\n", rank);
    }else{
    	//three_group reporters
    	MPI_Comm_rank(three_comm, &rank);
    	printf("Group 2 Reporter Rank %d\n", rank);
    }


	MPI_Finalize();
	return 0;
}