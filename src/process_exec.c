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

int is_background_exec(char **args)
{
    int token_pos = 0;

    while (args[token_pos] != NULL)
    {
        if (strcmp(args[token_pos], "&") == 0 && args[token_pos + 1] == NULL) //si & es el ultimo token
        {
            /* Remove & and set pointer to NULL */
            char *temp = args[token_pos];
            args[token_pos] = NULL;
            free(temp);

            return 1; //TRUE
        }
        token_pos++;
    }
    return 0; //FALSE
}

int exec_imp(char *file_name, char *argv[])
{

    /* 
        Trato de ubicar al ejecutable en la siguiente orden:
        1- si contiene / al, es decir, es un camino a un archivo
        caso contrario es el nombre de un ejecutable que deberia estar ubucado en:
        2- me fijo en el directorio actual
        3- trato de ubicar el archivo los los directorios de PATH
     */

    char buffer[1024];

    if (strchr(file_name, '/') != NULL)
    {
        execv(file_name, argv); //if execv returns then an error occurred
    }
    else
    {

        char *path = getenv("PATH");
        if (path == NULL)
        {
            fprintf(stderr, "PATH variable = NULL...");
            return 1;
        }

        size_t filelen = strlen(file_name) + 1;
        size_t pathlen = strlen(path);

        /* Add name to end of buffer and set pointer to ->/file_name */
        char *name = memcpy(buffer + pathlen + 1, file_name, filelen);
        *--name = '/';

        /* Try for current directory */
        char *cur_dir = name - 1;
        *cur_dir = '.';

        execv(cur_dir, argv);

        /* Try for directories in PATH */
        char *p = path;
        int exec_perm_denied = 0;

        do
        {
            char *path_dir;
            path = p;
            p = strchr(path, ':');
            if (!p) //if no :, get pointer to terminating null byte
            {
                p = strchr(path, '\0');
            }
            if (p == path) //if : is at beginning or end of path
            {
                path_dir = name + 1; 
            }
            else
            {
                path_dir = memcpy(name - (p - path), path, p - path); //copy pathname into buffer and point at the beginning of buffer
            }

            execv(path_dir, argv); //returns only on error.

            switch (errno) //man execve
            {
            case EACCES: //permission denied, diagnose we found it but couldnt execute it
                exec_perm_denied = 1;
            case ENOENT:  //file_namename doesnt exist, break
            case ENOTDIR: //component of path or file_namename isnt a directory
            case ENODEV:  //no such device
            case ENOEXEC: //these errors indicate file_name missing or not executable by us. so we want to try the next path
                break;
            case E2BIG: // other errors mean we found but something went wrong executing it, print and return error
            case EAGAIN:
            case EFAULT:
            case EINVAL:
            case EIO:
            case EISDIR:
            case ELOOP:
            case EMFILE:
            case ENAMETOOLONG:
            case ENFILE:
            case ENOMEM:
            case EPERM:
            case ETXTBSY:
            default:
                perror("execv");
                return errno;
            }

        } while (*p++ != '\0'); //in next loop path will point to p and p will get next : in path

        /* If not able to execute after checking all paths, warn permission denied */
        if (exec_perm_denied)
        {
            fprintf(stderr, "Execute permission is denied for the file\n");
            return 1;
        }
    }

    fprintf(stderr, "%s: command not found\n", file_name);
    return errno;
}

int fork_process(char **args)
{

    int background = is_background_exec(args);

    pid_t child_pid;
    int child_status;
    child_pid = fork();

    if (child_pid == (pid_t)0) //child
    {
        /* I/O redirection */
        int fd;
        int i = 0;
        while (args[i])
        {
            if (*args[i] == '<' && args[i + 1])
            {
                if ((fd = open(args[i + 1], O_RDONLY)) == -1)
                {
                    perror(args[i + 1]);
                    exit(EXIT_FAILURE);
                }
                dup2(fd, STDIN_FILENO);
                close(fd);
                while (args[i])
                {
                    args[i] = args[i + 2];
                    i++;
                }
                break;
            }
            else if (*args[i] == '>' && args[i + 1])
            {
                //int fd = open(args[i + 1], O_WRONLY | O_CREAT | O_TRUNCATE, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
                //creat es equiv to the above O_WRONLY | O_CREAT | O_TRUNCATE

                if ((fd = creat(args[i + 1], S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) == -1)
                {
                    perror(args[i + 1]);
                    exit(EXIT_FAILURE);
                }
                dup2(fd, STDOUT_FILENO);
                // dup2(fd, STDERR_FILENO); ADD STDOUT_ERROR?
                close(fd);
                while (args[i])
                {
                    args[i] = args[i + 2];
                    i++;
                }
                break;
            }
            i++;
        }

        /* After redirecting, call the echo function */
        if (echo_func(args) == 0)
        {
            _exit(EXIT_SUCCESS); //prevents stdio buffers to be flushed twice and temporary files might be removed
        }

        exec_imp(args[0], args);
        abort();
    }

    else if (!background)
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

    return fork_process(args);
}