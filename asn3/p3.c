#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>

/*** GLOBAL VARIABLES ***/
static int MAX_CAPACITY;
static int NUM_STUDENTS;
static int NUM_QUESTIONS;
static int TOTAL_QUESTIONS;

sem_t office_capacity_sem; // Used for locking down office when full
sem_t enter_sem; // Used for making sure people don't say they're entering well after someone else says they're waiting
sem_t question_sem; // Used to lock down question asking until an answer is received from the TA

bool question_open;
int questions_answered;
int office_occupancy;

struct Student_arg {
	int id;
	int questions;
};

void *studentPrint(void *arg);
void *taPrint(void *arg);

int main(int argc, char const *argv[])
{
	// Get arguments
	if(argc != 4) {
		printf("Invalid number of arguments.\n");
		exit(1);
	}
	NUM_STUDENTS = atoi(argv[1]);
	MAX_CAPACITY = atoi(argv[2]);
	NUM_QUESTIONS = atoi(argv[3]);
	TOTAL_QUESTIONS = NUM_STUDENTS * NUM_QUESTIONS;

	// Initialize global variables
	question_open = false;
	questions_answered = 0;
	office_occupancy = 0;

	// Initialize semaphores
	if(sem_init(&office_capacity_sem, 0, MAX_CAPACITY)) { 
		printf("Error creating office capacity semaphore");
		exit(1);
	}
	if(sem_init(&enter_sem, 0, 1)) {
		printf("Error creating enter semaphore");
		exit(1);
	}
	if(sem_init(&question_sem, 0, 1)) {
		printf("Error creating question semaphore");
		exit(1);
	}

	// Initialize student parameters
	struct Student_arg* s_args[NUM_STUDENTS];
	int i; for(i = 0; i < NUM_STUDENTS; i++) {
		s_args[i] = (struct Student_arg*)malloc(sizeof(struct Student_arg));
		s_args[i]->id = i+1;
		s_args[i]->questions = 1;
	}

	// Create student threads
	pthread_t student_threads[NUM_STUDENTS];
	for (i = 0; i < NUM_STUDENTS; i++) {
		if(pthread_create(&student_threads[i], NULL, studentPrint, (void *)s_args[i])) {
			printf("Error creating student thread %d\n", i);
			exit(1);
		}
	}

	// Create TA thread
	pthread_t ta_thread;
	if(pthread_create(&ta_thread, NULL, taPrint, NULL)) {
		printf("Error creating TA thread\n");
		exit(1);
	}

	// Join student threads
	for(i = 0; i < NUM_STUDENTS; i++) {
		if(pthread_join(student_threads[i], NULL)) {
			printf("Error joining student thread %d\n", i);
			exit(1);
		}
	}

	// Joint TA thread
	if(pthread_join(ta_thread, NULL)) {
		printf("Error joining TA thread\n");
		exit(1);
	}

	sem_destroy(&office_capacity_sem);
	sem_destroy(&question_sem);
	sem_destroy(&enter_sem);


	return 0;
}

void *studentPrint(void *arg)
{
	struct Student_arg *s = (struct Student_arg*)arg;
	// Try to enter the TA's office
	sem_wait(&enter_sem);
	if(sem_trywait(&office_capacity_sem))
	{
		printf("I am student %d and I'm waiting outside the TA's door\n", s->id);
		sem_wait(&office_capacity_sem);
	}
	// Successfully enter the TA's office
	printf("I am student %d and I'm entering the office\n", s->id);
	sem_post(&enter_sem);

	/*** QUESTIONS MUST BE ASKED HERE ***/
	//
	//
	// Enter the question critical section
	while(s->questions <= NUM_QUESTIONS)
	{
		sem_wait(&question_sem);
		printf("I am student %d and I'm asking question %d\n", s->id, s->questions);
		s->questions += 1;
		question_open = true;
		while(question_open) 
		{// Loop until question gets answered
		}
		sem_post(&question_sem);
	}
	//
	//

	// Leave the office and allow another student to fit in
	printf("I am student %d and I'm leaving the office\n", s->id);
	sem_post(&office_capacity_sem);
}

void *taPrint(void *arg)
{
	while(questions_answered < TOTAL_QUESTIONS)
	{
		// Questions are answered after they are asked! Then the floor
		// is opened up for another question.
		if(question_open)
		{
			printf("I am TA and this will be discussed in class\n");
			questions_answered++;
			question_open = false;
		}
	}
}