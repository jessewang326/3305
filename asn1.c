#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h> 
#include <unistd.h>
#include <string.h>
#include "asn1.h"


//#define CMD_SIZE 128
//#define HISTORY_MAX 150
//#define TOKEN_MAX 16


typedef enum {false=0, true=1} boolean; 

int main(int argc, char* argv[])
{
  char cmd[CMD_SIZE];
  char cmdHistory[HISTORY_MAX][CMD_SIZE] = {};
  char* tokens[TOKEN_MAX];
  int historyIndex = 0, tokenCount;
  pid_t pid;  

  do{
    printf("jiaxi>");
    if (fgets(cmd,CMD_SIZE,stdin) != NULL) 
    {
      strtok(cmd, "\n");
    }
    else 
      printf("no command!\n");

    //  for (i = 0; i < n; i++)
    //    printf("extracted token is %s\n", tokens[i]);

    //build-in command -- exit
    if(strcmp(cmd, "exit") == 0)
    {
      printf("000\n"); 
      exit(1);
    }

    //store the command into the history if it is not "exit"
    printf("input cmd is %s\n", cmd);
    strcpy(cmdHistory[historyIndex % HISTORY_MAX], cmd);
    historyIndex = (historyIndex + 1) % HISTORY_MAX;

    //convert the cmd into tokens
    tokenCount = make_tokenlist(cmd, tokens);

    //built-in command -- history
    if(strcmp(tokens[0], "history") == 0)
    {
      //outSize is 10 in default
      int outSize = 10;

      //if there is a parameter
      if(tokenCount > 1)
        outSize =  atoi(tokens[1]);

      history(cmdHistory, historyIndex, outSize);
      continue;
    }


    //exec commands

    //fork
    if((pid = fork())<0) 
      perror("Problem forking");

    //parent process -- shell(0)
    if(pid > 0)
      wait(NULL);
    //child process
    else
    {
      //if there is no pipe("|")
      if(strstr(cmd, "|") == NULL) 
      {
        simpleCmd(tokens);
      }
      else
      {
        printf("multi\n");
        //PipeCleaner (line,Procs);
        //MultiPipe(Procs, 0);
      }

      printf("Fatal error\n");
      return 0;
    }
  }while(true);
}



void history(char cmdHistory[HISTORY_MAX][CMD_SIZE], int index, int num)
{
  printf("index is %d, outsize is %d\n", index, num);

  int count,a;

  if(num > HISTORY_MAX)
    num = HISTORY_MAX;

  if(num > index)
  {
    num -= index;
    index--;
    count = index;
    if(strcmp(cmdHistory[HISTORY_MAX-1],"") != 0)
    {
      count = num;
      int a;
      num--;
      for(a = HISTORY_MAX - 1 - num;num >= 0;count--)
      {
        printf("%d %s\n", count, cmdHistory[a++]); 
      }
    }

    for(a = 0;index >= 0; index--, count--)
    {
      printf("%d %s\n", count, cmdHistory[a++]); 
    }
  }
  else
  {
    count = num-1;
    num--;
    index--;
    for(a = index - num;num >= 0; num--, count--)
    {
      printf("%d %s\n", count, cmdHistory[a++]); 
    }
  }
}

void simpleCmd(char **tokens)
{
  execvp(tokens[0], tokens);
  printf("%s: command not found\n", tokens[0]);
  exit(0);
}

/*
   This function takes as input the following:
buf: This represents the string for which the tokens are to be determined
for

tokens: This represents the array that the found tokens are tbe put into

The function strtok() is used to find the tokens.  The delimiter used
to distinguish tokens is a space
*/
int make_tokenlist(char *buf, char *tokens[])
{

  char input_line[CMD_SIZE];
  char *line;
  int i,n;

  i = 0;

  line =   buf;
  tokens[i] = strtok(line, " ");
  do  {
    i++;
    line = NULL;
    tokens[i] = strtok(line, " ");
  } while(tokens[i] != NULL);

  return i;
}


