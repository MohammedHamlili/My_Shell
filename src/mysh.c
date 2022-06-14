#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

#include "builtins.h"
#include "io_helpers.h"
#include "variables.h"
#include "commands.h"



void signal_handler(int sig_num) {
    display_message("\n");
}

int main(int argc, char* argv[]) {
    char *prompt = "mysh$ "; // TODO Step 1, Uncomment this.

    char input_buf[MAX_STR_LEN + 1];
    input_buf[MAX_STR_LEN] = '\0';
    char *token_arr[MAX_STR_LEN] = {NULL};
    int bckg_res = 1;
    while (1) {
        // Setting up the flags
        int bckg_flag = 0;
        int flag = 1;
        // strncpy(input_buf,"",MAX_STR_LEN);
        if (processes_running <= 0) {
            counter_processes = 0;
            processes_running = 0;
        }
        // Prompt and input tokenization
        // TODO Step 2:
        // Display the prompt via the display_message function.
        display_message(prompt);
        // Set up the handler for the Ctrl-C input (SIGINT signal)
        struct sigaction newact;
        newact.sa_handler = signal_handler;
        newact.sa_flags = 0;
        sigemptyset(&newact.sa_mask);
        sigaction(SIGINT, &newact, NULL);

        // search_process(get_last_process());

        int ret = get_input(input_buf);
        char tempInputBuf[MAX_STR_LEN] = "";
        strncpy(tempInputBuf, input_buf, strlen(input_buf));
        if (strlen(tempInputBuf) > 2 && (tempInputBuf[strlen(tempInputBuf) - 2] == '&' && tempInputBuf[strlen(tempInputBuf) - 1] == '\n')) {
            tempInputBuf[strlen(tempInputBuf)-2] = '\0'; // Removes the '&' from the end of the string.
        }
        size_t token_count = tokenize_input(input_buf, token_arr);
        for (int i = 0; i < token_count; i++) {
            if (strncmp(token_arr[i], "$", 1) == 0) {
                char *var_value = search_node(&(token_arr[i][1]));
                if (var_value != NULL) {
                    // snprintf(token_arr[i], MAX_STR_LEN, "%s", var_value);
                    token_arr[i] = var_value;
                }
            }
        }
        if (token_count >= 1) {
            bn_ptr builtin_fn = check_builtin(token_arr[0]);
            if (strchr(token_arr[token_count - 1], '&') != NULL) {
                if (strlen(token_arr[token_count - 1]) == 1) {
                    token_arr[token_count - 1] = NULL;
                    set_token_count(token_count - 1);
                    bckg_flag = 1;
                } else if (builtin_fn != bn_echo){
                    strtok(token_arr[token_count-1], "&");
                    bckg_flag = 1;
                }
                if (bckg_flag == 1) {
                    counter_processes += 1;
                    processes_running += 1;
                    bckg_res = fork();
                }
            }
            if (bckg_flag != 1 || bckg_res == 0) {
                if (strchr(tempInputBuf, '|') != NULL) {
                    flag = pipes(token_arr, tempInputBuf, get_last_node()); // If there is | in the input_buf, pipes is the handler.
                }
                if ((ret == 0 || (ret != -1 && (token_count > 0 && (strncmp("exit", token_arr[0], sizeof(*token_arr)) == 0)))) && flag != 0) {
                    if (get_last_node() != NULL) {
                        delete_variables(get_last_node());
                    }
                    if (get_last_process() != NULL) {
                        delete_processes(get_last_process());
                    }
                    // bn_close(token_arr);
                    break;
                }
                if (flag != 0) {
                    char variable_name[MAX_STR_LEN] = "";
                    char variable_value[MAX_STR_LEN] = "";
                    char *pointer_equal = strchr(token_arr[0], '=');
                    if (token_count == 1 && pointer_equal != NULL) {
                        int index_equal = pointer_equal - token_arr[0];
                        strncpy(variable_name, "", 64);
                        strncpy(variable_value, "", 64);
                        strncpy(variable_name, token_arr[0], index_equal);
                        strncpy(variable_value, pointer_equal+1, 64 - index_equal);
                        Node *node = create_node(get_last_node(), variable_name, variable_value);
                        set_last_node(node);
                    }
                    else if (builtin_fn != NULL) {
                        ssize_t err = builtin_fn(token_arr);
                        if (err == - 1) {
                            display_error("ERROR: Builtin failed: ", token_arr[0]);
                        }
                    } 
                    else {
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
                    }
                }
                if (bckg_res == 0) { 
                    delete_processes(get_last_process());
                    exit(0); // END OF EXECUTION
                }
            } else {
                char start_process[100] = "";
                sprintf(start_process, "[%d] %d\n", counter_processes, bckg_res);
                display_message(start_process);
                Process *process = create_process(get_last_process(), bckg_res, counter_processes, tempInputBuf);
                set_last_process(process);
            }
        }
        search_process(get_last_process());
    }
    return 0;
}
