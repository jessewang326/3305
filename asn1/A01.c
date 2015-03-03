#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h> 
#include <unistd.h>
#include <string.h>
#include <signal.h>


#define QUIT "quit"
#define HISTORY "history"
#define StringBuffer 200
#define SpaceSymbol " "
#define PipeSymbol "|"
#define MaxNumCommands 10
#define PATH "/usr/bin/asd"
#define MaxHistory 10

typedef enum {false=0, true} bool;

bool IsQuit (char *S);
bool IsHistory (char *S);
void PrepProgArray (char *S, char **prog_argv);
void PrepProgArray (char *Proc, char **prog_argv);
void PipeCleaner (char *line, char **Procs);
void NoPipe (char **Procs);
void MultiPipe (char **Procs, int i);
void PrintHistory (char CommandHistory[MaxHistory][StringBuffer], int j);

int main(int argc, char *argv[]) {
    int pid;
    int i, j=0;
    int child_status, c;
    char line[StringBuffer];
    char *Procs[MaxNumCommands+1];
    char CommandHistory[MaxHistory][StringBuffer];
    int A[10][10];
    
    while (true) {
        printf("pvrbik>");
        gets(line);
        
        if (IsQuit(line)) {
            printf("Goodbye\n"); 
            return 0;
        } 
        
        strcpy( CommandHistory[j % MaxHistory], line );
        j=(j+1) % MaxHistory; /* no % means term will crash when j > MaxInt */
        
        if (IsHistory(line)) { 
            PrintHistory(CommandHistory,j); 
            continue; 
        }
        
        if ((pid = fork())<0) perror("fork()");
        if (pid > 0) { /* Parent process */
            wait(NULL);
        } else { /*child process */
            PipeCleaner (line,Procs);
            
            if (Procs[1]==NULL) {
                NoPipe(Procs);
            } else {
                MultiPipe(Procs, 0);
            }
            
            printf("Fatal error\n");
            return 0;
            
        }
    }
    
    return 0;
}

void PrintHistory (char CommandHistory[MaxHistory][StringBuffer], int j) {
    int i;
    for (i=0; i<MaxHistory; i++) {
        printf("[%d]  %s\n",MaxHistory-i-1,CommandHistory[(j+i)%MaxHistory]); 
    }
}

void NoPipe (char **Procs) {
    char *args[MaxNumCommands+1];
    
    PrepProgArray(Procs[0],args);
    execvp(args[0],args);
    printf("%s:  command not found\n",Procs[0]);
    exit(0);
    
}

void MultiPipe (char **Procs, int i) {
    int child_status;
    int fds[2];
    int pid;
    char *args[MaxNumCommands+1];
    
    /* attempt to create a pipe */
    if (pipe(fds)<0) {
        perror("Fatal Error");
        exit(1);
    }
    
    /* create another process */
    if ((pid = fork())<0) {
        perror("Problem forking");
        exit(1);
    } else if (pid>0) {/* parent process */
        close(fds[1]);


        /* close stdin and reconnect to the reading end of the pipe */
        if (dup2(fds[0],STDIN_FILENO) < 0) {
            perror("can't dup");
            exit(1);
        }
        

        /* reading will block if proc[i-1] hasn't written */
        if (Procs[i+2]==NULL) {
            PrepProgArray(Procs[i+1],args);
            execvp(args[0],args);
            printf("%s:  command not found\n",Procs[0]);
            exit(0);
        } else {
            MultiPipe(Procs, i+1);
        }
                
    } else {/* child process */
        /* executes command at proc[i] */
        close(fds[0]);
        /* close stdout and reconnect to the writing end of the pipe */
        if ( dup2(fds[1],STDOUT_FILENO)<0) {
            perror("can't dup");
            exit(1);
        }
        
        PrepProgArray(Procs[i],args);
        
        execvp(args[0],args);
        printf("%s:  command not found\n",args[0]);
        exit(0);
    }
}

void PrepProgArray (char *Proc, char **prog_argv) {
    int i;
    /*Parse " "*/
    prog_argv[0] = strtok( Proc, SpaceSymbol );
    printf("prog_argv[0],%s\n", prog_argv[i]);
    
    for (i=1; prog_argv[i-1]!=NULL; i++) {
        /*next word*/
        prog_argv[i] = strtok(NULL,SpaceSymbol);
        printf("prog_argv[%d],%s\n", i, prog_argv[i]);
    } 
}

void PipeCleaner (char *line, char **Procs) {
    int i;
    /*Parse "|"*/
    Procs[0] = strtok( line, PipeSymbol );
    printf("Procs[0], %s\n",  Procs[0]);
    
    for (i=1; Procs[i-1]!=NULL; i++) {
        /*next program (i.e. prog1 | prog2 | ... )*/
        Procs[i] = strtok(NULL, PipeSymbol ); 
        printf("Procs[%d], %s\n", i, Procs[i]);
    }
}

bool IsQuit (char *S) {
    if (strcmp(S,QUIT)==0) return true;
    return false;
}

bool IsHistory (char *S) {
    if (strcmp(S,HISTORY)==0) return true;
    return false;
}
