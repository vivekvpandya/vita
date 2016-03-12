#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <mpi.h>
#include <stddef.h>
#include <unistd.h>
#include <iostream>
#include <map>
#include <algorithm>
#define TRUE 1
#define FALSE !TRUE

typedef struct news {
    char timeStamp[30];
    char title[100];
    char details[500];
} News ;

using namespace std;


News** getNewsArray(char *recvBuffer, int size, int repo_count, int proc_count, int rank) {
    if (rank == 1) {
        printf("Size : %d \n", size);
        printf("Repo_count : %d \n", repo_count);
        printf("Proc_count : %d \n", proc_count);
    }
    News **newsArray = (News **)malloc(sizeof(News *) * proc_count);
    int proc_size = 630 * proc_count;
    int readOn = 1;
    int t = 0;
    int p = 0;
    for ( ; t < proc_count;  t++) {
        if (recvBuffer[p] == '\0' && recvBuffer[p+1] ==  '\0') {
            // skip the extra null entries
            newsArray[t] = NULL;
            } else {
            newsArray[t] = (News *)malloc(sizeof(News) * repo_count);
            int j = 0;
            int s = 0;
            for (; s <  repo_count ; s++) {
                if(rank ==1)
                printf("The News Item : %d \n",s);
                int temp = p + s * (proc_size);
                readOn = 1;
                printf("n");
                
                while(readOn) {
                    
                    if (j == 0) {
                        for( int k = 0; k <29 ; k++) {
                            if(rank == 1)
                            printf("%c",recvBuffer[temp + k]);
                            newsArray[t][s].timeStamp[k] = recvBuffer[temp + k];
                        }
                        newsArray[t][s].timeStamp[29] = '\0';
                        temp = temp + 30;
                        j++;
                        } else if (j == 1) {
                        for( int k = 0; k <99 ; k++) {
                            if(rank == 1)
                            printf("%c",recvBuffer[temp + k]);
                            newsArray[t][s].title[k] = recvBuffer[temp + k];
                        }
                        newsArray[t][s].title[99] = '\0';
                        temp = temp + 100;
                        j++;
                        } else {
                        for( int k = 0; k <499 ; k++) {
                            if(rank == 1)
                            printf("%c",recvBuffer[temp + k]);
                            newsArray[t][s].details[k] = recvBuffer[temp + k];
                        }
                        newsArray[t][s].details[499] = '\0';
                        temp = temp + 500;
                        j = 0;
                        readOn = 0;
                    }
                }
                
            }
            p = p + 630;
        }//readOn = 1;
        
        
    }
    
    
    
    return newsArray;
    
}

