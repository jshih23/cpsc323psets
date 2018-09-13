#include <stdio.h>
#include <string.h>
#include <stdlib.h>	
#include <sys/types.h>
#define _GNU_SOURCE
#include <sys/stat.h> 
#include <unistd.h>
#include <limits.h>
#include <stdbool.h>
#include <dirent.h>

int replace(char *archive_name, char **names, int names_count);
int delete(char *archive_name, char **names, int names_count);
int isDirectory(const char *path);
int isReg(const char *path);
void listFilesRecursively(char *basePath, char **dir_files, int *num_files);
int getFileNames(char **file_names, char **names, int names_count);
int getFileHeader(const char *path, char *header, size_t header_size);

/* global variable declaration */
int NAMES_SIZE = 8; //number of files to replace/read
FILE *archive_file, *temp_file, *target_file;
char ch;
bool std = false;

int main (int argc, char *argv[]) {
	int i=0;

	if (argc < 3){
		fprintf(stderr, "farthing: too few arguments: %d\n", argc);
		return 1;
	}
	//printf("\ncmdline args count=%d", argc);
	for (i=1; i< argc; i++) {
		//printf("\narg%d=%s", i, argv[i]);
	}
	
	char * archive_name = argv[2];
	if (strcmp(archive_name, "-") == 0){
		std = true;
	}

	//printf("archive_name = %s\n", argv[2]);
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
		names[j] = malloc((len+1) * sizeof(char));
		strcpy(names[j], argv[i]);
		names_count += 1;
	}


	switch(*argv[1]){
		case 'r' :
		printf("replace\n");
		replace(archive_name, names, names_count);
		break;

		case 'x' :
		printf("execute\n");
		break;

		case 'd' :
		printf("delete\n");
		delete(archive_name, names, names_count);
		break;

		case 't' :
		printf("list\n");
		break;

		default :
		fprintf(stderr, "farthing: invalid Key: %s\n", argv[1]);

	}
	//printf("names_count = %d\n", names_count);
	for (int i = 0; i < names_count; ++i)
		free(names[i]);
	free(names);
	return 0;
 }

  int delete(char *archive_name, char **names, int names_count){
 	FILE *temp_file;
 	char ch; //use for reading through files with getc

 	// open archive file. if archive file doesn't exist, create it
 	if (!std){
 		archive_file = fopen(archive_name, "r");
 	 	if( archive_file == NULL )
		{
			archive_file = fopen(archive_name, "w");

			if ( archive_file ==  NULL ){
				printf("farthing: No disk space\n");
				exit(EXIT_FAILURE);
			}
		}	
 	}
 	else if (std){
 		archive_file = stdin;
 	}

	// create array of all file names 
	// iterate through names array
	// if it's a file, add name to file names
	// if it's a directory, go into directory and add all files
	char **file_names;

	file_names = malloc(NAMES_SIZE * sizeof(char*));
	int num_file_names = getFileNames(file_names, names, names_count);
	if (num_file_names == -1){
		for (int i = 0; i < names_count; ++i)
		{
			free(names[i]);
		}
		free(names);
		fclose(archive_file);
		exit(EXIT_FAILURE);
	}

	// we have a list of the files that need to be replaced in archive
	// and we have the archive file
	// now we need to read through archive
	// if we run into a file name in the archive that matches with a name in file_names
	//		then we have to skip the file in the archive, and write the new file into temp
	// else for the files that we didn't run into, we append them to temp
	char temp_name[PATH_MAX];
	if (!std){
		int file_descriptor = -1;
		memset(temp_name, 0, sizeof(temp_name));
		strcpy(temp_name,"temparchive-XXXXXX");
		file_descriptor = mkstemp(temp_name);

		if (file_descriptor < 1){
			printf("Creation of temp file failed\n");
			return 1;
		}

		// open temporary file with pointer temp_file
		temp_file = fdopen(file_descriptor, "w");
	}
	else
		temp_file = stdout;
	
	char temp[1024];
	char name_buffer[512];
	
	char name[512];
	int len, size;

	bool found = false;

	char header[512];
	size_t header_size = sizeof(header);

	while (fgets(temp, 1024, archive_file) != NULL){
		for (int i = 0; i < num_file_names; ++i){
			snprintf(name_buffer, sizeof(name_buffer), "| %s |", file_names[i]);
			if (strstr(temp, name_buffer) != NULL){
				found = true;
				sscanf( temp, "%d | %s | %d", &len, name, &size);
				// skip size bytes
				for(int i = 0; i < size; i++){
					fgetc(archive_file);
				}
			}
			if (found){
				break;
			}
		}
		if (!found){
			fputs(temp, temp_file);
		}
		found = false;
	}
	if (!std){
		// after temp is written, delete original and rename temp to original
		char del[] = "deleteme.temp";
		rename(archive_name, del);
		rename(temp_name, archive_name);
		unlink(del);
	}

	for (int i = 0; i < num_file_names; ++i)
		free(file_names[i]);

	free(file_names);

	fclose(archive_file);
	fclose(temp_file);
	return(0);
 }

 int replace(char *archive_name, char **names, int names_count){
 	FILE *temp_file;
 	char ch; //use for reading through files with getc

 	// open archive file. if archive file doesn't exist, create it
 	if (!std){
 		archive_file = fopen(archive_name, "r");
 	 	if( archive_file == NULL )
		{
			printf("farthing: Archive file doesn't exist, creating %s\n", archive_name);
			archive_file = fopen(archive_name, "w");

			if ( archive_file ==  NULL ){
				printf("No disk space??\n");
				exit(EXIT_FAILURE);
			}
		}	
 	}
 	else if (std){
 		archive_file = stdin;
 	}

	// create array of all file names 
	// iterate through names array
	// if it's a file, add name to file names
	// if it's a directory, go into directory and add all files
	char **file_names;

	file_names = malloc(NAMES_SIZE * sizeof(char*));
	int num_file_names = getFileNames(file_names, names, names_count);
	if (num_file_names == -1){
		for (int i = 0; i < names_count; ++i)
		{
			free(names[i]);
		}
		free(names);
		fclose(archive_file);
		exit(EXIT_FAILURE);
	}

	// we have a list of the files that need to be replaced in archive
	// and we have the archive file
	// now we need to read through archive
	// if we run into a file name in the archive that matches with a name in file_names
	//		then we have to skip the file in the archive, and write the new file into temp
	// else for the files that we didn't run into, we append them to temp
	char temp_name[PATH_MAX];
	if (!std){
		int file_descriptor = -1;
		memset(temp_name, 0, sizeof(temp_name));
		strcpy(temp_name,"temparchive-XXXXXX");
		file_descriptor = mkstemp(temp_name);

		if (file_descriptor < 1){
			printf("Creation of temp file failed\n");
			return 1;
		}
		else{
			 printf("Temporary file [%s] created\n", temp_name);
		}	

		// open temporary file with pointer temp_file
		temp_file = fdopen(file_descriptor, "w");
	}
	else
		temp_file = stdout;
	
	char temp[1024];
	char name_buffer[512];
	
	char name[512];
	int len, size;

	char target_name[PATH_MAX];
	memset(target_name, 0, sizeof(target_name));

	bool found = false;
	int replaced[num_file_names];
	for (int i = 0; i < num_file_names; ++i)
	{
		replaced[i] = 0;
	}


	char header[512];
	size_t header_size = sizeof(header);

	while (fgets(temp, 1024, archive_file) != NULL){
		for (int i = 0; i < num_file_names; ++i){
			strcpy(target_name, file_names[i]);
			snprintf(name_buffer, sizeof(name_buffer), "| %s |", target_name);
			if (strstr(temp, name_buffer) != NULL){
				found = true;
				replaced[i] = 1;
				sscanf( temp, "%d | %s | %d", &len, name, &size);
				for(int i = 0; i < size; i++){
					fgetc(archive_file);
				}
				target_file = fopen(target_name, "r");
				// get header of the file
				memset(header, 0, sizeof(header));
				
				// must pass header_size in because header decays to a pointer of 
				// size 8 when passed to function. Calling sizeof(header) in func
				// gives you 8 as opposed to 512.
				getFileHeader(target_name, header, header_size); 
				// move stream pointer past the file in archive, and then read
				// from target into temp
				fputs(header, temp_file);
				while( (ch = fgetc(target_file)) != EOF){
					fputc(ch, temp_file);
				}
				fclose(target_file);
			}
			if (found){
				break;
			}
		}
		if (!found){
			fputs(temp, temp_file);
		}
		found = false;
	}
	for (int i = 0; i < num_file_names; ++i) {
		if (replaced[i] != 1){
			memset(header, 0, sizeof(header));
			target_file = fopen(file_names[i], "r");
			getFileHeader(file_names[i], header, header_size); 

			fputs(header, temp_file);

			while( (ch = fgetc(target_file)) != EOF){
				fputc(ch, temp_file);
			}
			fclose(target_file);
		}
	}

	if (!std){
		// after temp is written, delete original and rename temp to original
		char del[] = "deleteme.temp";
		rename(archive_name, del);
		rename(temp_name, archive_name);
		unlink(del);
	}

	for (int i = 0; i < num_file_names; ++i)
	{
		free(file_names[i]);
	}
	free(file_names);
	fclose(archive_file);
	fclose(temp_file);
	return(0);
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

 int getFileNames(char **file_names, char **names, int names_count){
 	int j = 0;
 	bool error = false;
 	for (int i = 0; i < names_count; ++i)
 	{
 		if (j >= NAMES_SIZE){
 			NAMES_SIZE *= 2;
 			file_names = realloc(file_names, NAMES_SIZE*sizeof(char*));
 		}
 		if (isReg(names[i]) != 0){
 			file_names[j] = malloc(sizeof(char)*(strlen(names[i])+1));
            strcpy(file_names[j], names[i]);
 			j++;
 		}
 		else if (isDirectory(names[i]) != 0){
 			int *num_files;
 			num_files = malloc(sizeof(int));
 			*num_files = 0;

 			char **dir_files;
 			dir_files = malloc(sizeof(char*)*32); //maybe make dynamic later?

 			listFilesRecursively(names[i], dir_files, num_files);
 			for (int i = 0; i < *num_files; ++i)
		    {
		        //printf("%s\n", dir_files[i]);
		        file_names[j] = malloc(sizeof(char)*(strlen(dir_files[i])+1));
            	strcpy(file_names[j], dir_files[i]);
            	j++;
		    }


		    for (int i = 0; i < *num_files; ++i)
		    {
		        free(dir_files[i]);
		    }
		    free(dir_files);
		    free(num_files);
 		}
 		else{
 			fprintf(stderr, "farthing: %s does not exist\n", names[i]);
 			error = true;
 		}
 	}
 	if (error){
		for (int i = 0; i < j; ++i)
			free(file_names[i]);
		free(file_names);
 		return -1;
 	}
 	return j;
 }

void listFilesRecursively(char *basePath, char **dir_files, int *num_files)
{

    char path[1000];
    struct dirent *dp;
    DIR *dir = opendir(basePath);

    // base case, basePath is not directory
    if (!dir)
        return;

    while ((dp = readdir(dir)) != NULL)
    {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
        {

            // Construct new path from our base path
            strcpy(path, basePath);
            strcat(path, "/");
            strcat(path, dp->d_name);

            //printf("%s\n", path);
            if (isDirectory(path) == 0){
                dir_files[*num_files] = malloc(sizeof(char)*(strlen(path)+1));
                strcpy(dir_files[*num_files], path);
                *num_files = *num_files + 1;
            }

            listFilesRecursively(path, dir_files, num_files);
        }
    }
    closedir(dir);
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