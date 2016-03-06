#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <mpi.h>
#include <stddef.h>
#include <unistd.h>

typedef struct news {
    char timeStamp[30];
    char title[100];
    char details[500];
} News ;


News  findLatest(News *news, int size) { 

    //printf( "Current local time and date: %s", asctime(&time) );
	News latest = news[1];
	for ( int i = 1; i <= size ; i++ ) {
		struct tm time1, time2;
		News newsToCompare = news[i];
    	strptime(latest.timeStamp,"%a %b %e %T %Z %Y",&time1);
		strptime(newsToCompare.timeStamp,"%a %b %e %T %Z %Y",&time2);
    	time_t time1t = mktime(&time1);
		time_t time2t = mktime(&time2);
		double seconds = difftime(time1t, time2t);
		if (seconds < 0) {
			latest = newsToCompare;		
		}
	}
	return latest; 
}

void dumpRecvNews( int rank, News *news, int size) {
	for (int p = 0 ; p < size; p++) {
		if (rank == p) {
			printf("Process %d dump starts ----------------------------- \n\n",p);
			for (int i = 1 ; i <= size ; i++) {
				printf("News item : %d \n", i);
				printf("News TimeStamp: %s \n", news[i].timeStamp);
				printf("News Title: %s \n", news[i].title);
				printf("News Details: %s \n", news[i].details);
			}
			printf("Process %d dump ends ------------------------------ \n\n",p);
			//MPI_Barrier(MPI_COMM_WORLD);
			usleep(10000);
		}
	}
}

