/*
 * Assignment 3
 * Author: Jiaxi Wang
 * Stuent #: 250620848
 * Email: jwang724@uwo.ca
 *
 * File:twoProducers.c
 *
 * Note: I would like to use 1 late coupon for this assignment
 */

#include <stdio.h>
#include <stdlib.h>
#include<unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define PRO_NUM 2

static int BUFF1;
static int BUFF2;

int p_num1, p_num2, total;
int stock1 = 0, stock2 = 0;
sem_t p_lock, c_lock;

struct Argument {
  int id;
};

void *produce(void *arg)
{
  struct Argument *ids = (struct Argument*) arg;
  int id = ids -> id;

  if(id == 1)
  {
    while(p_num1 > 0)
    {
      if(stock1<BUFF1)
      {
        sem_wait(&p_lock);
        stock1++;
        printf("producer 1 has put an item in its buffer, stock1 is %d, %d left\n", stock1, (p_num1 - 1));
        sem_post(&p_lock);
        p_num1--;
      }
    }
  } 
  else
  {
    while(p_num2 > 0)
    {
      if(stock2<BUFF2)
      {
        sem_wait(&p_lock);
        stock2++;
        //printf("producer 2 has put an item in its buffer\n");
        printf("producer 2 has put an item in its buffer, stock2 is %d, %d left\n", stock2, (p_num2 - 1));
        sem_post(&p_lock);
        p_num2--;
      }
    }
  }

  return NULL;
}

void *consume(void *arg)
{
  while(total > 0)
  {
    if(stock1 > 0)
    {
      sem_wait(&c_lock);
      stock1--;
      sem_post(&c_lock);
      //printf("Consumer has consumed an item from producer 1\n");
      total--;
      printf("Consumer has consumed an item from producer 1, stock1 is %d, %d left\n", stock1, total);
    }
    else if(stock2 > 0)
    {
      sem_wait(&c_lock);
      stock2--;
      sem_post(&c_lock);
      //printf("Consumer has consumed an item from producer 2\n");
      total--;
      printf("Consumer has consumed an item from producer 2, stock2 is %d, %d left\n", stock2, total);
    }
  }
  return NULL;
}

int main(int argc, char *argv[])
{
  BUFF1 = atoi(argv[1]);
  BUFF2 = atoi(argv[2]);
  p_num1 = atoi(argv[3]);
  p_num2 = atoi(argv[4]);
  total = p_num1 + p_num2;

  int i;

  if(argc != 5 || (BUFF1*BUFF2*p_num1*p_num2) < 0) 
  {
    printf("Wrong arguments\n");
  }

  // Initialize semaphores
  if(sem_init(&p_lock, 0, 1)) { 
    printf("Fail to initialize semaphore ");
    exit(1);
  }
  if(sem_init(&c_lock, 0, 1)) { 
    printf("Fail to initialize semaphore ");
    exit(1);
  }

  // create threads
  struct Argument* thread_arg[PRO_NUM];
  pthread_t producers[PRO_NUM];
  pthread_t consumer;
  for (i = 0; i < PRO_NUM; i++) 
  {
    thread_arg[i] = (struct Argument*) malloc (sizeof(struct Argument));
    thread_arg[i] -> id = i+1;
    if(pthread_create(&producers[i], NULL, produce, (void *)thread_arg[i])) 
    {
      fprintf(stderr,"Error while creating thread A\n");
      exit(1);
    }
  }
  if(pthread_create(&consumer, NULL, consume, NULL)) 
  {
    fprintf(stderr,"Error while creating thread A\n");
    exit(1);
  }



  for (i = 0; i < PRO_NUM; i++)
  {
    if(pthread_join(producers[i], NULL)) 
    {
      fprintf(stderr, "Error while waiting for thread A\n");
      exit(1); 
    }
  }
  if(pthread_join(consumer, NULL)) 
  {
    fprintf(stderr, "Error while waiting for thread A\n");
    exit(1); 
  }

  sem_destroy(&p_lock);
  sem_destroy(&c_lock);

  return 0;
}

