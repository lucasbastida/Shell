#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>

#include "string_manager.h"
#include "file_manager.h"
#include "process_exec.h"

#define HOSTNAMEPATH "/proc/sys/kernel/hostname"

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
        exit(EXIT_FAILURE);

    char buffer[1024];
    char **tokens;

    while (fgets(buffer, sizeof buffer, fp) != NULL)
    {
        // printf("LINE---%s", buffer);
        tokens = read_tokens(buffer);
        execute(tokens);
        free(tokens);
    }

    fclose(fp);
    exit(EXIT_SUCCESS);
}

void command_mode()
{

    char *username = getenv("USER");
    char *hostname = read_file(HOSTNAMEPATH);
    char *pwd;

    char *input;
    char **tokens;

    do
    {
        pwd = getenv("PWD");

        printf("%s@%s:%s$ ", username, hostname, replace_str(pwd, getenv("HOME"), "~"));
        input = read_input();
        tokens = read_tokens(input);

        execute(tokens);

        free(input);
        free(tokens);
    } while (1);
}