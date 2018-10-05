// parsnip.c

#include "parsnip.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct Node{
	token* tok;
	struct Node* next;
	struct Node* prev;
} Node;

Node* head; // global variable - pointer to head node.


Node* createNewNode(token* tok){
	Node* newNode = (Node*)malloc(sizeof(Node));
	newNode->tok = tok;
	newNode->next = NULL;
	newNode->prev = NULL;
	return newNode;
}

// insert a new node at head position
void insertAtHead(token* tok){
	Node* newNode = createNewNode(tok);

	// if head doesn't exist, simply set head to newNode
	if (head == NULL){
		head = newNode;
		return;
	}
	// else bump the head to the next node of the newNode
	else {
		head->prev = newNode;
		newNode->next = head;
		head = newNode;
		return;
	}
}

void insertAtTail(token* tok){
	Node* newNode = createNewNode(tok);
	Node* temp = head;
	// if head is null, then head is tail
	if (head == NULL){
		head = newNode;
		return;
	}
	// traverse through linked list until we come across a 
	// node without a next node, which should be the tail.
	while (temp->next != NULL){
		temp = temp->next;
	}
	temp->next = newNode;
	newNode->prev = temp;
	return;
}

void printNodes(){
	Node* temp = head;
	while(temp != NULL){
		printf("%d: %s, ", temp->tok->type, temp->tok->text);
		temp = temp->next;
	}
	printf("\n");
}


//frees all nodes in linked list from head to tail
void freeNodes() {
	Node* temp = head;
	while (temp != NULL){
		Node* next = temp->next;
		free(temp);
		temp = next;
	}
}

// takes list of tokens, parses them, and returns rearranged token list
// rearranged order: ARG, LOCAL, redir, subcommands
token * parse(token *list){
// 	token* argList = malloc(sizeof(token*)*16);
// 	token* redOpList = malloc(sizeof(token*)*16);
// 	int i = 0;
// 	int argListSize = 0;
// 	int redOpListSize = 0;
// 	while(list[i].type != NONE){
// 		if (RED_OP(list[i].type)){

// 			redOpList[0]=list[i]
// 			redOpListSize++;
// 		}
// 		else{
// 			argList[argListSize]=createNewToken(list[i].type, list[i].type);
// 		}
// 	}
}

token* createNewToken(int type, char* text){
	token* tok = (token*)malloc(sizeof(token));
	tok->type = type;
	if (text != NULL){
		tok->text = (char*)malloc(sizeof(char)*6);
		strcpy(tok->text, text);
	}
	else{
		tok->text = NULL;
	}
	return tok;
}

void printTokenList(token* list){
	int i = 0;
	while(list[i].type != NONE){
		if (list[i].text != NULL){
			printf("type: %d text: %s\t", list[i].type, list[i].text);
		}
		else{
			printf("type: %d text: %s\t", list[i].type, "NULL");
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

// Free NONE-terminated array of tokens TOK and return NULL.
// token *freeList (token *tok)
// {
//     for (int i = 0; tok[i].type != NONE;  i++) {
// 	if (tok[i].text)
// 	    free (tok[i].text);
// 	tok[i].type = NONE;
// 	tok[i].text = NULL;
//     }
//     free (tok);
//     return NULL;
// }

// int main(int argc, char const *argv[])
// {
//     token *list;

//     char *line = NULL;
//     size_t nLine = 0;

// 	printf ("(_)$ ");

// 	fflush (stdout);

// 	getline (&line,&nLine, stdin); // Read line

// 	list = tokenize (line);
// 	printTokenList(list);

// 	list = freeList (list);
// 	printf ("\n");
// 	free (line);
// 	return 0;

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
// 	return 0;
// }