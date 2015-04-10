#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h> 
#include <unistd.h>
#include <string.h>
#include <signal.h>

///////////////// Variables ////////////////
//MACs
#define MAX_PAGE 1023
#define LRU 0
#define LFU 1
#define SIZE = 150;

//Global variables
int maxFrame = -1;
int algorithm = -1; //algorithm, 0 for LRU, 1 for LFU
int frameUsed = 0;

/////////////////// structs //////////////////
typedef enum {false=0, true=1} bool;
typedef enum (invalid=0, valid=1} validBit;

typedef struct pageInfo
{ 
  int frameNumber;
  validBit valBit;
} pageInfoEntry;

typedef struct frame
{
  int pageNumber;
  struct frameEntry *pre;
  struct frameEntry *next;
} frameEntry;


/////////////////// Functions //////////////
//LRU algorithm
int execLRU(FILE* fp)
{
  //pageInfoEntry *pageTable;
  //pageTable=(pageInfoEntry*)malloc(sizeof(pageInfoEntry)*MAX_PAGE);
  pageInfoEntry pageTable[MAX_PAGE];

  
  char* line = NULL;
  size_t len = 0;
  int pageNum;
  int i;
  for(i = 0; i < MAX_PAGE; i++)
  {
    pageTable->frameNumber = -1;
    pageTable->valBit = invalid;
  }

  if(getline(&line, &len, fp) != -1)
  {
    printf("Empty file!");
    exit(EXIT_FAILURE);
  }

  pageNum = atoi(line);
  pageTable[pageNum]->validBit = valid;

  //frameEntry frameTable[maxFrame];
  //frameEntry[0]->pageNum = atoi(pageNum);
  frameEntry* head = createFrame(-1);
  frameEntry* tail = createFrame(-1);
  frameEntry* temp = createFrame(pageNum);
  head->next = temp;
  temp->pre = head;
  tem->next = tail;
  tail->pre = temp;
  frameUsed++; //first frame taken


  frameEntry* fist,last;
  while ( getline(&line, &len, fp) != -1) 
  { 
    pageNum = atoi(line);
    printf("%s", &pageNum); 

    if (pageTable[pageNum].valBit == valid)//if the page is valid 
    {
      //renew the page
      //last frame is the newest
      last = taile->pre;
      //start the iteration from the head(oldest)
      temp = head->next;
      while((temp->next) != NULL)
      {
        //if found the page in the frame
        if((temp->pageNum) == pageNum
        {
          //remove it from current position
          deleteFrame(temp);
          //add it to the end
          insertFrame(last, pageNum);
        }

        temp = temp->next;
      }
    } 
    //if there is still space for new page
    else if (maxFrame > numFrameUsed++)
    {
      addFrame(head,pageNum);
      frameUsed++;
      pageTable[pageNum]->valBit = valid;
    }
    //if there is no free space, used the LRU algorithm to replace
    else 
    {

    }


  }

}  

frameEntry* createFrame(int num)
{
  frameEntry* temp;
  temp = (frameEntry*)malloc(sizeof(frameEntry));
  temp->pageNum = num;
  temp->pre = NULL;
  temp->next = NULL;
  return frameEntry;
}

void deleteFrame(frameEntry* curr)
{
  frameEntry* currFrame = curr;
  (currFrame->pre)->next = currFrame->next;
  (currFrame->next)->pre = currFrame->pre;
}

void insertFrame(frameEntry* curr, int newPageNum)
{
  frameEntry* currFrame = curr;
  frameEntry* newFrame = createFrame(newPageNum);
  frameEntry* nextFrame = currFrame->next;
  currFrame->next = newFrame;
  newFrame->pre = currFrame;
  newFrame->next = nextFrame;
  nextFrame->pre = newFrame;
}


//main
int main(int argc, char *argv[])
{
  if (argc != 4)
  {
    printf("Invalid arguments!");
    exit(EXIT_FAILURE);
  }

  //frame num
   maxFrame = atoi(argv[1]);

  if (maxFrame <= 0)
  {
    printf("Frame Num must be postive!");
    exit(EXIT_FAILURE);
  }
  
  //trace file
  FILE* fp = fopen(argv[2], "r");
  if (fp == NULL)
  {
    printf("Cannot open the file!");
    exit(EXIT_FAILURE);
  }
  //char* references[SIZE] = NULL;
  //size_t len = 0;
  //while ( getline(&references, &len, fp) != -1) 
    //{ printf("%s", line); }


  //algorithm
  char* algo = argv[3];
  if (strcmp(algo, "LRU") == 0) 
  {
    algorithm = 0;

  }
  else if (strcmp(algo, "LFU") == 0) 
  {
    algorithm = 1;
  }
  else
  {
    printf("Invalid algorithm!");
    exit(EXIT_FAILURE);
  }

  fclose(fp);

}

