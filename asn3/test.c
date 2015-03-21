#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

int n = 1;
int N = 100;
pthread_mutex_t mutex;

int pv=1;
void *func1()
{
  while(n < N+1)
  {
    if(pv==1)//
    {
      pthread_mutex_lock(&mutex);
      printf("%d 1\n",n);
      n++;
      pv=2;//
      pthread_mutex_unlock(&mutex);
    }
  }
  if(n == N+1)
    pthread_exit(NULL);
}

void *func2()
{
  while(n < N+1)
  {
    if(pv==2)
    {
      pthread_mutex_lock(&mutex);
      printf("%d 2\n",n);
      n++;
      pv=1;
      pthread_mutex_unlock(&mutex);
    }
  }
  if(n == N+1)
    pthread_exit(NULL);
}

int main(int argc, char** argv)
{
  pthread_t pid1,pid2;

  pthread_mutex_init(&mutex, NULL);

  if((pthread_create(&pid1,NULL,func1,NULL)) != 0)
    printf("pthread create error\n");
  if((pthread_create(&pid2,NULL,func2,NULL)) != 0)
    printf("pthread create error\n");

  pthread_join(pid1,NULL);
  pthread_join(pid2,NULL);

  return 0;
}
