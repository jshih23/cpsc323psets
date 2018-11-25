#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int main(int argc, char const *argv[])
{

	char *line = NULL;
	size_t nLine = 0;
	// char *lin= malloc(sizeof(char)*32);
	char lin[32];
	for (;;)
	{
		printf("Hello.");
		fflush(stdout);

		int a;
		fgets(lin, 32, stdin);
		printf("%s\n", lin);
		// if ( getline (&line,&nLine, stdin) <= 0)
		// 	break;
	}
	return 0;
}
