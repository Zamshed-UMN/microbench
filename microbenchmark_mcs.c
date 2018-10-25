#include <omp.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
//#include "openmpbarrier.h"
#include <stdint.h>
/*MCS tree barrier*/

static int iterations;
static double threadstime[256];

typedef struct mcs_node_omp
{
    union {
        uint8_t unit[4];
        uint32_t pack;
    }childnotready;
    union {
        uint8_t unit[4];
        uint32_t pack;
    }havechild;
    uint8_t *parentpointer;
    bool *childpointer[2];
    bool parentsense;
    bool dummy;
    bool sense;
    /* data */
}MCSompNode __attribute__((aligned(8)));

static MCSompNode* nodes = NULL;

/*initize MCS barrier*/
void mp_mcs_init(int num_threads)
{
    int i,j;
    nodes = (MCSompNode *)malloc(sizeof(MCSompNode)*num_threads);
    memset(nodes,0,sizeof(MCSompNode)*num_threads);
    for (i=0; i<num_threads; i++) {
        if (i==0) {
            nodes[i].parentpointer = (uint8_t *)&(nodes[i].dummy);
//	    printf("nodes[%d].parentpointer=%d \n",i,*nodes[i].parentpointer);
        }
        else {
            nodes[i].parentpointer = &(nodes[(i-1)/4].childnotready.unit[(i-1)%4]);
//	    printf("nodes[%d].parentpointer=%d \n",i,*nodes[i].parentpointer);
        }
        for(j=0;j<4;j++) {
            if(i*4+j+1<num_threads) {
                nodes[i].havechild.unit[j]=1;
//		printf("nodes[%d].havechild.unit[%d]=%d \n",i,j,nodes[i].havechild.unit[j]);
            }
            else {
                nodes[i].havechild.unit[j]=0;
//		printf("nodes[%d].havechild.unit[%d]=%d \n",i,j,nodes[i].havechild.unit[j]);
            }
        }
        nodes[i].childnotready.pack = nodes[i].havechild.pack;
//	printf("nodes[%d].childnotready.pack=%zu\n",i,nodes[i].childnotready.pack);

        //wakeup tree
        for(j=0;j<2;j++) {
            if (i*2+j+1 < num_threads){
                nodes[i].childpointer[j] = &(nodes[i*2+j+1].parentsense);
//		printf("nodes[%d].childpointer[%d]=%d\n",i,j,nodes[i].childpointer[j]);
		}
            else 
		{
                nodes[i].childpointer[j] = &(nodes[i].dummy);
//		printf("nodes[%d].childpointer[%d]=%d\n",i,j,nodes[i].childpointer[j]);
		}
		
        }
        nodes[i].parentsense = false;
        nodes[i].sense = true;
    }

}


void mp_mcsbarrier(int num_threads)
{
    int threadno = omp_get_thread_num();
    while (nodes[threadno].childnotready.pack);
    nodes[threadno].childnotready.pack = nodes[threadno].havechild.pack;
	fprintf(stderr,"nodes[%d].childnotready.pack=%d\n",threadno,nodes[threadno].childnotready.pack);
    *nodes[threadno].parentpointer = 0;
    if (threadno!=0)
        while(nodes[threadno].parentsense!=nodes[threadno].sense);
    *nodes[threadno].childpointer[0] = nodes[threadno].sense;
    *nodes[threadno].childpointer[1] = nodes[threadno].sense;
    fprintf(stderr, "*nodes[%d].childpointer[0]=%d\n",threadno,*nodes[threadno].childpointer[0]);
    fprintf(stderr, "*nodes[%d].childpointer[1]=%d\n",threadno,*nodes[threadno].childpointer[1]);
    nodes[threadno].sense = !nodes[threadno].sense;
    fprintf(stderr, "*nodes[%d].sense=%d\n",threadno,nodes[threadno].sense);
    
    
}


void mp_mcsbarrier_destroy()
{
    free(nodes);
    nodes = NULL;
}



double timeduration(struct timeval t1, struct timeval t2)
{
    return (t2.tv_sec-t1.tv_sec)*1000000+(t2.tv_usec-t1.tv_usec);
}


int main(int argc, char **argv)
{
    int thread_no;
    int numberofthreads = 0 ;
    int i;
    if (argc >=2) {
        numberofthreads = atoi(argv[1]);
   //     iterations = atoi(argv[2]);
    }
    else {
        printf("usage: test numberofthreads numberofiterations (numberofthreads no larger than %d)\n",omp_get_max_threads());
        return 0;
    }
    long z;
    double a;
    mp_mcs_init(numberofthreads);
    double averagetime=0.0;
    #pragma omp parallel private(thread_no, a, z) num_threads(numberofthreads)
    //#pragma omp parallel num_threads(numberofthreads)
    {
        struct timeval t1, t2;
        double duration=0.0;
        thread_no = omp_get_thread_num();
        int i;
	a=1.2;
//         printf("Thread %d is at the barrier\n", thread_no);
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

        mp_mcsbarrier(numberofthreads);
        fprintf(stderr, "Thread %d has crossed the barrier\n", thread_no);
#pragma omp for nowait
        for (z=0; z<1000; z++)
        {
        a+=a*2.7;
        a+=a*3.7;
        a+=a*10.7;
        a+=a*9.7;
        a+=a*2.3;
         }

    }
    
    mp_mcsbarrier_destroy();
    return 0;
}







