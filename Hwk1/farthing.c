#include <stdio.h>
#include <string.h>
#include <stdlib.h>	

/* global variable declaration */
int NAMES_SIZE = 8;

int main (int argc, char *argv[]) {
	int i=0;

	if (argc < 3){
		fprintf(stderr, "Invalid Arguments: Too few arguments: %d\n", argc);
		return 1;
	}
	printf("\ncmdline args count=%d", argc);
	for (i=1; i< argc; i++) {
		printf("\narg%d=%s", i, argv[i]);
	}
	printf("\n");

	char * archive_file_name = argv[2];

	printf("archive_file_name = %s\n", argv[2]);
	int names_count = 0;
	// is it okay to use global variables like this? Also, is it okay to realloc like this? 
	char ** names;
	names = malloc(NAMES_SIZE * sizeof(char*));
	for (int i = 3, j = 0; i < argc; ++i, ++j)
	{
		if (j >= NAMES_SIZE){
			NAMES_SIZE *= 2;
			// realloc to resize array
			names = realloc(names, NAMES_SIZE * sizeof(char*));
		}
		int len;
		len = strlen(argv[i]);
		names[j] = malloc(len * sizeof(char));
		strcpy(names[j], argv[i]);
		names_count += 1;
	}

	for (int i = 0; i < names_count; ++i)
	{
		printf("%s\n", names[i]);
	}

	switch(*argv[1]){
		case 'r' :
		printf("\nreplace\n");
		break;

		case 'x' :
		printf("\nexecute\n");
		break;

		case 'd' :
		printf("\ndelete\n");
		break;

		case 't' :
		printf("\nlist\n");
		break;

		default :
		fprintf(stderr, "Invalid Key: %s\n", argv[1]);

	}
	printf("names_count = %d\n", names_count);
	for (int i = 0; i < names_count; ++i)
	{
		printf("%s freed\n", names[i]);
		free(names[i]);
	}
	free(names);
	return 0;
 }