News findLatest(News *news, int size) {
    
    //printf( "Current local time and date: %s", asctime(&time) );
    
    News latest = news[0];
    for ( int i = 1; i < size ; i++ ) {
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


void insert_news(map<string,string> &news,News rnews){
    string headline = rnews.title;
    string details = rnews.details;
    
    map<string,string>::iterator it = news.find(headline);
    
    if(it != news.end()){
        string foundDetails = it->second;
        foundDetails+=details;
        it->second = foundDetails;
        } else{
        news.insert(pair<string,string>(headline,details));
    }
    
}

void print_news_items(map<string,string> &news){
    for_each(news.begin(), news.end(), [](pair<string,string> p){
        cout << "--------------------------------------------------------" << endl << "Title: " << p.first << endl << "Report: " << p.second << endl << endl;
    });
}


void print_news(News news){
    printf("News TimeStamp: %s\nNews Title: %s\nNews Details: %s\n*************************************************************************\n",news.timeStamp,news.title,news.details);
}


int main(int argc, char **argv) {
    
    
    const int tag = 0;
    int world_size, world_rank;
    int rep_size, rep_rank;
    int *process_rank;
    
    MPI_Group world_group, new_group;
    MPI_Comm rep_comm,world_comm;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    int repo_count = world_size - 1;
    
    //printf("Rank %d says World Size is %dn", world_rank,world_size);
    
    process_rank =     (int*)malloc(sizeof(int) * (world_size - 1));
    
    for(int i = 0 ; i < repo_count ; i++){
        process_rank[i] = i+1;
    }
    
    
    
    
    
    //printf("Time Stamp : %sn",news1.timeStamp);
    //printf("Title : %sn",news1.title);
    //printf("Details : %sn",news1.details);
    /*
    for (int i = 1 ; i <= count ; i++) {
        printf("News item : %d n", i);
        printf("News TimeStamp: %s n", news[i].timeStamp);
        printf("News Title: %s n", news[i].title);
        printf("News Details: %s n", news[i].details);
    }
    */
    
    
    /*
    News *latestNews = findLatest(news, count);
    printf("Latest News TimeStamp: %s n", latestNews->timeStamp);
    printf("Latest News Title: %s n", latestNews->title);
    printf("Latest News Details: %s n", latestNews->details);
    */
    //%a %b %e %T %Z %Y => Thu Mar  3 22:32:41 IST 2016
    //struct tm time;
    //strptime(news1.timeStamp,"%a %b %e %T %Z %Y",&time);
    //time_t loctime = mktime(&time);
    //printf ( "Current local time and date: %s", asctime (&time) );
    
    
    
    
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
    //MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    
    //Code for the creation of REPORTER COMM goes here.
    
    
    
    MPI_Comm_dup(MPI_COMM_WORLD, &world_comm);
    MPI_Comm_group(world_comm, &world_group);
    MPI_Group_incl(world_group, (world_size - 1), process_rank, &new_group);
    MPI_Comm_create(world_comm, new_group, &rep_comm);
    
    
    //if(rep_comm == MPI_COMM_NULL){
        //    printf("%d is not included in the new reporters comm worldn",world_rank);
    //}
    //printf("%dn",error);
    
    //Get the size of the Comm REPORTER.
    
    if(world_rank == 0){
        //Do editor's task
        //printf("%sn","Waiting for the reporters to send a valid and latest newsn" );
        MPI_Status status;
        News recv_news;
        int number_amount;
        map<string,string> news_db;
        int more_to_recv = TRUE;
        int reps_done = 0;
        
        
        MPI_Iprobe(MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&more_to_recv,&status);
        
        while(TRUE){
            
            MPI_Recv(&recv_news,1,mpi_test_type,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
            //cout << "News received with tag " << status.MPI_TAG << endl;
            
            if(status.MPI_TAG == 1){
                reps_done++;
                
                if(reps_done == repo_count){
                    break;
                }
                }else{
                //print_news(recv_news);
                insert_news(news_db,recv_news);
            }
            
            
            MPI_Iprobe(MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&more_to_recv,&status);
        }
        
        
        cout << "Received all the messages..\nPrinting the news items\n";
        
       
        print_news_items(news_db);
        
        
        
        }else {
        
        
        
        MPI_Comm_size(rep_comm, &rep_size);
        MPI_Comm_rank(rep_comm, &rep_rank);
        
        char * dirPrefix = "/mirror/local/vita/input/";
        int done =FALSE;
        FILE * fp;
        
        News *news;
        News Test;
        int count = atoi(argv[1]);
        //printf("Count %d",count);
        //news = (News *)malloc(sizeof(News)*(count));
        int loadBalance = count % repo_count;
        int actual_count  =  count;
        if (  loadBalance != 0) {
            count = count + ( repo_count - loadBalance ); // make count multiple of repo_count
        }
        
        
        char *buffer = (char *)malloc(sizeof(char)*(count*630)); // 30 + 100 + 500
        int p = 0;
        for (int i = 1 ; i <= count ; i++ ) {
            if( i <= actual_count ) {
                char * line = NULL;
                size_t len = 0;
                ssize_t read;
                char *filePath = (char *)malloc(sizeof(char)*200);
                strcpy(filePath,dirPrefix);
                char *arg = (char *)malloc(sizeof(char)*32);
                snprintf(arg, 32, "%d", i);
                strcat(filePath,arg);
                //printf("So file path is : %sn",filePath);
                fp = fopen(filePath, "r");
                if (fp == NULL) {
                    exit(EXIT_FAILURE);
                }
                //news[i] = (News *)malloc(sizeof(News));
                int j = 0 ;
                while ((read = getline(&line, &len, fp)) != -1) {
                    //printf("Retrieved line of length %zu :n", read);
                    //printf("%s", line);
                    
                    if (j == 0) {
                        //news[i]->timeStamp = (char *)malloc(sizeof(char)*read);
                        //strncpy(news[i].timeStamp,line,read);
                        //news[i].timeStamp[read] = '';
                        strncpy(&(buffer[p]),line,read);
                        if (read < 30) {
                            int k = p + read;
                            for (; k < 30 ; k++) {
                                buffer[k] = '\0';
                            }
                            } else {
                            buffer[p+29] = '\0';
                        }
                        j++;
                        p = p+ 30;
                        } else if ( j ==  1) {
                        //news[i]->title = (char *)malloc(sizeof(char)*read);
                        //strncpy(news[i].title,line,read);
                        //news[i].title[read] = '';
                        strncpy(&(buffer[p]),line,read);
                        if (read < 100) {
                            int k = p + read;
                            for (; k < 100 ; k++) {
                                buffer[k] = '\0';
                            }
                            } else {
                            buffer[p+99] = '\0';
                        }
                        j++;
                        p = p+ 100;
                        } else {
                        //news[i]->details = (char *)malloc(sizeof(char)*read);
                        //strncpy(news[i].details,line,read);
                        //news[i].details[read] = '';
                        strncpy(&(buffer[p]),line,read);
                        if (read < 500) {
                            int k = p + read;
                            for (; k < 500 ; k++) {
                                buffer[k] = '\0';
                            }
                            } else {
                            buffer[p+499] = '\0';
                        }
                        j++;
                        p = p + 500;
                        j = 0;
                    }
                    
                }
                fclose(fp);
                if (line)
                free(line);
                } else {
                // just fill  characters
                int ite = p;
                for ( ; ite < 630 ; ite++) {
                    buffer[ite];
                }
                p = p + 630;
            }
        }
        
        if (rep_rank == 5) {
            int limit = count * 630 -1;
            for (int i = 0 ; i < limit; i++) {
                printf("%c", buffer[i]);
            }
        }
        /*
        for (int i = 1 ; i <= count ; i++) {
            printf("News item : %d n", i);
            printf("News TimeStamp: %s n", news[i].timeStamp);
            printf("News Title: %s n", news[i].title);
            printf("News Details: %s n", news[i].details);
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
            printf("nRank %d sending n %s  n %s n %sn", rank, news[1]->timeStamp, news[1]->title, news[1]->details);
        }
        if(rank == 2) {
            MPI_Status status;
            const int src = 1;
            News recv;
            MPI_Recv(&recv, 1, mpi_test_type, src, tag, MPI_COMM_WORLD, &status);
            printf("nRank %d received n %s n %s n %s n", rank, recv.timeStamp,recv.title,recv.details);
        }
        
        */
        //News *recvNews = (News *)malloc(sizeof(News )*(count));
        char *recvBuffer = (char *)malloc(sizeof(char) * (count*630));
        int proc_count = count / repo_count;
        int proc_size = 630 * ( proc_count ) ; // this should always be divisible
        /*
        if ( rep_rank == 1) {
            printf("Repo count : %d n", repo_count);
            printf("Count : %d n", count);
            printf("Proc_size : %d n", proc_size );
        }
        */
        int status = MPI_Alltoall(buffer,proc_size,MPI_CHAR,recvBuffer,proc_size,MPI_CHAR,rep_comm);
        
        if(status != 0) {
            printf("MPI_Alltoall failed with status %d\n", status);
            exit(EXIT_FAILURE);
        }
        //MPI_Barrier(rep_comm);
        printf(" \n \n \n");
        /*
        if (rank == 1) {
            int limit = count * 630 -1;
            for (int i = 0 ; i < limit; i++) {
                printf("%c", recvBuffer[i]);
            }
            
        }
        */
        //MPI_Barrier(MPI_COMM_WORLD);
        News** newsArray = getNewsArray(recvBuffer,count,repo_count,proc_count,9999);
        
        
        //if (rep_rank == 0) {
            /*
            for( int n_items = 0; n_items < proc_count ;  n_items++) {
                printf("News item : %d n",n_items);
                for ( int r_count = 0; r_count < repo_count; r_count++) {
                    printf("Time stamp : %s n", newsArray[n_items][r_count].timeStamp);
                    printf("Title : %s n", newsArray[n_items][r_count].title);
                    printf("Details : %s n", newsArray[n_items][r_count].details);
                }
            } */
            /*
            int in = 0;
            for( int n_items = 0; n_items < proc_count ;  n_items++) {
                
                for ( int r_count = 0; r_count < repo_count; r_count++) {
                    printf("n");
                    for (int i = 0; i < 29; i++) {
                        printf("%c",recvBuffer[in+i]);
                    }
                    in = in + 29;
                    for (int i = 0; i < 99; i++) {
                        printf("%c",recvBuffer[in+i]);
                    }
                    in = in + 99;
                    for (int i = 0; i < 499; i++) {
                        printf("%c",recvBuffer[in+i]);
                    }
                    in = in + 499;
                }
            } */
            /*
            for( int n_items = 0; n_items < proc_count ;  n_items++) {
                
                for ( int r_count = 0; r_count < repo_count; r_count++) {
                    printf("n");
                    printf("Timestamp : %s n", newsArray[n_items][r_count].timeStamp);
                    printf("Title : %s n", newsArray[n_items][r_count].title);
                    printf("Details : %s n", newsArray[n_items][r_count].details);
                }
            }
            */
        //}

        
        
        
        for( int my_rank = 0; my_rank < repo_count; my_rank++) {
            //printf("Proc_count : %d n",proc_count);
            if( my_rank == rep_rank ) {
                int news_item = 0;
                for ( int news_item = 0 ; news_item < proc_count ; news_item++ ) {
                    if (newsArray[news_item] != NULL){
                        News latestNews = findLatest(newsArray[news_item],repo_count);
                        //TAG 0 is used when there is some news to be sent to the editor.
                        MPI_Send(&latestNews,1,mpi_test_type,0,0,MPI_COMM_WORLD);
                        printf("***********Latest news %d : on node %d************** \n ",news_item, my_rank);
                        print_news(latestNews);
                    }
                }

                done = TRUE;
        		//Tag 1 is used when there is no news to be sent to the editor
        		// and hence signal that the work has been done.
        		News dummynews;
        		//cout << "Sending the done flag from node " << rep_rank << endl;
       			 MPI_Send(&dummynews,1,mpi_test_type,0,1,MPI_COMM_WORLD);
        
                
            }
        }
        
      
        /*
        else if(rank == 1) {
            for (int i = 1 ; i <= size ; i++) {
                printf("News item : %d rank %dn", i , rank);
                printf("News TimeStamp: %s rank %dn", recvNews[i].timeStamp,rank);
                printf("News Title: %s rank %dn", recvNews[i].title,rank);
                printf("News Details: %s rank %d n", recvNews[i].details,rank);
            }
        }
        */
        //dumpRecvNews(,recvNews,size);
        
        
        //free the derived data type
        
    }
    
    
    
    
    
    MPI_Type_free(&mpi_test_type);
    MPI_Finalize();
    
    
    
    exit(EXIT_SUCCESS);
}