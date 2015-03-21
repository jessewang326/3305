/*
 * Assignment 3
 * Author: Jiaxi Wang
 * Stuent #: 250620848
 * Email: jwang724@uwo.ca
 *
 * File:takeTurns2.c
 *
 * Note: I would like to use 1 late coupon for this assignment
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

static int ARG;
int turn;
sem_t lock;

void *opA(void *arg)
{
  turn = 0;
  int a = ARG;
  while(a > 0)
  {
    if(turn == 0)
    {
    sem_wait(&lock);
    printf("Thread A executing opA\n");
    turn = 1;
    sem_post(&lock);
    a--;
    }
  }
  return NULL;
}

void *opB(void *arg)
{
  turn = 1;
  int b = ARG;
  while(b > 0)
  {
    if(turn == 1)
    {
    sem_wait(&lock);
    printf("Thread B executing opB\n");
    turn = 0;
    sem_post(&lock);
    b--;
    }
  }
  return NULL;
}

int main(int argc, char *argv[])
{
  ARG = atoi(argv[1]);

  if(argc != 2 || ARG <= 0) 
  {
    printf("Wrong arguments, this program takes one argument that represents the number of times that thread A and thread B execute their operations\n");
  }


  pthread_t threadA;
  pthread_t threadB;

	// Initialize semaphores
	if(sem_init(&lock, 0, 1)) { 
		printf("Fail to initialize semaphore ");
		exit(1);
	}

  // create threads A&B 
  if(pthread_create(&threadA, NULL, opA, (void *)ARG)) {
    fprintf(stderr,"Error while creating thread A\n");
    exit(1);
  }

  if(pthread_create(&threadB, NULL, opB, (void *)ARG)) {
    fprintf(stderr,"Error while creating thread B\n");
    exit(1);
  }


  if(pthread_join(threadA, NULL)) 
  {
    fprintf(stderr, "Error while waiting for thread A\n");
    exit(1); 
  }

  if(pthread_join(threadB, NULL)) 
  {
    fprintf(stderr, "Error while waiting for thread B\n");
    exit(1); 
  }

  sem_destroy(&lock);

  return 0;
}

