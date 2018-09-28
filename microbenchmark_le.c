#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#define SLEEP_us 10
//#define NUM_THREADS 8
//size_t temp=200;
size_t token=32;
int NUM_THREADS, NUM_BARRIERS;


static void le_barrier( int *count, int *sense, int *local_sense, long *wait ){
int f;
//printf("thread %d is entering barrier\n", omp_get_thread_num());
*local_sense = !*local_sense;
if (__sync_fetch_and_sub (count, 1) == 1) {
    *count = NUM_THREADS;
//    *sense = *local_sense;
    *wait=(__sync_sub_and_fetch (&token, 1));
    *sense = *local_sense;
     //for (f=0;f<*wait;f++);    
} else {*wait=(__sync_sub_and_fetch (&token, 1));
    while (*sense != *local_sense);
}
//*wait=(__sync_fetch_and_sub (&temp, 100);
//printf("thread %d is has token #%d\n", omp_get_thread_num(),__sync_fetch_and_add (&token, 1));
//printf("thread %d will sleep %ld us\n", omp_get_thread_num(),*wait*SLEEP_us);

usleep(*wait*SLEEP_us);

//printf("thread %d is leaving barrier\n", omp_get_thread_num());
}

void check_env(){

  char* pPath = getenv ("GOMP_CPU_AFFINITY");
  if (pPath!=NULL)
    printf ("GOMP_CPU_AFFINITY is: %s\n",pPath);
  else {
    printf("GOMP_CPu__AFFINITY Not Set\n");
    putenv("GOMP_CPU_AFFINITY=0");
    pPath = getenv("GOMP_CPU_AFFINITY");
    printf ("GOMP_CPU_AFFINITY is: %s\n",pPath);
    //exit(1);
  }

}


int main(int argc, char **argv)
{
  if( argc == 2 ){
      NUM_THREADS = atoi( argv[1] );
//      NUM_BARRIERS = atoi( argv[2] );
  }

  else{
      printf("Syntax:\n./centralized num_threads num_barriers\n");
      exit(-1);
  }
  
//  check_env();

  double a; 
  // Serial code
//  printf("This is the serial section\n");
  omp_set_num_threads( NUM_THREADS );
  int sense = 1, count = NUM_THREADS;
  //double time1, time2;

#pragma omp parallel private (a) shared(sense, count)     
//#pragma omp critical     
    {
    
        int num_threads = omp_get_num_threads();
        int thread_num = omp_get_thread_num();
        int local_sense = 1;
        long i,j, k, wait=0;
	a=1.2;
//        for(j=0; j<NUM_BARRIERS; j++){
//        printf("Hello World from thread %d of %d.\n", thread_num, num_threads);
	//FP task
	#pragma omp for nowait
	for (k=0; k<100000; k++)
	{
        a+=a*2.7;
        a+=a*3.7;
        a+=a*10.7;
        a+=a*9.7;
        a+=a*2.3;
         }

        le_barrier(&count, &sense, &local_sense, &wait);            
        printf("Hello World from thread %d of %d after barrier\n", thread_num, num_threads);

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


//    }
//printf("The number of processors on this system is %d", omp_get_numprocs());
 
// Resume serial code
//  printf("Back in the serial section again\n");
  return 0;
}



