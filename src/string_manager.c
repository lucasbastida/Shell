#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "string_manager.h"

#define MAXSIZE 1024

char *replace_str(char *str, char *orig, char *rep)
{
    static char buffer[MAXSIZE];
    char *p;

    if (!(p = strstr(str, orig))) // Is 'orig' even in 'str'?
        return str;

    strncpy(buffer, str, p - str); // Copy characters from 'str' start to 'orig' st$
    buffer[p - str] = '\0';

    sprintf(buffer + (p - str), "%s%s", rep, p + strlen(orig));

    return buffer;
}

char *read_input()
{
    char *input = NULL;
    size_t bufsize = 0;
    getline(&input, &bufsize, stdin);
    return input;
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