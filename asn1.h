#ifndef _ASN1_H
#define _ASN1_H

#define CMD_SIZE 128 
#define HISTORY_MAX 150  
#define TOKEN_MAX 16 

void history(char cmdHistory[HISTORY_MAX][CMD_SIZE], int index, int num);
int make_tokenlist(char *buf, char *tokens[]);
void simpleCmd(char **tokens);

#endif /* _ASN1_H */