int main(int argc, char **argv)
{	char * dirPrefix = "/mirror/local/vita/input/";
    FILE * fp;
    
    News *news;
	News Test;
	int count = atoi(argv[1]);
	//printf("Count %d",count);
	//news = (News *)malloc(sizeof(News)*(count));
	char *buffer = (char *)malloc(sizeof(char)*(count*630)); // 30 + 100 + 500
	int p = 0;
    for (int i = 1 ; i <= count ; i++ ) {
		char * line = NULL;
   	 	size_t len = 0;
    	ssize_t read;
		char *filePath = (char *)malloc(sizeof(char)*200);
		strcpy(filePath,dirPrefix);
		char *arg = (char *)malloc(sizeof(char)*32);
		snprintf(arg, 32, "%d", i);
		strcat(filePath,arg);
		//printf("So file path is : %s\n",filePath);
        fp = fopen(filePath, "r");
		if (fp == NULL) {
		    exit(EXIT_FAILURE); 
		}
		//news[i] = (News *)malloc(sizeof(News));   
		int j = 0 ;
		while ((read = getline(&line, &len, fp)) != -1) {
		    //printf("Retrieved line of length %zu :\n", read);
		    //printf("%s", line);

		    if (j == 0) {
		        //news[i]->timeStamp = (char *)malloc(sizeof(char)*read);
		        //strncpy(news[i].timeStamp,line,read);
		        //news[i].timeStamp[read] = '\0';
				strncpy(&(buffer[p]),line,read);
				if (read < 30) {
					int k = p + read;
					for (; k < 30 ; k++) {
						buffer[k] = '\0';					
					} 
				} else {
					buffer[29] = '\0';				
				}
		        j++;
				p = p+ 30;
		    } else if ( j ==  1) {
		        //news[i]->title = (char *)malloc(sizeof(char)*read);
		        //strncpy(news[i].title,line,read);
		        //news[i].title[read] = '\0';
				strncpy(&(buffer[p]),line,read);
				if (read < 100) {
					int k = p + read;
					for (; k < 100 ; k++) {
						buffer[k] = '\0';					
					} 
				} else {
					buffer[99] = '\0';				
				}
		        j++;
				p = p+ 100;
		    } else {
		       	//news[i]->details = (char *)malloc(sizeof(char)*read);
		        //strncpy(news[i].details,line,read);
		        //news[i].details[read] = '\0';
				strncpy(&(buffer[p]),line,read);
				if (read < 500) {
					int k = p + read;
					for (; k < 500 ; k++) {
						buffer[k] = '\0';					
					} 
				} else {
					buffer[99] = '\0';				
				}
		        j++;
				p = p + 500;
		        j = 0;
		    }  
			    
		}
		fclose(fp);
		if (line)
		    free(line);
    }
/*
	int limit = count * 630 -1; 
	for (int i = 0 ; i < limit; i++) {
		printf("%c", buffer[i]);
	} 
	*/
    //printf("Time Stamp : %s\n",news1.timeStamp);
    //printf("Title : %s\n",news1.title);
    //printf("Details : %s\n",news1.details);
	/*
   for (int i = 1 ; i <= count ; i++) {
				printf("News item : %d \n", i);
				printf("News TimeStamp: %s \n", news[i].timeStamp);
				printf("News Title: %s \n", news[i].title);
				printf("News Details: %s \n", news[i].details);
	}
	*/


	/*
	News *latestNews = findLatest(news, count);
	printf("Latest News TimeStamp: %s \n", latestNews->timeStamp);
	printf("Latest News Title: %s \n", latestNews->title);
	printf("Latest News Details: %s \n", latestNews->details);
	*/
    //%a %b %e %T %Z %Y => Thu Mar  3 22:32:41 IST 2016
    //struct tm time;
    //strptime(news1.timeStamp,"%a %b %e %T %Z %Y",&time);
    //time_t loctime = mktime(&time);
    //printf ( "Current local time and date: %s", asctime (&time) );

	
    const int tag = 0;
	int size, rank;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	//number of items inside structure Test
	const int nitems = 3;

	//count of item of each type inside Test in order
	int blocklengths[3] = {1, 1, 1};

	MPI_Datatype mpi_timestamp;
	MPI_Datatype mpi_title;
	MPI_Datatype mpi_details;

	MPI_Type_contiguous(100,MPI_CHAR,&mpi_title);
	MPI_Type_commit(&mpi_title);

	MPI_Type_contiguous(30,MPI_CHAR,&mpi_timestamp);
	MPI_Type_commit(&mpi_timestamp);

	MPI_Type_contiguous(500,MPI_CHAR,&mpi_details);
	MPI_Type_commit(&mpi_details);


	//data types present inside Test in order
	MPI_Datatype types[3] = {mpi_timestamp, mpi_title, mpi_details};

	//name of derived data type
	MPI_Datatype mpi_test_type;

	//array to store starting address of each item inside Test
	MPI_Aint offsets[3];

	//offset of each item in Test with respect to base address of Test
	offsets[0] = offsetof(News, timeStamp);
	offsets[1] = offsetof(News, title);
	offsets[2] = offsetof(News, details);

	//create the new derived data type
	MPI_Type_create_struct(nitems, blocklengths, offsets, types, &mpi_test_type);

	//commit the new data type
	MPI_Type_commit(&mpi_test_type);

	//get rank of current process
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	/*
	for (int i = 1 ; i <= count ; i++) {
				printf("News item : %d \n", i);
				printf("News TimeStamp: %s \n", news[i].timeStamp);
				printf("News Title: %s \n", news[i].title);
				printf("News Details: %s \n", news[i].details);
	}
	MPI_Barrier(MPI_COMM_WORLD);
	/*
	if(rank == 1) {
		// News send;
		// News.one = 1;
		// News.two = 2.0;
		// strncpy(send.news,"This is simple news.",sizeof(send.news));
		const int dest = 2;
		MPI_Send(news[1], 1, mpi_test_type, dest, tag, MPI_COMM_WORLD);
		printf("\nRank %d sending \n %s  \n %s \n %s\n", rank, news[1]->timeStamp, news[1]->title, news[1]->details);
	}		
	if(rank == 2) {
		MPI_Status status;
		const int src = 1;
		News recv;
		MPI_Recv(&recv, 1, mpi_test_type, src, tag, MPI_COMM_WORLD, &status);
		printf("\nRank %d received \n %s \n %s \n %s \n", rank, recv.timeStamp,recv.title,recv.details);
	}
	
	*/
	//News *recvNews = (News *)malloc(sizeof(News )*(count));
	char *recvBuffer = (char *)malloc(sizeof(char) * (count*630));
	
	int status = MPI_Alltoall(buffer,630,MPI_CHAR,recvBuffer,630,MPI_CHAR,MPI_COMM_WORLD);	
	
	if(status != 0) {
		printf("MPI_Alltoall failed with status %d\n", status);
		exit(EXIT_FAILURE);	
	}	
	MPI_Barrier(MPI_COMM_WORLD);
	printf(" \n \n \n");

	if (rank == 1) {
	int limit = count * 630 -1; 
	for (int i = 0 ; i < limit; i++) {
		printf("%c", recvBuffer[i]);
	} 
		
	} 
	/*
	if (rank == 0) {
	for (int i = 1 ; i <= size ; i++) {
				printf("News item : %d rank %d\n", i , rank);
				printf("News TimeStamp: %s rank %d\n", recvNews[i].timeStamp,rank);
				printf("News Title: %s rank %d\n", recvNews[i].title,rank);
				printf("News Details: %s rank %d \n", recvNews[i].details,rank);
	}
	}
	else if(rank == 1) {
	for (int i = 1 ; i <= size ; i++) {
				printf("News item : %d rank %d\n", i , rank);
				printf("News TimeStamp: %s rank %d\n", recvNews[i].timeStamp,rank);
				printf("News Title: %s rank %d\n", recvNews[i].title,rank);
				printf("News Details: %s rank %d \n", recvNews[i].details,rank);
	}
	}
	*/
	//dumpRecvNews(,recvNews,size);	

	
	//free the derived data type
	MPI_Type_free(&mpi_test_type);
	MPI_Finalize();
	
	

    exit(EXIT_SUCCESS);
}
