#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "variables.h"
#include "io_helpers.h"

Process *last_process = NULL;
Node *last_node = NULL;
int counter_processes = 0;
int processes_running = 0;


Node *create_node(Node *next, char *var_name, char *var_value) {
    Node *new_node = malloc(sizeof(Node));
    new_node->var_name=malloc(strlen(var_name)+1);
    new_node->var_value=malloc(strlen(var_value)+1);
    new_node->next = next;
    strcpy(new_node->var_name, var_name); // maybe strcpy
    strcpy(new_node->var_value, var_value);
    return new_node;
}

char *search_node(char *var_name) {
    Node *current = get_last_node();
    while (current != NULL) {
        // printf("Current->var_name: %s\n", current->var_name);
        if (strcmp(current->var_name,var_name) == 0) {
            // printf("%s\n", current->var_value);
            return current->var_value;
        }
        current = current->next;
    }
    return NULL;
}

int delete_variables() {
    Node *old = get_last_node();
    while (old != NULL) {
        Node *current = old->next; 
        free(old->var_name);
        free(old->var_value);
        free(old);
        old = current;
    }
    return 0;
}

Node *get_last_node() {
    return last_node;
}

void set_last_node(Node *curr) {
    last_node = curr;
}

Process *create_process(Process *next, pid_t pid_arg, int number_arg, char *p_name) {
    Process *process = malloc(sizeof(Process));
    process->pid = malloc(sizeof(int));
    process->number = malloc(sizeof(int));
    process->pname = malloc(strlen(p_name) + 1);
    process->next = next;
    *(process->number) = number_arg;
    *(process->pid) = pid_arg;
    strcpy(process->pname, p_name);
    return process;
}

Process *remove_processes(Process *first, Process *toRem) {
    if (first == toRem) {
        set_last_process(first->next);
        free(first->pid);
        free(first->number);
        free(first->pname);
        free(first);
        return get_last_process();
    }
    Process *current = first;
    while (current != NULL && current->next != toRem) {
        current = current->next;
    }
    if (current != NULL && current->next == toRem) {
        current->next = toRem->next;
        free(toRem->pid);
        free(toRem->number);
        free(toRem->pname);
        free(toRem);
        return current->next;
    } return current;
}

Process *search_process(Process *first) {
    Process *current = first;
    while (current != NULL) {
        int wstatus;
        if (waitpid(*(current->pid), &wstatus, WNOHANG) != 0) {
            if (WIFEXITED(wstatus) || WIFSIGNALED(wstatus)) {
                processes_running -= 1;
                char end_process[100] = "";
                sprintf(end_process, "[%d]+  Done %s\n", *(current->number), current->pname);
                display_message(end_process);
                current = remove_processes(get_last_process(), current);
            } else {
                current = current->next;
            }
        } else {
            current = current->next;
        }
    } return NULL;
}

Process *delete_processes(Process *first) {
    Process *old = first;
    while (old != NULL) {
        Process *current = old->next; 
        free(old->pid);
        free(old->number);
        free(old->pname);
        free(old);
        old = current;
    }
    return 0;
}

Process *get_last_process() {
    return last_process;
}

void set_last_process(Process *curr) {
    last_process = curr;
}

