/*
 * Assignment 3
 * Author: Jiaxi Wang
 * Stuent #: 250620848
 * Email: jwang724@uwo.ca
 *
 * File:takeTurns3.c
 *
 * Note: I would like to use 1 late coupon for this assignment
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

static int EX_NUM;
static int THREAD_NUM;
int turn = 0;
int count;
sem_t lock;

struct Argument {
  int id;
};

void *op(void *arg)
{
  struct Argument *ids = (struct Argument*) arg;
  int id = ids -> id;

  while(count > 0)
  {
    if(turn == (id - 1))
    {
      sem_wait(&lock);
      printf("Thread %d executing op%d\n", id, id);
      turn = (turn + 1) % THREAD_NUM;
      sem_post(&lock);
      count--;
    }
  }
  return NULL;
}

int main(int argc, char *argv[])
{
  EX_NUM = atoi(argv[1]);
  THREAD_NUM = atoi(argv[2]);
  count = EX_NUM * THREAD_NUM;
  int i;

  if(argc != 3 || EX_NUM <= 0 || THREAD_NUM <= 0) 
  {
    printf("Wrong arguments\n");
  }

  // Initialize semaphores
  if(sem_init(&lock, 0, 1)) { 
    printf("Fail to initialize semaphore ");
    exit(1);
  }

  // create threads
  struct Argument* thread_arg[THREAD_NUM];
  pthread_t threads[THREAD_NUM];
  for (i = 0; i < THREAD_NUM; i++) 
  {
    thread_arg[i] = (struct Argument*) malloc (sizeof(struct Argument));
    thread_arg[i] -> id = i+1;
    if(pthread_create(&threads[i], NULL, op, (void *)thread_arg[i])) 
    {
      fprintf(stderr,"Error while creating thread A\n");
      exit(1);
    }
  }


  for (i = 0; i < THREAD_NUM; i++)
  {
    if(pthread_join(threads[i], NULL)) 
    {
      fprintf(stderr, "Error while waiting for thread A\n");
      exit(1); 
    }
  }

  sem_destroy(&lock);

  return 0;
}

