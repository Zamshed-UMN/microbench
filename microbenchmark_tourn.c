#include <omp.h>
#include <stdio.h>
#include <math.h>
#include<stdlib.h>
#include <malloc.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
//#define numprocessors 16
#define rounds_first ceil(log(numprocessors)/log(2))
#define rounds1 rounds_first
#define winner 0
#define loser 1
#define bye 2
#define champion 3
#define dropout 4
int NUM_THREADS, NUM_BARRIERS;

struct round_struct {
    int role;
    int vpid;
    int tb_round;
    bool *opponent;
    bool flag;
};
typedef struct round_struct round_struct;
round_struct array[1000][100];

void barrier( int vpid, bool *sense,int rounds) {
    //fprintf(stderr,"Hello from thread %d before barrier\n",omp_get_thread_num());
    int round = 0;
    int id=omp_get_thread_num(); 	
    while(1) {

        if(array[vpid][round].role == loser){
            *( array[vpid][round] ).opponent = *sense;
	    fprintf(stderr, "Thread %d--> *( array[%d][%d]).opponent=%d\n", vpid, vpid,round,*( array[vpid][round] ).opponent);
            while( array[vpid][round].flag != *sense );
            break;
        }

        if( array[vpid][round].role == winner ) {
            while( array[vpid][round].flag != *sense );
        }

        if( array[vpid][round].role == champion ){
            while( array[vpid][round].flag != *sense );
            *( array[vpid][round] ).opponent = *sense;
	    fprintf(stderr, "Thread %d--> *( array[%d][%d]).opponent=%d\n", vpid,vpid,round,*( array[vpid][round] ).opponent);
            break;
        }

        if(round < rounds)
            round = round + 1;
        //i=1;
    }
#pragma omp single nowait 
{
fprintf(stderr, "Waking up ...\n");	
}
    //wake up
    while(1) {
        if( round > 0 )
            round = round - 1;

        if( array[vpid][round].role == winner ){
            *( array[vpid][round] ).opponent = *sense;
            fprintf(stderr, "Thread %d--> *( array[%d][%d]).opponent=%d\n", vpid,vpid,round,*( array[vpid][round] ).opponent);
	}
        if( array[vpid][round].role == dropout )
            break;
    }

    *sense = !*sense;
    //fprintf(stderr,"thread %d leaving barrier\n",omp_get_thread_num());
}


int main(int argc, char **argv) {
    bool x = false;

    if( argc==2 ){
        NUM_THREADS = atoi(argv[1]);
//        NUM_BARRIERS = atoi(argv[2]);
    }
    else {
        printf("Syntax:\n./tournament num_threads num_barriers\n");
        exit(-1);
    }

    int counter = 0;
    int rounds = ceil( log(NUM_THREADS)/log(2) );

    // Serial code
//    printf("This is the serial section\n");
    double time1, time2;
    int i,j,k,l;
	long z;
    float a; 

    for(j=0;j<NUM_THREADS;j++) {
        for(k=0;k<=rounds;k++) {
            array[j][k].flag = false;
            array[j][k].role = -1;
            array[j][k].opponent = &x;
        }
    }
       
    i=0;
    int temp=0, temp2, g=0, comp, comp_second=0;
//initialization begins
    for(l=0 ; l<NUM_THREADS; l++) {
        for(k=0;k<=rounds;k++) {
            temp = k;
            temp2 = l;
            comp = ceil( pow(2,k) );
            comp_second = ceil( pow(2,k-1) );

            if((k > 0) && (l%comp==0) && ((l + (comp_second))< NUM_THREADS) && (comp < NUM_THREADS)){
                array[l][k].role = winner;
//		fprintf(stderr, "array[%d][%d].role=%d\n", l,k,array[l][k].role);
            }

            if((k > 0) && (l%comp == 0) && ((l + comp_second)) >= NUM_THREADS){
                array[l][k].role = bye;
//		fprintf(stderr, "array[%d][%d].role=%d\n", l,k,array[l][k].role);
            }

            if((k > 0) && ((l%comp == comp_second))){
                array[l][k].role = loser;
//		fprintf(stderr, "array[%d][%d].role=%d\n", l,k,array[l][k].role);
            }

            if((k > 0) && (l==0) && (comp >= NUM_THREADS)){
                array[l][k].role = champion;
//		fprintf(stderr, "array[%d][%d].role=%d\n", l,k,array[l][k].role);
            }

            if(k==0) {
                array[l][k].role = dropout;
//		fprintf(stderr, "array[%d][%d].role=%d\n", l,k,array[l][k].role);
            }

            if(array[l][k].role == loser) {
                array[l][k].opponent = &array[l-comp_second][k].flag;
//		fprintf(stderr, "array[%d][%d].champion=%d\n", l,k,array[l][k].role);
            }

            if(array[l][k].role == winner || array[l][k].role == champion) {
                array[l][k].opponent = &array[l+comp_second][k].flag;
//		fprintf(stderr, "array[%d][%d].opponent=%d\n", l,k,array[l][k].role);
            }
        }
    }


for(j=0;j<NUM_THREADS;j++) {
        for(k=0;k<=rounds;k++) {
//            fprintf(stderr,"array[%d][%d].flag=%d\n", j,k,array[j][k].flag);
            fprintf(stderr,"array[%d][%d].role=%d\n", j,k,array[j][k].role);
//            fprintf(stderr,"array[%d][%d].opponent=%d\n", j,k,*array[j][k].opponent);
        }
    }
        
//initialization ends
#pragma omp parallel num_threads(NUM_THREADS) private (a,z) shared(array,counter)
                {
        int vpid=0;
        bool *sense;
        bool temp = true;
	a=1.2;
        #pragma omp critical
        {
        vpid = omp_get_thread_num();
        sense = &temp;
        }

     int num_threads = omp_get_num_threads();
     int i,j;
    int thread_num = omp_get_thread_num();
        
//    for( i = 0; i < NUM_BARRIERS; i++ ){
        //#pragma omp for
        //    for(j=0; j<50; j++);
        //printf( "Hello World from thread %d of %d.\n", thread_num, num_threads );

//        time1=omp_get_wtime();
//FP task
#pragma omp for nowait
	for (z=0; z<100000; z++)
	{        
        a+=a*2.7;
        a+=a*3.7;
        a+=a*10.7;
        a+=a*9.7;
        a+=a*2.3;
	}
	
        barrier(vpid,sense,rounds);
	
//        printf( "Hello from thread %d of %d after barrier\n", thread_num, num_threads );
#pragma omp for nowait
        for (i=0; i<1000; i++)
        {
        a+=a*2.7;
        a+=a*3.7;
        a+=a*10.7;
        a+=a*9.7;
        a+=a*2.3;
         }
//    }
//    printf( "Time spent in barrier by thread %d is %f\n", thread_num, time2-time1 );
    }
}




