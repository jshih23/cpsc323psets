#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#define _GNU_SOURCE
#include <sys/stat.h> 
#include <unistd.h>
#include <limits.h>
#include <stdbool.h>

int isDirectory(const char *path);
int isReg(const char *path);
int getFileHeader(const char *path, char *header, size_t header_size);

FILE *archive_file, *temp_file, *target_file;
char ch;


int main(int argc, char const *argv[])
{
	char archive_name[PATH_MAX], target_name[PATH_MAX], temp_name[PATH_MAX];
	int file_descriptor = -1;
	memset(archive_name, 0, sizeof(archive_name));
	memset(target_name, 0, sizeof(target_name));
	memset(temp_name, 0, sizeof(temp_name));

	// open archive file for reading, create if doesn't exist?
	printf("Enter name archive file\n");
	scanf("%s", archive_name);
	archive_file = fopen(archive_name, "r"); 

	if( archive_file == NULL )
	{
		printf("[N]  Archive file doesn't exist, creating %s\n", archive_name);
		archive_file = fopen(archive_name, "w");

		if ( archive_file ==  NULL ){
			printf("No disk space??\n");
			exit(EXIT_FAILURE);
		}
	}
	else
		printf("[N] Archive file found\n");

	printf("Enter name target file\n");
	scanf("%s", target_name);

	if (isDirectory(target_name) != 0){
		printf("%s is directory\n", target_name);
		fclose(archive_file);
		return 0;
	}
	else if (isReg(target_name) != 0){
		printf("%s is a file\n", target_name);
		// open NAME file to read

		// get header of the file
		char header[512];
		memset(header, 0, sizeof(header));
		size_t header_size = sizeof(header);
		// must pass header_size in because header decays to a pointer of 
		// size 8 when passed to function. Calling sizeof(header) in func
		// gives you 8 as opposed to 512.
		getFileHeader(target_name, header, header_size); 

		// create temporary file
		strcpy(temp_name,"temparchive-XXXXXX");
		file_descriptor = mkstemp(temp_name);

		if (file_descriptor < 1){
			printf("\n Creation of temp file failed");
			return 1;
		}
		else{
			 printf("\n Temporary file [%s] created\n", temp_name);
		}
		// open temporary file with pointer temp_file
		temp_file = fdopen(file_descriptor, "w");
		
		// read through archive file, while writing to temp
		char temp[1024];
		char name_buffer[512];
		snprintf(name_buffer, sizeof(name_buffer), "| %s |", target_name);
		char name[512];
		int len, size;

		target_file = fopen(target_name, "r");
		bool replaced = false;

		while (fgets(temp, 1024, archive_file) != NULL){
			// if the line im reading contains name_buffer
			// we skip -size- characters in archive, and then we 
			// put header in temp and copy contents of target to temp
			if (strstr(temp, name_buffer) != NULL){
				printf("it's in there\n");
				printf("%s\n", temp);
				sscanf( temp, "%d | %s | %d", &len, name, &size);
				printf("name: %s, length: %d\n", name, len);
				printf("skipping %d chars\n", size);
				for(int i = 0; i < size; i++){
					printf("skipped %c\n", fgetc(archive_file));
				}
				// move stream pointer past the file in archive, and then read
				// from target into temp
				fputs(header, temp_file);
				while( (ch = fgetc(target_file)) != EOF){
					fputc(ch, temp_file);
				}
				replaced = true;
			}
			// otherwise we just copy line from archive into temp
			else{
				fputs(temp, temp_file);
			}
		}
		// if we get to the end of the archive, and we didn't find
		// the file, we just add the file to the end of temp
		if (!replaced){
			fputs(header, temp_file);
			while( (ch = fgetc(target_file)) != EOF){
				fputc(ch, temp_file);
			}
		}

		// after temp is written, delete original and rename temp to original
		char del[] = "deleteme.temp";
		rename(archive_name, del);
		rename(temp_name, archive_name);
		unlink(del);
	}
	else
		printf("aint no file yo\n");

	// close files
	fclose(temp_file);
	fclose(target_file);
	fclose(archive_file);
	//fclose(temp_file);
	return 0;
}

int getFileHeader(const char *path, char *header, size_t header_size){
	// gather file name length and file size (bytes), and store them as char arrays
	struct stat file_info;
	lstat(path, &file_info);

	char path_length[32];
	memset(path_length, 0, sizeof(path_length));
	snprintf(path_length, 32, "%ld", strlen(path));

	char file_size_buf[32];
	memset(file_size_buf, 0, sizeof(file_size_buf));
	snprintf(file_size_buf, 32, "%ld", file_info.st_size);

	snprintf(header, header_size, "%s | %s | %s\n",path_length, path, file_size_buf);
	return 0;
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