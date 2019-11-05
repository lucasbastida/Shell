#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "string_manager.h"
#include "process_exec.h"

void command_mode();
void batch_mode(char *);

int main(int argc, char *argv[])
{

    if (argc == 1)
        command_mode();
    else if (argc == 2)
        batch_mode(argv[1]);
    else
    {
        printf("Demasiados argumentos...");
        exit(EXIT_FAILURE);
    }
    return 0;
}

void batch_mode(char *file)
{

    FILE *fp = fopen(file, "r");
    if (fp == NULL)
    {
        exit(EXIT_FAILURE);
    }

    char buffer[1024];
    char **tokens;

    while (fgets(buffer, sizeof buffer, fp) != NULL)
    {
        if (strcmp(buffer, "\n") != 0)
        {
            tokens = read_tokens(buffer);
            execute(tokens);

            for (size_t i = 0; tokens[i] != NULL; i++)
            {
                free(tokens[i]);
            }
            free(tokens);
        }
    }

    fclose(fp);
    exit(EXIT_SUCCESS);
}

void command_mode()
{
    char *HOSTNAMEPATH = "/proc/sys/kernel/hostname";

    FILE *fp = fopen(HOSTNAMEPATH, "r");
    if (fp == NULL)
    {
        exit(EXIT_FAILURE);
    }

    char hostname[1024];
    fgets(hostname, sizeof hostname, fp);
    fclose(fp);

    hostname[strcspn(hostname, "\n")] = 0; //remove newline character

    char *username = getenv("USER");
    char *pwd;

    char *input;
    char **tokens;
    
    do
    {
        pwd = getenv("PWD");
        printf("\x1b[0;32m%s@%s\x1b[0m:\x1b[0;34m%s\x1b[0m$ ", username, hostname, pwd); //replace_str(pwd, getenv("HOME"), "~")
        //https://notes.burke.libbey.me/ansi-escape-codes/
        //https://gist.github.com/RabaDabaDoba/145049536f815903c79944599c6f952a
        input = read_input();
        tokens = read_tokens(input);

        execute(tokens);

        for (size_t i = 0; tokens[i] != NULL; i++)
        {
            free(tokens[i]);
        }
        free(tokens);
        free(input);

    } while (1);
}