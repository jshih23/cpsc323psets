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

token * parse(token *list){

}

token* createNewToken(int type, char* text){
	token* tok = (token*)malloc(sizeof(token));
	tok->type = type;
	tok->text = (char*)malloc(sizeof(char)*6);
	strcpy(tok->text, text);
	return tok;
}

void freeToken(token* tok){
	free(tok->text);
	free(tok);
	return;
}

int main(int argc, char const *argv[])
{

	token* tok1 = createNewToken(1, "one");
	token* tok2 = createNewToken(2, "two");
	token* tok3 = createNewToken(3, "thre");
	token* tok4 = createNewToken(4, "four");

	// Calling an Insert and printing list both in forward as well as reverse direction. 
	insertAtTail(tok1); printNodes();
	insertAtTail(tok2); printNodes();
	insertAtHead(tok3); printNodes();
	insertAtTail(tok4); printNodes();

	freeNodes();

	freeToken(tok1);
	freeToken(tok2);
	freeToken(tok3);
	freeToken(tok4);
	return 0;
}