// parsnip.c
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "parsnip.h"

char* concat(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1) + strlen(s2) + 1); // +1 for the null-terminator
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

// token* createNewToken(int type, char* text){
// 	token* tok = malloc(sizeof(token));
// 	tok->type = type;
// 	if (text != NULL){
// 		tok->text = (char*)malloc(sizeof(char)*64);
// 		strcpy(tok.text, text);
// 	}
// 	else{
// 		tok->text = NULL;
// 	}
// 	return tok;
// }

void printTokenList(token* list){
	int i = 0;
	while(list[i].type != NONE){
		if (list[i].text != NULL){
			printf("type: %d text: %s\n", list[i].type, list[i].text);
		}
		else{
			printf("type: %d text: %s\n", list[i].type, "NULL");
		}
		i++;
	}
	printf("\n");
}

void freeToken(token* tok){
	free(tok->text);
	free(tok);
	return;
}

//Free NONE-terminated array of tokens TOK and return NULL.
token *freeList (token *tok)
{
    for (int i = 0; tok[i].type != NONE;  i++) {
		if (tok[i].text)
		    free (tok[i].text);
		tok[i].type = NONE;
		tok[i].text = NULL;
    }
    free (tok);
    return NULL;
}

token endToken = {NONE, NULL};

// takes list of tokens, parses them, and returns rearranged token list
// rearranged order: ARG, LOCAL, redir, subcommands
// token * parse(token *list){
// 	token* argList = malloc(sizeof(token*)*16);
// 	token* redOpList = malloc(sizeof(token)*16);
// 	int i = 0;
// 	int argListSize = 0;
// 	int redOpListSize = 0;
// 	//char* tempStr = malloc(sizeof(char)*64);
// 	while(list[i].type != NONE){
// 		if (RED_OP(list[i].type)){
// 			printf("Is red_op\n");
// 			if (list[i+1].type != NONE && list[i+1].text != NULL){
// 				// make sure subsequent token is a simple (file)
// 				if (list[i+1].type == SIMPLE){
// 					printf("Is a valid red_op simple pair\n");
// 					printf("%s\n", list[i+1].text);
// 					redOpList[redOpListSize] = createNewToken(list[i].type, list[i+1].text);
// 					redOpListSize++;
// 					i++; //skip subsequent token in next read
// 				}
// 			}
// 			else
// 				fprintf(stderr, "not valid red_op simple pair error\n");
// 		}
// 		else if(list[i].type == SIMPLE){
// 			printf("Is a simple\n");
// 			argList[argListSize] = createNewToken(ARG, list[i].text);
// 			argListSize++;
// 		}
// 		else{
// 			printf("not a red_op\n");
// 			// argList[argListSize]=createNewToken(list[i].type, list[i].type);
// 		}
// 		i++;
// 	}

// 	token *finList = malloc(sizeof(token)*32);
// 	int finListSize = 0;
// 	for (int j = 0; j < argListSize; ++j)
// 	{
// 		finList[finListSize] = argList[j];
// 		finListSize++;
// 	}
// 	for (int j = 0; j < redOpListSize; ++j)
// 	{
// 		finList[finListSize] = redOpList[j];
// 		finListSize++;
// 	}
// 	finList[finListSize] = endToken;
// 	freeList(redOpList);
// 	freeList(argList);
// 	return finList;
// }

int main(int argc, char const *argv[])
{
    token *list;
    //token *cmd;
    char *line = NULL;
    size_t nLine = 0;

	printf ("(_)$ ");

	fflush (stdout);

	getline (&line,&nLine, stdin); // Read line

	list = tokenize (line);
	printTokenList(list);

	//cmd = parse(list);
	//printTokenList(cmd);

	freeList (list);
	//freeList (cmd);
	printf ("\n");
	free (line);
	return 0;

	// token* tok1 = createNewToken(1, "one");
	// token* tok2 = createNewToken(2, "two");
	// token* tok3 = createNewToken(3, "thre");
	// token* tok4 = createNewToken(14, NULL);
	// token* list = malloc(sizeof(token)*4);
	// list[0] = *tok1;
	// list[1] = *tok2;
	// list[2] = *tok3;
	// list[3] = *tok4;

	//printf("%d\n", NONE);
	

	// // Calling an Insert and printing list both in forward as well as reverse direction. 
	// insertAtTail(tok1); printNodes();
	// insertAtTail(tok2); printNodes();
	// insertAtHead(tok3); printNodes();
	// insertAtTail(tok4); printNodes();

	// freeNodes();

	// freeToken(tok1);
	// freeToken(tok2);
	// freeToken(tok3);
	// freeToken(tok4);
	return 0;
}