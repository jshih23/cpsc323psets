#include <stdio.h>
#include <stdlib.h>

int main(int argc, char const *argv[])
{
	char ch, source_file[20], target_file[20];
	FILE *source, *target;

	printf("Enter name of file to copy from\n");
	scanf("%s", source_file);

	source = fopen(source_file, "r");


	if( source == NULL )
	{
		printf("Press any key to exit...\n");
		exit(EXIT_FAILURE);
	}

	printf("Enter name of target file\n");
	scanf("%s", target_file);

	target = fopen(target_file, "w");

	if (target == NULL){
		fclose(source);
		printf("Press any key to exit...\n");
		exit(EXIT_FAILURE);
	}

	while ( (ch = fgetc(source)) != EOF){
		fputc(ch, target);
	}

	printf("File copied successfully\n");

	fclose(source);
	fclose(target);
	return 0;
}