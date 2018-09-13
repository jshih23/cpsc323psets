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
int isDirectory(const char *path);
int isReg(const char *path);
void listFilesRecursively(char *basePath, char **dir_files, int *num_files);
int getFileNames(char **file_names, char **names, int names_count);

/* global variable declaration */
int NAMES_SIZE = 8; //number of files to replace/read
FILE *archive_file, *temp_file, *target_file;

int main (int argc, char *argv[]) {
	int i=0;

	if (argc < 3){
		fprintf(stderr, "Invalid Arguments: Too few arguments: %d\n", argc);
		return 1;
	}
	//printf("\ncmdline args count=%d", argc);
	for (i=1; i< argc; i++) {
		//printf("\narg%d=%s", i, argv[i]);
	}
	//printf("\n");

	char * archive_name = argv[2];

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
		break;

		case 't' :
		printf("list\n");
		break;

		default :
		fprintf(stderr, "Invalid Key: %s\n", argv[1]);

	}
	//printf("names_count = %d\n", names_count);
	for (int i = 0; i < names_count; ++i)
	{
		//printf("%s freed\n", names[i]);
		free(names[i]);
	}
	free(names);
	return 0;
 }

 int replace(char *archive_name, char **names, int names_count){
 	FILE *temp_file;
 	char ch; //use for reading through files with getc

 	// open archive file. if archive file doesn't exist, create it
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

	// create array of all file names 
	// iterate through names array
	// if it's a file, add name to file names
	// if it's a directory, go into directory and add all files
	char **file_names;

	file_names = malloc(NAMES_SIZE * sizeof(char*));
	int num_file_names = getFileNames(file_names, names, names_count);
	for (int i = 0; i < num_file_names; ++i)
	{
		printf("poo; %s\n", file_names[i]);
	}

	// now, we have a list of the files that need to be replaced in archive
	// and we have the archive file
	// now we need to read through archive
	// if we run into a file name in the archive that matches with a name in file_names
	//		then we have to skip the file in the archive, and write the new file into temp
	// for the files that we didn't run into, we append them to temp


	for (int i = 0; i < num_file_names; ++i)
	{
		free(file_names[i]);
	}
	free(file_names);
	fclose(archive_file);
	return(0);
 }

 int getFileNames(char **file_names, char **names, int names_count){
 	int j = 0;
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