/**
 * C program to list contents of a directory recursively.
 */

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

int isDirectory(const char *path);

void listFilesRecursively(char *path);
int *j;
char **file_names;

int main()
{
    // Directory path to list files
    char path[100];
    j = malloc(sizeof(int));
    *j = 0;
    file_names = malloc(sizeof(char*)*10);
    // Input path from user
    strcpy(path, "testdir");

    listFilesRecursively(path);

    for (int i = 0; i < *j; ++i)
    {
        printf("%s\n", file_names[i]);
    }
    for (int i = 0; i < *j; ++i)
    {
        free(file_names[i]);
    }
    free(file_names);
    free(j);
    return 0;
}


/**
 * Lists all files and sub-directories recursively 
 * considering path as base path.
 */
void listFilesRecursively(char *basePath)
{

    char path[1000];
    struct dirent *dp;
    DIR *dir = opendir(basePath);

    // Unable to open directory stream
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
                file_names[*j] = malloc(sizeof(char)*(strlen(path)+1));
                strcpy(file_names[*j], path);
                *j = *j + 1;
            }

            listFilesRecursively(path);
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