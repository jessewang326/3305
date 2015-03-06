/* Program to compute Pi using Monte Carlo methods */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <math.h>
#include <sys/time.h>
#include <sched.h>
#define SEED 35791246

/*This function finds the difference between two times */

double computeElapsedTime(struct timeval start_time,
                          struct timeval finish_time)

{
  
  double start_count, end_count, elapsed_time;

  start_count =
     (double)start_time.tv_sec+1.e-6*(double)start_time.tv_usec;
  end_count =
     (double)finish_time.tv_sec+1.e-6*(double)finish_time.tv_usec;

  elapsed_time = (end_count - start_count);

  return elapsed_time;
}

main(int argc, char **argv)
{
  int niter;
  double x,y;
  int i,count=0; /* # of points in the 1st quadrant of unit circle */
  double z;
  double pi;
  int nchildren;
  int policy;
  pid_t pid;  

  if (argc != 3){
    perror("incorrect arguments");
    exit(0);
  }


  niter = atoi(argv[1]);  
  nchildren = atoi(argv[2]);

  struct sched_param param;

   int index;
    for(index = 0; index < nchildren; index++){
      if((index % 2) == 0)
      {policy = SCHED_OTHER;}
      else
      {policy = SCHED_FIFO;}
      param.sched_priority = sched_get_priority_max(policy);

      //fork
      if((pid = fork())<0) 
        perror("Problem forking");

      //children process
      if(pid == 0)
      {
        struct timeval start_time,finish_time;
        double elapsed_time;

        gettimeofday(&start_time,NULL);

        if(sched_setscheduler(0,policy,&param)){
          perror("Error setting scheduler policy");
          exit(EXIT_FAILURE);
        }


        /* initialize random numbers */
        srand(SEED);
        count=0;
        for ( i=0; i<niter; i++) {
          x = (double)rand()/RAND_MAX;
          y = (double)rand()/RAND_MAX;
          z = x*x+y*y;
          if (z<=1) count++;
        }
        pi=(double)count/niter*4;
        gettimeofday(&finish_time,NULL);
        elapsed_time = computeElapsedTime(start_time,finish_time);
        printf("Index %d, # of trials= %d , estimate of pi is %g, time taken %f, algorithm %s\n",index, niter,pi, elapsed_time, policy == 0 ? "RR" : "FIFO");
        exit(0);
      }


      
    }

      int status;
      for(index = 0; index < nchildren; index++) {
        wait(&status);
    }
}
