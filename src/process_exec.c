#define _GNU_SOURCE

#include "process_exec.h"

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
    /* Si contiene "/" entonces es un camino */
    if (strchr(argv[0], '/') != NULL)
    {
        execv(argv[0], argv); //if execv returns then an error occurred
        return -1;
    }

    /* Si no ese un camino me fijo en directorio actual luego en PATH */

    /* Executar en el actual directorio */
    char buffer[MAX_SIZE]; //buscar algo mejor que un arreglo te tamanio fijo ya que es propenso a problemas
    char *test = getcwd(buffer, MAX_SIZE);
    if (test == NULL /* condition */)
    {
        printf("ERR\n");
    }

    strcat(buffer, "/");
    strcat(buffer, argv[0]);
    execv(buffer, argv);

    /* iterar sobre los subcaminos e intentar ejecutar */

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

    /* NOTA: creo que usar strcat no es lo ideal, seguro hay otra forma mucho mejor y optima */
    return -1;
}

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
        setenv("PWD", args[1], 1);
        return 0;
    }
    return 1;
}

int fork_process(char **args, int is_background_proc)
{

    pid_t child_pid;
    int child_status;
    child_pid = fork();
    int fd;

    if (child_pid == (pid_t)0) //child
    {
        if (is_background_proc)
        {
            fd = open("/dev/null", O_WRONLY | O_CREAT);
            dup2(fd, 1);
        }

        execv_mod(args);
        abort(); //if execv returns then an error occurred
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

    /* Personalized commands */
    if (strcmp(args[0], "quit") == 0)
    {
        exit(EXIT_SUCCESS);
    }

    if (strcmp(args[0], "clr") == 0)
    {
        fprintf(stdout, "\E[H\E[2J");
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

    /* Program calls */
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
