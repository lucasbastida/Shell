#include <stdio.h>
#include <stdlib.h>

#include "string_manager.h"
#include "file_manager.h"
#include "process_exec.h"

#define HOSTNAMEPATH "/proc/sys/kernel/hostname"

int main()
{
    char *username = getenv("USER");
    char *hostname = read_file(HOSTNAMEPATH);
    char *pwd;

    char *input;
    char **tokens;
   
    do
    {
        pwd = getenv("PWD");

        printf("%s@%s:%s$", username, hostname, replace_str(pwd, getenv("HOME"), "~"));
        input = read_input();
        tokens = read_tokens(input);

        execute(tokens);

        free(input);
        free(tokens);
    } while (1);

    return 0;
}