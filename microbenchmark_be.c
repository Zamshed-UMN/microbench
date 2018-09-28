#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#define BATCH_SIZE 4
//#define NUM_THREADS 8
size_t temp=1000;
int sense2=0, sense3=0,sense4=0;
int NUM_THREADS, NUM_BARRIERS;


static void bulk_barrier( int *count, int *sense, int *local_sense, long *wait ){
//int f;
int thread_num=omp_get_thread_num();
*local_sense = !*local_sense;
if (__sync_sub_and_fetch (count, 1) == 1) {
    *count = NUM_THREADS;
    *sense = *local_sense;
//    *wait=(__sync_sub_and_fetch (&temp, 100));
     //for (f=0;f<*wait;f++);    
} else {//*wait=(__sync_sub_and_fetch (&temp, 100));
    while (*sense != *local_sense);
}
/*if (thread_num%BATCH_SIZE==0) {
                        usleep(5);
                        sense2=1;
        } else {
                        while (sense2==0);
                        usleep(5000);
                
        }*/
if (thread_num%BATCH_SIZE==0) {
                        usleep(5);
                        sense2=1;
        } else if (thread_num%BATCH_SIZE==1){
                        while (sense2==0);
                        usleep(200);
			sense3=1;}
else if (thread_num%BATCH_SIZE==2){
                        while (sense3==0);
                        usleep(200);
			sense4=1;}
else{
                        while (sense4==0);
                        usleep(200);}
}


int main(int argc, char **argv)
{
  if( argc == 2 ){
      NUM_THREADS = atoi( argv[1] );
     // NUM_BARRIERS = atoi( argv[2] );
  }

  else{
      printf("Syntax:\n./centralized num_threads num_barriers\n");
      exit(-1);
  }
   long k;
   float a=1.2; 
  // Serial code
//  printf("This is the serial section\n");
  omp_set_num_threads( NUM_THREADS );
  int sense = 1, count = NUM_THREADS;
  double time1, time2;

    #pragma omp parallel private (a,k) shared(sense, count)         
    {
            int num_threads = omp_get_num_threads();
        int thread_num = omp_get_thread_num();
        int local_sense = 1;
        long i,j, wait=0;
 	a=1.2;
//        for(j=0; j<NUM_BARRIERS; j++){
//            printf("Hello World from thread %d of %d.\n", thread_num, num_threads);

#pragma omp for nowait
	for (k=0; k<100000; k++)
	{        
        a+=a*2.7;
        a+=a*3.7;
        a+=a*10.7;
        a+=a*9.7;
        a+=a*2.3;
	}
            bulk_barrier(&count, &sense, &local_sense, &wait);

            printf("Hello World from thread %d of %d after barrier\n", thread_num, num_threads);
       // }
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

  return 0;
}



