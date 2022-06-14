#ifndef VARIABLES_H
#define VARIABLES_H

int counter_processes;
int processes_running;

typedef struct node {
    struct node *next;
    char *var_name;
    char *var_value;
} Node;

Node *create_node(Node *next, char *var_name, char *var_value);

char *search_node(char *var_name);

int delete_variables();

Node *get_last_node();

void set_last_node(Node *curr);


typedef struct process {
    struct process *next;
    int *pid;
    int *number;
    char *pname;
} Process;

Process *create_process(Process *next, pid_t pid, int number, char *pname);

Process *delete_processes(Process *first);

Process *remove_processes(Process *first, Process *toRem);

Process *search_process(Process *first);

Process *get_last_process();

void set_last_process(Process *curr);

#endif