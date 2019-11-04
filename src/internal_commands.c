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
    if (strcmp(args[0], "cd") == 0)
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
                switch (change_pwd)
                {
                case ENOENT:
                    fprintf(stderr, "Error. %s doesnt exist...\n", args[1]);
                    break;
                case ENOTDIR:
                    fprintf(stderr, "Error. %s isnt a directory...\n", args[1]);
                    break;
                default:
                    fprintf(stderr, "Error. Couldnt change the directory...\n");
                }
                return -1;
            }

            char *cwd = getcwd(NULL, 0);
            setenv("PWD", cwd, 1);

            free(cwd);
            return 0;
        }
    }
    return 1;
}

int echo_func(char **args)
{
    if (strcmp(args[0], "echo") == 0)
    {
        for (size_t pos = 1; args[pos] != NULL; pos++)
        {
            fprintf(stdout, "%s ", args[pos]);
        }
        fprintf(stdout, "\n");
        fflush(stdout);

        for (size_t i = 0; args[i] != NULL; i++)
        {
            free(args[i]);
        }
        free(args);

        return 0;
    }
    return 1;
}

int clr_func(char **args)
{
    if (strcmp(args[0], "clr") == 0)
    {
        fprintf(stdout, "\033[H\033[2J"); //\033 = \e ASCII escape sequence
        return 0;
    }
    return 1;
}

int quit_func(char **args)
{
    if (strcmp(args[0], "quit") == 0)
    {
        exit(EXIT_SUCCESS);
    }
    return 1;
}