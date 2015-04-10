#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h> 
#include <unistd.h>
#include <string.h>
#include <signal.h>

#define MaxPageNum 1023
#define StringBuffer 200
#define SpaceSymbol " "
#define LRU  0
#define FIFO 1
//PAUL VRBIK//

/*Globals and Definitions*/
int MaxNumFrames;
int Method;

typedef enum {false=0, true} bool;
typedef enum {valid=0, invalid} Validity;
typedef unsigned int page;

struct frame {
    page Page;
    struct frame *next;
    struct frame *prev;
};

typedef struct frame FrameType;

/* MAIN SUB-ROUTINES */
FILE* ParseConfigFile ();
int DoFIFO (FILE *TraceFile);
int DoLRU (FILE *TraceFile);

/* Link list stuff */
FrameType* NewFrame (page x);
FrameType* RemoveFrame (page x, FrameType *L );
FrameType* InsertFrame (page x, FrameType *Tail);
void PrintList (FrameType *head);
void PrintListBackwards (FrameType *tail);

int main(int argc, char *argv[]) {
    
    FILE* TraceFile = ParseConfigFile();
    
    int PageFaults = 0;
    
    if (Method == LRU) {
        PageFaults = DoLRU( TraceFile );
    } else if (Method == FIFO) {
        PageFaults = DoFIFO( TraceFile );
    } else {
        printf("Error: Unknown method\n");
        PageFaults = 0;
    };
   
    fclose( TraceFile );
    
    printf("PageFaults = %d\n",PageFaults);
    return PageFaults;
}



/* MAIN SUB-ROUTINES */
FILE* ParseConfigFile () {
    FILE *fp;
    fp = fopen( "config", "r" );
    char Line[StringBuffer];
    char *temp;
    char TraceFileName[StringBuffer];
    
    fgets(Line,StringBuffer,fp);
    MaxNumFrames = atoi( strtok( Line, SpaceSymbol ) );
    
    strcpy( TraceFileName, strtok(NULL, SpaceSymbol) );
    
    temp = strtok(NULL,SpaceSymbol);
    if ( strcmp("LRU", temp)==0 ) {
        Method = LRU;
    } else if ( strcmp("FIFO", temp)==0 ) {
        Method = FIFO;
    } else {
        printf("Error: Config file is formatted incorrectly.\n");
        printf("    -is last element is LRU or FIFO (case sensitive)?\n");
        printf("    -are there no tailing spaces or new lines in config?\n");
        Method = -1;
    }
    fclose(fp);
    
    FILE *TraceFile;
    TraceFile = fopen( TraceFileName, "r");
    return TraceFile;
}

int DoFIFO (FILE *TraceFile) {
    
    if (feof(TraceFile)) return 0; //empty trace file
    
    Validity PageTable[MaxPageNum];
    int i;
    for (i = 0; i<MaxPageNum; i++) PageTable[i] = invalid;
    
    int NumFrames=0;
    int PageFaults = 0;
    page NextPage;
    FrameType *tmp;
    
    fscanf(TraceFile, "%u", &NextPage);
    
    PageTable[NextPage] = valid;
    
    FrameType *Head = NewFrame(-1); //MakeNull
    FrameType *Tail = NewFrame(-1); //MakeNull
    
    Head -> next = NewFrame( NextPage );
    Tail -> next = (Head -> next);

    NumFrames++;
    
    while (!feof(TraceFile)) {
        fscanf(TraceFile, "%u", &NextPage);
        // case: Page is valid
        if ( PageTable[NextPage] == valid ) {
            //do nothing
        }
        // case: Page is valid and there is room 
        else if ( NumFrames < MaxNumFrames ) {
            NumFrames++;
            Tail -> next = InsertFrame( NextPage, Tail );
            
            PageTable[NextPage] = valid;
        }
        // case: Invalid Page and no room
        else { //out of room
            //Update page table
            PageFaults++;
            PageTable[ (Head -> next) -> Page ] = invalid;
            PageTable[NextPage] = valid;
            
            //Remove head
            Head -> next = (Head -> next) -> next;
            
            //Add element to end of list
            Tail -> next = InsertFrame( NextPage, Tail);
        };
    };
    
    return PageFaults;
};


int DoLRU (FILE *TraceFile) {
    if (feof(TraceFile)) return 0; //empty trace file
    
    Validity PageTable[MaxPageNum];
    int i;
    for (i = 0; i<MaxPageNum; i++) PageTable[i] = invalid;
    
    int NumFrames=0;
    int PageFaults = 0;
    page NextPage;
    FrameType *tmp;
    
    fscanf(TraceFile, "%u", &NextPage);
    
    PageTable[NextPage] = valid;
    
    FrameType *Head = NewFrame(-1); //MakeNull
    FrameType *Tail = NewFrame(-1); //MakeNull
    
    //Initialize doubly link list
    Head -> next = NewFrame( NextPage );
    (Head -> next) -> prev = Head;
    Head -> prev = NULL;
    Tail -> next = (Head -> next);
    Tail -> prev = Tail -> next;
    
    NumFrames++;
    
    while (!feof(TraceFile)) {
        fscanf(TraceFile, "%u", &NextPage);
        
        // case: Page is valid
        if ( PageTable[NextPage] == valid ) {
            //Make the page the newest
            tmp = Head -> next;
            while (true) {
                if (tmp -> next == NULL) break; //already the newest
                if (tmp -> Page == NextPage) {
                    //remove page//
                    (tmp -> prev) -> next = (tmp -> next);
                    (tmp -> next) -> prev = (tmp -> prev);
                    
                    //attach element to end of list//
                    tmp -> next = NULL;
                    tmp -> prev = Tail -> next;
                    (tmp -> prev) -> next = tmp;
                    Tail -> next = tmp;
                    Tail -> prev = tmp;
                    break;
                }
                tmp = tmp -> next; 
            } 
        }
        // case: Page is valid and there is room 
        else if ( NumFrames < MaxNumFrames ) {
            //Add element to end of list
            NumFrames++;
            Tail -> next = InsertFrame( NextPage, Tail );
            Tail -> prev = Tail -> next;
            
            PageTable[NextPage] = valid;
        }
        // case: Invalid Page and no room
        else { //out of room
            PageFaults++;
            //Update page table
            PageTable[NextPage] = valid;
            PageTable[ (Head -> next) -> Page ] = invalid;
            
            //Remove head
            Head -> next = (Head -> next) -> next;
            (Head -> next) -> prev = Head;
            
            //Add element to end of list
            Tail -> next = InsertFrame( NextPage, Tail);
            Tail -> prev = Tail -> next;
            
        };
        
    };
    
    return PageFaults;
}

//Link list stuff//
FrameType* NewFrame (page x) {
    FrameType *L;
    L = (FrameType*)malloc(sizeof(FrameType));
    L -> Page = x;
    L -> next = NULL;
    L -> prev = NULL;
    return L;
};

FrameType* InsertFrame (page x, FrameType *Tail) {
    FrameType *tmp = NewFrame(x);
    
    tmp -> prev = Tail -> next;
    (tmp -> prev) -> next =  tmp;
    
    return tmp;
}

//Debugging//
void PrintList (FrameType *head) {
    printf("--forward--\n");
    while (!(head -> next == NULL)) {
        printf("%u\n", (head->next)->Page);
        head = head -> next;
    };
    printf("\n");
}

void PrintListBackwards (FrameType *tail) {
    printf("--backward--\n");
    while (!( (tail -> prev) -> prev == NULL)) {
        printf("%u\n", (tail->prev)->Page);
        tail = tail -> prev;
    };
    printf("\n");
}
