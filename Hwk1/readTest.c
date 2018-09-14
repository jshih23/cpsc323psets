#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#define _GNU_SOURCE
#include <sys/stat.h> 
#include <unistd.h>

int isDirectory(const char *path);
int isReg(const char *path);
int readTarget(const char *path);

FILE *archive, *target;
char ch;


int main(int argc, char const *argv[])
{
	char archive_file[80], target_file[80], temp_file[80];


	printf("Enter name archive file\n");
	scanf("%s", archive_file);

	archive = fopen(archive_file, "r"); // open archive for reading only

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

	if (isDirectory(target_file) != 0){
		printf("%s is directory\n", target_file);
		fclose(archive);
		return 0;
	}
	else if (isReg(target_file) != 0){
		printf("%s is a file\n", target_file);
		// open NAME file to read
		readTarget(target_file);
		return 0;
	}
	else
		printf("aint no file yo\n");
	
}

int isDirectory(const char *path) {
	struct stat statbuf;
	if (lstat(path, &statbuf) != 0)
		return 0;
	return S_ISDIR(statbuf.st_mode);
}

int isReg(const char *path) {
	struct stat statbuf;
	if (lstat(path, &statbuf) != 0)
		return 0;
	return S_ISREG(statbuf.st_mode);
}

int readTarget(const char *path){
	target = fopen(path, "r+");
	struct stat file_info;

	lstat(path, &file_info);

	if (target == NULL){
		fclose(archive);
		printf("No NAME file found\n");
		exit(EXIT_FAILURE);
	}

	char target_length_buffer[32];
	snprintf(target_length_buffer, 32, "%ld", strlen(path));
	//printf("%s\n", target_length_buffer);

	char file_size_buffer[32];
	snprintf(file_size_buffer, 32, "%lld", (long long) file_info.st_size);
	//printf("%s\n", file_size_buffer);

	// read through file and search for path in a header
	char temp[1024];
	char buffer[512];
	snprintf(buffer, sizeof(buffer), "|%s\n", path);
	char name[512];
	int len, size;

	while (fgets(temp, 1024, archive) != NULL){
		if (strstr(temp, buffer) != NULL){
			printf("it's in there\n");
			sscanf( temp, "%d|%s", &len, name);
			printf("len: %d, name: %s\n", len, name);

		}
	}

	// fputs(target_length_buffer, archive);
	// fputc("|", archive);
	// fputs(path, archive);
	// fputc("\n", archive);
	// fputs(file_size_buffer, archive);
	// fputs('|\n', archive);
	// // get header? "put" header into archive
	// // 13 | filetest.test | 100000
	// while ( (ch = fgetc(target)) != EOF){
	// 	fputc(ch, archive);
	// }
	// //fputc('\n', archive);

	// printf("Target file archived successfully\n");

	fclose(archive);
	fclose(target);
	return 0;
}