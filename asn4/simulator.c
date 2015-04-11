/**
 * @author Jiaxi Wang
 * asn4
 * @email jwang724@uwo.ca
 * @note I would like to use 1 lage coupon for this assignment
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

///////////////// Variables ////////////////
//MACs
#define MAX_PAGE 1023
#define LRU 0
#define LFU 1
#define SIZE = 150;

//Global variables
int maxFrame = -1;
int frameUsed = 0;

/////////////////// structs //////////////////
typedef enum {false=0, true=1} bool;
typedef enum {invalid=0, valid=1} validBit;

typedef struct pageInfo
{ 
  validBit valBit;
} pageInfoEntry;

typedef struct frame
{
  int pageNumber;
  int frequency;
  struct frame *pre;
  struct frame *next;
} frameEntry;


/////////////////// Functions //////////////
frameEntry* createFrame(int num, int frq);
void deleteFrame(frameEntry* curr);
void insertFrame(frameEntry* curr, int newPageNum, int frq);
void printFrames(frameEntry *head);


//LRU algorithm
int execLRU(FILE* fp)
{
  //pageInfoEntry *pageTable;
  //pageTable=(pageInfoEntry*)malloc(sizeof(pageInfoEntry)*MAX_PAGE);
  pageInfoEntry pageTable[MAX_PAGE];
  char* line = NULL;
  size_t len = 0;
  int pageNum;
  int faultCount = 0;
  int i;

  //initialization
  for(i = 0; i < MAX_PAGE; i++)
  {
    pageTable[i].valBit = invalid;
  }

  //check file
  if(getline(&line, &len, fp) == -1)
  {
    printf("Empty file!\n");
    exit(EXIT_FAILURE);
  }

  pageNum = atoi(line);
  pageTable[pageNum].valBit = valid;

  //initialize the frames
  frameEntry* head = createFrame(-1, -1);
  frameEntry* tail = createFrame(-1, -1);
  frameEntry* temp = createFrame(pageNum, -1);
  head->next = temp;
  temp->pre = head;
  temp->next = tail;
  tail->pre = temp;
  frameUsed++; //first frame taken

  //printf("%d ", pageNum); 

  frameEntry *first, *last;
  while ( getline(&line, &len, fp) != -1) 
  { 
    //printFrames(head);
    pageNum = atoi(line);
    //printf("%d ", pageNum); 

    if (pageTable[pageNum].valBit == valid)//if the page is valid 
    {
      //printf("(1) "); 
      //renew the page
      //last frame is the newest
      last = tail->pre;
      //start the iteration from the head(oldest)
      temp = head->next;
      while((temp->next) != NULL)
      {
        //if found the page in the frame
        if((temp->pageNumber) == pageNum)
        {
          if(temp->next != tail)
          {
            //remove it from current position
            deleteFrame(temp);
            //add it to the end
            insertFrame(last, pageNum, -1);
            break;
          }
        }

        temp = temp->next;
      }
    } 
    //if there is still space for new page
    else if (maxFrame > frameUsed)
    {
      //printf("(2) "); 
      last = tail->pre;
      insertFrame(last,pageNum, -1);
      frameUsed++;
      pageTable[pageNum].valBit = valid;
    }
    //if there is no free space, used the LRU algorithm to replace
    else 
    {
      //printf("(3) "); 
      //remove the Least-Recently-Used page from the frames
      first = head->next;
      last = tail->pre;

      pageTable[first->pageNumber].valBit = invalid;
      pageTable[pageNum].valBit = valid;

      if(first->next != tail)
      {

        deleteFrame(first);
        insertFrame(last,pageNum, -1);
      }
      else
      {
        first->pageNumber = pageNum;
      }

      //printf("%d(d) ", first->pageNumber);
      faultCount++;
    }
  }
  return faultCount;
}  

//LFU algorithm
int execLFU(FILE* fp)
{
  pageInfoEntry pageTable[MAX_PAGE];

  char* line = NULL;
  size_t len = 0;
  int pageNum;
  int faultCount = 0;
  int i;

  for(i = 0; i < MAX_PAGE; i++)
  {
    pageTable[i].valBit = invalid;
  }

  if(getline(&line, &len, fp) == -1)
  {
    printf("Empty file!\n");
    exit(EXIT_FAILURE);
  }

  pageNum = atoi(line);
  pageTable[pageNum].valBit = valid;

  frameEntry* head = createFrame(-1, -1);
  frameEntry* tail = createFrame(-1, -1);
  frameEntry* temp = createFrame(pageNum, 1);
  head->next = temp;
  temp->pre = head;
  temp->next = tail;
  tail->pre = temp;
  frameUsed++; //first frame taken

  //printf("%d ", pageNum); 

  frameEntry *first, *last;
  while ( getline(&line, &len, fp) != -1) 
  { 
    // printFrames(head);
    pageNum = atoi(line);
    //printf("%d ", pageNum); 

    if (pageTable[pageNum].valBit == valid)//if the page is valid 
    {
      //printf("(1) "); 

      //renew the page
      //last frame is the newest
      last = tail->pre;
      //start the iteration from the head(oldest)
      temp = head->next;
      while((temp->next) != NULL)
      {
        //if found the page in the frame
        if((temp->pageNumber) == pageNum)
        {
          //remove it from current position
          int frq = (temp->frequency) + 1;
          //printf("\nnew frequency is %d\n", frq);
          if(temp->next != tail)
          {
            deleteFrame(temp);
            //add it to the end
            insertFrame(last, pageNum, frq);
          }
          else
          {
            temp->frequency = frq;
          } 
          break;
        }

        temp = temp->next;
      }
    } 
    //if there is still space for new page
    else if (maxFrame > frameUsed)
    {
      //printf("(2) "); 

      last = tail->pre;
      insertFrame(last,pageNum, 1);
      frameUsed++;
      pageTable[pageNum].valBit = valid;
    }
    //if there is no free space, used the LFU algorithm to replace
    else 
    {
      //printf("(3) "); 

      //start the iteration from the head(oldest)
      temp = head->next;
      int minFrequence = temp->frequency;
      frameEntry *LFUpage = temp;
      while((temp->next) != NULL)
      {
        //if found a less frequently used page 
        if((temp->frequency) < minFrequence)
        {
          minFrequence = temp->frequency;
          LFUpage = temp;
        }

        temp = temp->next;
      }
      //remove the Least-Frequently-Used page from the frames
      last = tail->pre;

      pageTable[LFUpage->pageNumber].valBit = invalid;
      pageTable[pageNum].valBit = valid;

      if(LFUpage->next != tail)
      {

        deleteFrame(LFUpage);
        insertFrame(last,pageNum,1);
      }
      else
      {
        LFUpage->pageNumber = pageNum;
        LFUpage->frequency = 1;
      }

      //printf("%d(d) ", LFUpage->pageNumber);
      faultCount++;
    }
  }
  return faultCount;
}  



frameEntry* createFrame(int num, int frq)
{
  frameEntry* temp;
  temp = (frameEntry*)malloc(sizeof(frameEntry));
  temp->pageNumber = num;
  temp->frequency = frq;
  temp->pre = NULL;
  temp->next = NULL;
  return temp;
}

void deleteFrame(frameEntry* curr)
{
  frameEntry* currFrame = curr;
  (currFrame->pre)->next = currFrame->next;
  (currFrame->next)->pre = currFrame->pre;
}

void insertFrame(frameEntry* curr, int newPageNum, int frq)
{
  frameEntry* currFrame = curr;
  frameEntry* newFrame = createFrame(newPageNum, frq);
  frameEntry* nextFrame = currFrame->next;
  currFrame->next = newFrame;
  newFrame->pre = currFrame;
  newFrame->next = nextFrame;
  nextFrame->pre = newFrame;
}

void printFrames(frameEntry *head) {
  frameEntry* curr = head;
  while (!(curr->next == NULL)) {
    printf("\n%d %d\n", (curr->next)->pageNumber, (curr->next)->frequency);
    curr = curr->next;
  }
  printf("\n");
}

//main
int main(int argc, char *argv[])
{
  if (argc != 4)
  {
    printf("Invalid arguments!\n");
    exit(EXIT_FAILURE);
  }

  //frame num
  maxFrame = atoi(argv[1]);

  if (maxFrame <= 0)
  {
    printf("Frame Num must be postive!\n");
    exit(EXIT_FAILURE);
  }

  //trace file
  FILE* fp = fopen(argv[2], "r");
  if (fp == NULL)
  {
    printf("Cannot open the file!\n");
    exit(EXIT_FAILURE);
  }
  //char* references[SIZE] = NULL;
  //size_t len = 0;
  //while ( getline(&references, &len, fp) != -1) 
  //{ printf("%s", line); }


  int faultsNum;
  //algorithm
  char* algo = argv[3];
  if (strcmp(algo, "LRU") == 0) 
  {
    faultsNum = execLRU(fp);
  }
  else if (strcmp(algo, "LFU") == 0) 
  {
    faultsNum = execLFU(fp);
  }
  else
  {
    printf("Invalid algorithm!\n");
    exit(EXIT_FAILURE);
  }

  fclose(fp);

  printf("There are %d page faults encountered.\n", faultsNum);

}

