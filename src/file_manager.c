#include "file_manager.h"
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#define MAXSIZE 1024

char *read_file(char *name)
{

    static char buffer[MAXSIZE];
    FILE *fp = fopen(name, "r");
    char a;
    int i = 0;

    while (((a = getc(fp)) != EOF) && (i < MAXSIZE))
        buffer[i++] = a;

    buffer[i-1] = '\0';

    fclose(fp);

    return buffer;
}

char *get_fdescriptor(char *path)
{

    DIR *dr = opendir(path);

    if (dr == NULL)
    {
        printf("Could not open directory!\n");
        return NULL;
    }

    struct dirent *de;
    static char string[MAXSIZE];
    string[0] = ' ';

    while ((de = readdir(dr)) != NULL)
    {
        strcat(string, de->d_name);
        strcat(string, " ");
    }

    closedir(dr);

    return string;
}