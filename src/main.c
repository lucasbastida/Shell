#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "string_manager.h"
#include "process_exec.h"

void command_mode();
void batch_mode(char *);
void print_prompt();

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
        printf("LINE---%s", buffer);
        if (strcmp(buffer, "\n") != 0)
        {
            tokens = read_tokens(buffer);
            execute(tokens);
            free(tokens);
        }
    }

    fclose(fp);
    exit(EXIT_SUCCESS);
}

void command_mode()
{
    char *input;
    char **tokens;

    do
    {
        print_prompt();
        input = read_input();
        tokens = read_tokens(input);

        execute(tokens);

        free(input);
        free(tokens);
    } while (1);
}

void print_prompt()
{
    char *HOSTNAMEPATH = "/proc/sys/kernel/hostname";

    FILE *fp = fopen(HOSTNAMEPATH, "r");
    if (fp == NULL)
    {
        exit(EXIT_FAILURE);
    }

    char hostname[1024];
    fgets(hostname, sizeof hostname, fp);
    hostname[strcspn(hostname, "\n")] = 0;

    char *username = getenv("USER");
    char *pwd = getenv("PWD");

    printf("%s@%s:%s$ ", username, hostname, replace_str(pwd, getenv("HOME"), "~"));

    fclose(fp);
}