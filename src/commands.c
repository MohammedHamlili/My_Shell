#include <string.h>
#include <stdio.h> 
#include <dirent.h>
#include <unistd.h>
#include "builtins.h"
#include "io_helpers.h"
#include <ctype.h>
#include <stdlib.h>
#include "variables.h"
#include <sys/types.h>
#include <sys/wait.h>


int pipes(char **token_arr, char *input_buf, Node *last_node) {
    // int count = get_token_count();
    // for (int i = 0; i < count; i++) {
    //     if (i > 0 && i < count - 1 && strcmp(token_arr[i], "|") == 0 && (strcmp(token_arr[i-1], "|") != 0 || strcmp(token_arr[i+1], "|") == 0)) {
    //         display_error("ERROR: Unrecognized command: ", token_arr[i]);
    //         return -1;
    //     }
    // } 
    int original_stdout = dup(STDOUT_FILENO);
    int original_stdin = dup(STDIN_FILENO);
    // int original_stdin = STDIN_FILENO;
    char *curr_ptr = strtok (input_buf, "|");
    size_t token_count = 0;
    char *tokens[MAX_STR_LEN] = {NULL};
    while (curr_ptr != NULL) {  
        tokens[token_count] = curr_ptr;
        token_count += 1;
        curr_ptr = strtok(NULL, "|");
    }
    if (token_count == 0) {
        display_error("ERROR: Unrecognized command: ", "");
        return -1;
    }
    tokens[token_count] = NULL;
    int pipe_fd[token_count][2];
    // char *tokensP[MAX_STR_LEN] = {NULL};
    for (int i = 0; i < token_count; i++) {
        // int ret = get_input(tokens[i]);
        // printf("%s\n", tokens[i]);
        if (pipe(pipe_fd[i]) == -1) {
            display_error("ERROR: ", "pipe");
            return -1;
        }
        int pidi = fork();
        if (pidi < 0) {   // case: a system call error
            // handle the error
            display_error("ERROR: ", "fork");
            return -1;
        } 
        else if (pidi == 0) {  // case: Child process
            // Child does their work here.         Child only writes to the pipe so close reading end
            if (close(pipe_fd[i][0]) == -1) {
                display_error("ERROR: ", "close reading end from inside child");
                return -1;
            }
            // before we forked the parent had open the reading ends to
            // all previously forked children -- so close those
            // int child_no;
            // for (child_no = 1; child_no < i; child_no++) {
            //     if (i > 0 && close(pipe_fd[child_no][0]) == -1) {
            //         display_error("ERROR: ", "close reading ends of previously forked children");
            //         return -1;
            //     }
            // }
            if (i == token_count - 1) {
                dup2(original_stdout, STDOUT_FILENO);
            } else {
                dup2(pipe_fd[i][1], STDOUT_FILENO);
            }
            if (close(pipe_fd[i][1]) == -1){
                display_error("ERROR: ", "close duplicate writing end from inside child");
                return -1;
            }
            // tokenize_input(tokens[i], token_arr);
            // if (input_buf != tokens[i]) {
            //     strncpy(input_buf, tokens[i], strlen(input_buf));
            // }
            // return pidi;


            // LET'S TRY RUNNING THE COMMAND HERE INSTEAD
            int ret = strlen(tokens[i]);
            int exitflag = 0;
            size_t command_token_count = tokenize_input(tokens[i], token_arr);
            for (int i = 0; i < command_token_count; i++) {
                if (strncmp(token_arr[i], "$", 1) == 0) {
                    char *var_value = search_node(&(token_arr[i][1]));
                    if (var_value != NULL) {
                        // snprintf(token_arr[i], MAX_STR_LEN, "%s", var_value);
                        token_arr[i] = var_value;
                    }
                }
            }
            if (ret == 0 || (ret != -1 && (command_token_count > 0 && (strncmp("exit", token_arr[0], strlen(token_arr[0])) == 0)))) {
                // if (last_node == NULL) {
                //     delete_variables(last_node);
                // }
                exitflag = 1;
            }
            if (command_token_count >= 1 && exitflag == 0) {
                bn_ptr builtin_fn = check_builtin(token_arr[0]);
                if (command_token_count == 1 && strchr(token_arr[0], '=') != NULL) {
                    ;
                } else if (builtin_fn != NULL) {
                    // printf("mysh.c: last_node: var.name: %s\n", last_node->var_name);
                    ssize_t err = builtin_fn(token_arr);
                    if (err == - 1) {
                        display_error("ERROR: Builtin failed: ", token_arr[0]);
                    }
                } else {
                    int result = fork();
                    if (result == -1){
                        display_error("ERROR: ", "fork");
                        return -1;
                    } else if (result == 0) {
                        char *args[MAX_STR_LEN] = {NULL};
                        for (int i = 0; i < get_token_count(); i++) {
                            args[i] = token_arr[i];
                        }
                        if (execvp(token_arr[0], args) < 0) {
                            display_error("ERROR: Unrecognized command: ", token_arr[0]);
                        }
                        exit(0);
                    } else {
                        waitpid(result, NULL, 0);
                    }            
                    // display_error("ERROR: Unrecognized command: ", token_arr[0]);
                }
            }
            exit(0); // Exit so child process doesn't fork as well.
        } else { // Case: Parent Process. Needs to read from Child so we close write end of the pipe.
            waitpid(pidi, NULL, 0);
            if (close(pipe_fd[i][1]) == -1) {
                display_error("ERROR: ", "close writing end of pipe in parent");
                return -1;
            }
            dup2(pipe_fd[i][0], STDIN_FILENO);
            if (close(pipe_fd[i][0]) == -1){
                display_error("ERROR: ", "close duplicate reading end from inside child");
                return -1;
            }
        }
    }
    dup2(original_stdin, STDIN_FILENO);
    return EXIT_SUCCESS;
}