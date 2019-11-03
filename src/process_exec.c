#define _GNU_SOURCE

#include "process_exec.h"
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

#define TRUE 1
#define FALSE 0

#define MAX_SIZE 1024

int execv_mod(char *argv[])
{
    /* 
        Trato de ubicar al ejecutable en la siguiente orden:
        1- si contiene / al inicio, es decir, es un camino a un archivo
        2- me fijo en el directorio actual
        3- trato de ubicar el archivo los los directorios de PATH
     */

    //1
    if (strchr(argv[0], '/') != NULL)
    {
        execv(argv[0], argv); //if execv returns then an error occurred
        return -1;
    }

    //2
    char buffer[MAX_SIZE];
    char *test = getcwd(buffer, MAX_SIZE);
    if (test == NULL)
    {
        printf("ERR\n");
    }

    strcat(buffer, "/");
    strcat(buffer, argv[0]);
    execv(buffer, argv);

    //3
    char *path = getenv("PATH");
    char *subpath = NULL;
    char *delim = ":";

    subpath = strtok(path, delim);

    memset(buffer, 0, sizeof buffer);

    while (subpath != NULL)
    {
        strcat(buffer, subpath);
        strcat(buffer, "/");
        strcat(buffer, argv[0]);

        execv(buffer, argv);

        memset(buffer, 0, sizeof buffer);
        subpath = strtok(NULL, delim);
    }

    return -1;
}

int fork_process(char **args, int is_background_proc)
{

    pid_t child_pid;
    int child_status;
    child_pid = fork();
    int fd;

    if (child_pid == (pid_t)0) //child
    {
        int inputRedir = FALSE;
        int outputRedir = FALSE;

        char input[MAX_SIZE];
        char output[MAX_SIZE];

        for (int i = 0; args[i] != NULL; i++)
        {
            if (strcmp(args[i], "<") == 0)
            {
                args[i] = NULL;
                strcpy(input, args[i + 1]);
                inputRedir = TRUE;
            }

            if (strcmp(args[i], ">") == 0)
            {
                args[i] = NULL;
                strcpy(output, args[i + 1]);
                outputRedir = TRUE;
            }
        }

        if (inputRedir)
        {
            int fd0 = open(input, O_RDONLY);
            dup2(fd0, STDIN_FILENO);
            close(fd0);
        }

        if (outputRedir)
        {
            int fd1 = creat(output, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
            dup2(fd1, STDOUT_FILENO);
            close(fd1);
        }

        if (is_background_proc && !outputRedir)
        {
            fd = open("/dev/null", O_WRONLY | O_CREAT);
            dup2(fd, 1);
        }

        if (echo_func(args) == 0)
        {
            exit(EXIT_SUCCESS);
        }

        execv_mod(args);
        abort();
    }

    else if (!is_background_proc)
    {
        while (wait(&child_status) != child_pid)
        {
        } //parent waits for child proc to finish
    }

    return 0;
}

int execute(char **args)
{
    /* check if empty string */
    if (args[0] == NULL)
    {
        return 1;
    }
    else if (quit_func(args) == 0)
    {
        return 0;
    }
    else if (cd_func(args) == 0)
    {
        return 0;
    }
    else if (clr_func(args) == 0)
    {
        return 0;
    }

    int token_pos = 0;

    while (args[token_pos] != NULL)
    {
        if (strcmp(args[token_pos], "&") == 0 && args[token_pos + 1] == NULL) //si & es el ultimo token
        {
            /* Remove & and set pointer to NULL */
            char *temp = args[token_pos];
            args[token_pos] = NULL;
            free(temp);

            return fork_process(args, TRUE);
        }
        token_pos++;
    }

    return fork_process(args, FALSE);
}
