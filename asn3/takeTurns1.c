/*
 * Assignment 3
 * Author: Jiaxi Wang
 * Stuent #: 250620848
 * Email: jwang724@uwo.ca
 *
 * File:takeTurns1.c
 *
 * Note: I would like to use 1 late coupon for this assignment
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

static int ARG;
int turn = 0;
pthread_mutex_t lock;

void *opA(void *arg)
{
  int a = ARG;
  while(a > 0)
  {
    if(turn == 0)
    {
      pthread_mutex_lock(&lock);
      printf("Thread A executing opA\n");
      turn = 1;
      pthread_mutex_unlock(&lock);
      a--;
    }
  }
  return NULL;
}

void *opB(void *arg)
{
  int b = ARG;
  while(b > 0)
  {
    if(turn == 1)
    {
      pthread_mutex_lock(&lock);
      printf("Thread B executing opB\n");
      turn = 0;
      pthread_mutex_unlock(&lock);
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


  if(pthread_mutex_init(&lock, NULL) != 0)
  {
    printf(" mutex init failed\n");
    return 1;
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

  pthread_mutex_destroy(&lock);

  return 0;
}

