#ifndef PROCESS_EXEC_H
#define PROCESS_EXEC_H

int fork_process(char **, int);
int execute(char **);
int execv_mod(char **);
int check_background_execution(char **);

#endif