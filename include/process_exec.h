#ifndef PROCESS_EXEC_H
#define PROCESS_EXEC_H

int fork_process(char **);
int execute(char **);
int exec_imp(char*, char **);
int is_background_exec(char **);

#endif