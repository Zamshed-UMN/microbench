#include <stdio.h>
#include <stdlib.h>
#include "omp.h"
#include <math.h>
#include <stdint.h>
#define MAXTHREADS 10000

int NUM_THREADS, NUM_BARRIERS;

typedef struct flags {
    int myflags[2][MAXTHREADS];
    int *partnerflags[2][MAXTHREADS];
}flags;

void dissemination_barrier( flags *localflags, int *sense, int *parity, int *proc) {
    int p = *parity, i;

    for(i=0; i<*proc; i++) {
	#pragma omp critical 
	{
            *localflags->partnerflags[p][i] = *sense;
	}
        while( localflags->myflags[p][i] != *sense ){}
    }

    if( *parity == 1 )
        *sense = !*sense;
    *parity = 1 - *parity;
}


int main(int argc, char **argv)
{
    if(argc==2) {
        NUM_THREADS = atoi(argv[1]);
    }
    
    else{
        printf("Syntax:\n./dissemination num_threads num_barriers\n");
        exit(-1);
    }


    flags allnodes[NUM_THREADS]; //shared
    omp_set_num_threads( NUM_THREADS );
    int proc = ceil(log(NUM_THREADS)/log(2));
    double a;
    long i, k;  

    #pragma omp parallel private (a,k) shared(allnodes, proc)
    {
        int thread_num = omp_get_thread_num();
        int numthreads = omp_get_num_threads();
        int r, j, x;
        int parity = 0; //processor private
        int sense = 1; //processor private
        flags *localflags = &allnodes[thread_num]; //processor private
        int temp, y;

        #pragma omp critical
            for(x=0; x<NUM_THREADS; x++)
                for(r=0; r<2; r++)
                    for(k=0; k<proc; k++)
                        allnodes[x].myflags[r][k] = 0;

        //        printf("Hello world from thread %d of %d\n", thread_num, numthreads);

                #pragma omp critical
                for(j=0; j<NUM_THREADS; j++)
                    for(k=0; k<proc; k++){temp=ceil(pow(2,k));
                    if( j == (thread_num+temp)%NUM_THREADS ){
                        allnodes[thread_num].partnerflags[0][k] =  &allnodes[j].myflags[0][k];
                        allnodes[thread_num].partnerflags[1][k] =  &allnodes[j].myflags[1][k];
                    }
            }

	#pragma omp for nowait
	for (k=0; k<100000; k++)
	{        
        a+=a*2.7;
        a+=a*3.7;
        a+=a*10.7;
        a+=a*9.7;
        a+=a*2.3;
	}
	printf("Hello world from thread %d of %d\n", thread_num, numthreads);
        dissemination_barrier(localflags, &sense, &parity, &proc);
        printf("Hello world from thread %d of %d after barrier\n", thread_num, numthreads);

	#pragma omp for nowait
        for (i=0; i<1000; i++)
        {
        a+=a*2.7;
        a+=a*3.7;
        a+=a*10.7;
        a+=a*9.7;
        a+=a*2.3;
         }

    }
}

