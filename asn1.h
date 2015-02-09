#ifndef _ASN1_H
#define _ASN1_H

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/stat.h> 
#include <fcntl.h>

#define CMD_SIZE 128 
#define HISTORY_MAX 150  
#define TOKEN_MAX 16 
#define IN "<"
#define OUT ">"


typedef enum {false=0, true=1} boolean;

void history(char cmdHistory[HISTORY_MAX][CMD_SIZE], int index, int num);
int make_tokenlist(char *buf, char *tokens[]);
void simpleCmd(char **tokens);
void pipeCmd(char** noPipeCmd, int index);
void deletePipes(char *cmd, char *noPipeCmd[]); 

#endif /* _ASN1_H */
