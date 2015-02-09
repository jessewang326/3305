#include "asn1.h"

int main(int argc, char* argv[])
{
  char cmd[CMD_SIZE];
  char cmdHistory[HISTORY_MAX][CMD_SIZE] = {};
  char* tokens[TOKEN_MAX];
  int historyIndex = 0, tokenCount, lastIndex, inIndex = -1, outIndex = -1;
  pid_t pid;  
  int pipeFlag = 0;

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
      exit(0);
    }

    //store the command into the history if it is not "exit"
    printf("input cmd is %s\n", cmd);
    strcpy(cmdHistory[historyIndex % HISTORY_MAX], cmd);
    lastIndex = historyIndex;
    historyIndex = (historyIndex + 1) % HISTORY_MAX;
    if(strstr(cmd, "|") != NULL)
      pipeFlag = 1;

    //convert the cmd into tokens
    tokenCount = make_tokenlist(cmd, tokens);
    int i = 0;
    for(; i < tokenCount; i++)
    {
      if(strcmp(tokens[i], "<") != 0 && i < (tokenCount-1))
        inIndex = (i+1);
      if(strcmp(tokens[i], "<") != 0 && i < (tokenCount-1))
        outIndex = (i+1);
      printf("token[%d], %s\n", i, tokens[i]);
    }
    strcpy(cmd,cmdHistory[lastIndex]);
    printf("input cmd2 is %s\n", cmd);

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
    //child process  -- shell(1)
    else
    {
      if(strstr(cmd,IN) != NULL || strstr(cmd,OUT) != NULL)
      {     
        /*
        int i;
        if (strstr(cmd,IN) != NULL)
        {
          for(i = 0; strcmp(tokens[i], "<") != 0 && i < (tokenCount-1); i++)
          {
            ; 
          }
          if(i != (tokenCount-1))
          {
          */
        if(inIndex != -1)
        {
            int fd0 = open(tokens[inIndex], O_RDONLY, 0);
            dup2(fd0, STDIN_FILENO);
            close(fd0);
            int i;
            for(i = (inIndex-1);i<(tokenCount - 2);i++)
            {
              tokens[i] = tokens[i+2];
            }
            tokens[i] = NULL;
            tokens[i+1] = NULL;
        }
            /*
          }
          else
          {
            printf("no input file!\n");
          }
        }
        if (strstr(cmd,OUT) != NULL)
        {
          for(i = 0; strcmp(tokens[i], ">") != 0 && i < (tokenCount-1); i++)
          {
            printf("token[%d], %s\n",i,tokens[i]); 
          }
          if(i != (tokenCount-1))
          {
          */
          else if(outIndex != -1)
          {
            int fd1 = creat(tokens[outIndex], 0644);
            dup2(fd1, STDOUT_FILENO);
            close(fd1);
            int i;
            for(i = (outIndex-1);i<(tokenCount - 2);i++)
            {
              tokens[i] = tokens[i+2];
            }
            tokens[i] = NULL;
            tokens[i+1] = NULL;

          }
          printf("token 0 is %s\n", tokens[0]);
            /*
          }
          else
          {
            printf("no output file!\n");
          }
          */
      }

      char *noPipeCmd[TOKEN_MAX] = {};
      deletePipes(cmd,noPipeCmd);

      //if there is no pipe("|")
      if(!pipeFlag) 
      {
        simpleCmd(tokens);
      }
      //if there is(are) pipe(s)
      else
      {
        //printf("multi\n");
        pipeCmd(noPipeCmd, 0);
      }

      printf("Fatal error\n");
      return 0;
    }
  }while(true);
}



/* To print out the cmd history, 
 * @index for the current cmd pointer, 
 * @num for the # of commands to print */
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
  printf("enter simple");
  execvp(tokens[0], tokens);
  printf("%s: command not found\n", tokens[0]);
  exit(0);
}

void pipeCmd(char** noPipeCmd, int index)
{

  int fds[2];
  pid_t pid;
  char* tokens[TOKEN_MAX]; 

  /* attempt to create a pipe */ 
  if(pipe(fds)<0) 
  {
    perror("Pipe Error"); 
    exit(1);
  }

  /* create another process */
  pid = fork(); 
  if(pid<0) 
  {
    perror("Forking Error");
    exit(1);
  } 
  else if(pid>0) 
  {
    /* parent process */ 
    //close stdout
    close(fds[1]);

    /* close stdin, reconnect to the reading end of the pipe */ 
    if(dup2(fds[0],STDIN_FILENO)<0) 
    {
      perror("can't dup");  
      exit(1);
    }

    //check the # of cmd left
    if (noPipeCmd[index+2]==NULL) 
    {
      //the parent process runs the last cmd
      make_tokenlist(noPipeCmd[index+1],tokens);
      execvp(tokens[0],tokens);
      printf("%s:  command not found\n",tokens[0]);
      exit(1);
    } 
    else 
    {
      //if there are more than two, fork new process for the cmds
      pipeCmd(noPipeCmd, index+1);
    }
  }
  else 
  {
    /* child process */
    //close stdin
    close(fds[0]);


    /* close stdout, reconnect to the writing end of the pipe */ 
    if(dup2(fds[1],STDOUT_FILENO) < 0) 
    {
      perror("can't dup");  
      exit(1);
    }

    //runs the cmd
    make_tokenlist(noPipeCmd[index],tokens);
    execvp(tokens[0],tokens);
    printf("%s: command not found\n", tokens[0]);
    exit(1);
  }
}

/* to remove the pipes in the command */
void deletePipes(char *cmd, char *noPipeCmd[])
{

  int i = 0;
  noPipeCmd[i] = strtok(cmd, "|");
  printf("noPipeCmd[0], %s\n", noPipeCmd[0]);

  do
  {
    i++;
    noPipeCmd[i] = strtok(NULL, "|");
    printf("noPipeCmd[%d], %s\n", i, noPipeCmd[i]);
  }while(noPipeCmd[i] != NULL);
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


