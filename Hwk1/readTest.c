#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/stat.h> 
#include <unistd.h>

int main(int argc, char const *argv[])
{
	char ch, archive_file[80], target_file[80];
	FILE *archive, *target;

	printf("Enter name archive file\n");
	scanf("%s", archive_file);

	archive = fopen(archive_file, "a+"); // open archive for reading and writing

	// if archive_file doesn't exist, we must create a new blank archive file with name archive_file
	if( archive == NULL )
	{
		printf("archive file doesn't exist, creating %s\n", archive_file);
		archive = fopen(archive_file, "w");

		if ( archive ==  NULL ){
			printf("No disk space??\n");
			exit(EXIT_FAILURE);
		}
	}

	if (archive != NULL){
		printf("%s is there\n", archive_file);
	}


	printf("Enter name of target file\n");
	scanf("%s",target_file);

	// open NAME file to read
	target = fopen(target_file, "r");

	struct stat file_info;

	lstat(target_file, &file_info);

	printf("file_name length: %ld\n", strlen(target_file));	
    printf("File size: %lld bytes\n", (long long) file_info.st_size);


	if (target == NULL){
		fclose(archive);
		printf("No NAME file found\n");
		exit(EXIT_FAILURE);
	}

	char target_length_buffer[32];
	snprintf(target_length_buffer, 32, "%ld", strlen(target_file));
	printf("%s\n", target_length_buffer);

	char file_size_buffer[32];
	snprintf(file_size_buffer, 32, "%lld", (long long) file_info.st_size);
	printf("%s\n", file_size_buffer);

	fputs(target_length_buffer, archive);
	fputs(" | ", archive);
	fputs(target_file, archive);
	fputs(" | ", archive);
	fputs(file_size_buffer, archive);
	fputc('\n', archive);
	// get header? "put" header into archive
	// 13 | filetest.test | 100000
	while ( (ch = fgetc(target)) != EOF){
		fputc(ch, archive);
	}
	//fputc('\n', archive);

	printf("Target file archived successfully\n");

	fclose(archive);
	fclose(target);
	return 0;
}