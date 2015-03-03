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

main(int argc, char **argv)
{
  int niter;
  double x,y;
  int i,count=0; /* # of points in the 1st quadrant of unit circle */
  double z;
  double pi;
  int nchildren;
  float x = 0.1;
  int policy;

  if (argc != 3){
    perror("incorrect arguments");
    exit(0);
  }


  niter = atoi(argv[1]);  
  nchildren = atoi(argv[2]);

  struct sched_param param;

  int index;
  for(index = 0; i < nchildren; index++){
    if((index % 2) == 0)
      {policy = SCHED_OTHER;}
    else
      {policy = SCHED_FIFO;}
    param.sched_priority = sched_get_priority_max(policy);

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
    printf("# of trials= %d , estimate of pi is %g \n",niter,pi);
  }
}
