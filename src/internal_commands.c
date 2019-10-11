#define _GNU_SOURCE

#include "internal_commands.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

#include <dirent.h>
#include <errno.h>


int cd_func(char **args)
{
    if (args[1] == NULL)
    {
        fprintf(stderr, "Error. cd sin argumentos...\n");
    }
    else
    {
        int change_pwd = chdir(args[1]);
        if (change_pwd != 0)
        {
            fprintf(stderr, "Error. Couldnt change the directory...\n");
            return -1;
        }

        char* cwd =  getcwd(NULL,0);
        setenv("PWD", getcwd(NULL,0), 1);

        free(cwd);
        return 0;
    }
    return 1;
}

int run_cmd(char **args)
{
    if (strcmp(args[0], "quit") == 0)
    {
        exit(EXIT_SUCCESS);
    }

    if (strcmp(args[0], "clr") == 0)
    {
        fprintf(stdout, "\033[H\033[2J");//ASCII escape sequence
        return 0;
    }

    if (strcmp(args[0], "echo") == 0)
    {
        for (size_t pos = 1; args[pos] != NULL; pos++)
        {
            fprintf(stdout, "%s ", args[pos]);
        }
        fprintf(stdout, "\n");
        return 0;
    }

    if (strcmp(args[0], "cd") == 0)
    {
        return cd_func(args);
    }

    return 1;
}