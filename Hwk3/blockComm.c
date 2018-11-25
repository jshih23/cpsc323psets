
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char const *argv[])
{
	FILE* ifp;
	FILE* ofp;
	ifp = fopen(argv[1], "r");
	int foundEBC = 0;
	if (ifp == NULL){
		fprintf(stderr, "Can't open input file %s\n", argv[1]);
		exit(1);
	}

	ofp = fopen(argv[2], "w");

	if (ofp == NULL){
		fprintf(stderr, "Can't open outut file %s\n", argv[2]);
	}

	char line[100];
	char comm[3] = "//\0";
	char blockComm[3] = "/*\0";
	char endBlockComm[3] = "*/\0";

	while (fgets(line, 99, ifp) != NULL){

		if (strncmp(line, comm, strlen(comm)) == 0){
			// do nothing
		}
		else if (strncmp(line, blockComm, strlen(blockComm)) == 0){
			// look for end */
			while (fgets(line, 99, ifp) != NULL){
				if (strstr(line, endBlockComm) != NULL){
					foundEBC = 1;
					break;
				}
			}
			if (foundEBC == 0){
				fprintf(stderr, "no end block comment found, bad file\n");
			}
		}
		else{
			fputs(line, ofp);
		}
	}


	
	return 0;
}