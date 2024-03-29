#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "string_functions.h"


char *read_input()
{
    char *lineptr = NULL;
    size_t n = 0;
    getline(&lineptr, &n, stdin);
    return lineptr;
}

char **read_tokens(char *line)
{
    int tok_max = 15;

    char *token;
    const char* delim = " \t\n";
    char **tokens = (char **)malloc(tok_max * sizeof(char *));

    int tok_index = 0;
    token = strtok(line, delim);

    while (token != NULL)
    {
        if (tok_max == tok_index - 2)
        {
            tok_max += tok_max;
            tokens = (char **)realloc(tokens, tok_max * sizeof(char *));
            if (tokens == 0)
            {
                exit(1); //failed to resize
            }
        }

        tokens[tok_index] = strdup(token);
        token = strtok(NULL, delim);
        tok_index++;
    }

    tokens[tok_index] = NULL;
    
    return tokens;
}