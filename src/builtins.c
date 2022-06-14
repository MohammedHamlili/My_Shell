#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <dirent.h>
#include <ctype.h>
#include <stdlib.h>
#include <poll.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>         
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h> 
#include <assert.h>

#include "builtins.h"
#include "io_helpers.h"
#include "variables.h"

pid_t startServPid = -1;
int counterServer = 0;
int sigint_received = 0;
int s_object;
int clientRunning = 0;

void sigC_D_handler(int signum) {
    clientRunning = 1;
}

// ====== Command execution =====

/* Return: index of builtin or -1 if cmd doesn't match a builtin
 */
bn_ptr check_builtin(const char *cmd) {
    ssize_t cmd_num = 0;
    while (cmd_num < BUILTINS_COUNT &&
           strncmp(BUILTINS[cmd_num], cmd, MAX_STR_LEN) != 0) {
        cmd_num += 1;
    }
    return BUILTINS_FN[cmd_num];
}

// ===== Builtins =====

/* Prereq: tokens is a NULL terminated sequence of strings.
 * Return 0 on success and -1 on error ... but there are no errors on echo. 
 */
ssize_t bn_echo(char **tokens) {
    ssize_t index = 1;
    
    if (tokens[index] != NULL) {
        // TODO:
        // Implement the echo command
        display_message(tokens[index]);
        index += 1;
    }
    while (tokens[index] != NULL) {
        // TODO:
        // Implement the echo command
        display_message(" ");
        display_message(tokens[index]);
        index += 1;
    }
    display_message("\n");

    return 0;
}

ssize_t bn_wc(char **tokens) {
    size_t count = get_token_count();
    size_t *result;
    if (count == 1) {
            FILE *fptr = NULL;
            struct pollfd fds;
            fds.fd = 0;     
            fds.events = POLLIN; 
            int ret = poll(&fds, 1, 10); // in your project, set this to 10, not 3000.
            if (ret == 0) {
                display_error("ERROR: No input source provided", ""); 
                return -1;
            } else {
                fptr = stdin;
                result = fileCounter(fptr);
            }
    } else {
        FILE *fptr;
        fptr = fopen(tokens[1], "r");
        result = fileCounter(fptr);
    }
    if (result != NULL){
        size_t wc = result[0]; size_t cc = result[1]; size_t nc = result[2];
        char wordcount[200] = "word count "; char charcount[200] = "character count "; char nlcount[200] = "newline count ";
        char wordcount1[100]; char charcount1[100]; char nlcount1[100];
        sprintf(wordcount1, "%ld\n", wc); sprintf(charcount1, "%ld\n", cc); sprintf(nlcount1, "%ld\n", nc);
        strcat(wordcount, wordcount1); strcat(charcount, charcount1); strcat(nlcount, nlcount1);
        display_message(wordcount); display_message(charcount); display_message(nlcount);
    }
    return 0;
}

ssize_t bn_cat(char **tokens){
    size_t count = get_token_count();
    char line[MAX_STR_LEN + 1];
    if (count == 1){
            FILE *fptr = NULL;
            struct pollfd fds;
            fds.fd = 0;     
            fds.events = POLLIN; 
            int ret = poll(&fds, 1, 10); // in your project, set this to 10, not 3000.
            if (ret == 0) {
                display_error("ERROR: No input source provided", ""); 
                return -1;
            } else {
                fptr = stdin;
                while (fgets(line, MAX_STR_LEN + 1, fptr) != NULL) {
                    display_message(line);
                }
            }
    } else{
        for (int i = 1; i < count; i++){ // In Bash, the terminal goes through all the different texts provided and prints them all
            FILE *source;
            source = fopen(tokens[i], "r");
            if (source == NULL) {display_error("ERROR: Cannot open file", ""); return -1;} // Checking for any error
            fseek(source, 0, SEEK_END);
            long int size = ftell(source);
            fseek(source, 0, SEEK_SET);
            char buf[size];
            while (fgets(buf, size, source) != NULL) {
                // TRIAL
                // END TRIAL
                display_message(buf);
            } fclose(source);
        }
    }
    return 0;
}

ssize_t bn_cd(char **tokens) {
    size_t count = get_token_count();
    // char buf[200];
    // printf("%s \n", getcwd(buf, 200));
    if (count == 1) chdir("/");
    else if (count > 2) {
        display_error("ERROR: Invalid path", "");
        return -1;
    } else {
        char *newdir = tokens[1];
        // printf("%c \n", *newdir);
        if (*newdir == '.') { // Checks if the first character of the path is a dot. 
           int counter = 0;
           for (int i = 0; i < strlen(newdir); i++) {
               if (newdir[i] == '.') counter++;
           } if (counter == strlen(newdir)) {
               for (int c = 0; c < counter - 1; c++) {
                   chdir("..");
                   // printf("%s \n", getcwd(buf, 200));
               }
               return 0; }
        } if (chdir(newdir) == -1) { // This condition check also calls the chdir function.
            display_error("ERROR: Invalid path", "");
            return -1;
        }
    }

    // printf("%s \n", getcwd(buf, 200));
    return 0;
}

ssize_t bn_ls(char **tokens) {
    ssize_t token_count = get_token_count();
    int depth = 0; int t_depth;
    int bool_isnum = true;
    int counter_depth_rec = 0;
    char *substring = NULL;
    char path[200] = "./";
    // if (token_count == 2) strcat(path, tokens[1]);
    if (token_count >= 2) {
        int index = 1;
        while (tokens[index] != NULL) {
            // printf("%s \n", tokens[index]);
            // printf("%ld \n", strlen(tokens[index]));
            if (strncmp(tokens[index], "--f", strlen(tokens[index])) == 0) {
                if (tokens[index+1] == NULL) {
                    display_error("ERROR: No substring provided for the --f subcommand", "");
                    return -1;
                } substring = tokens[index + 1];
                index+=2;
            } else if (strncmp(tokens[index], "--d", strlen(tokens[index])) == 0) {
                // display_message("im here1");
                if (tokens[index + 1] == NULL) {
                    display_error("ERROR: No depth level provided for --d subcommand", "");
                    return -1;
                } 
                for (int i = 0; i < strlen(tokens[index + 1]); i++) {
                    size_t result = isdigit(tokens[index + 1][i]);
                    if (result == 0) bool_isnum = false;
                }
                if (bool_isnum) {
                    t_depth = atoi(tokens[index + 1]);
                    index+=2;
                    counter_depth_rec++;
                } else {
                    display_error("ERROR: No valid delpth level provided for --d subcommand", "");
                    return -1;
                }
            } else if (strncmp(tokens[index], "--rec", strlen(tokens[index])) == 0) {
                counter_depth_rec++;
                index++;
            } else {
                char input_path[198] = "";
                strncpy(input_path, tokens[index], strlen(tokens[index]));
                char *delim = "/";
                char *first_c = strtok(input_path, delim);
                while (first_c != NULL) {
                    // printf("%c \n", *first_c);
                    strcat(path, first_c); 
                    strcat(path, "/");
                    first_c = strtok(NULL, delim);
                }
                index++;
                }
            // printf("Index: %d \n", index);
        }
    }
    if (counter_depth_rec == 2) depth = t_depth;
    // printf("%d \n", depth);
    int result = bn_ls_helper(path, depth, substring);
    // DIR *dir = opendir(path);
    // if (dir == NULL) {
    //     display_error("ERROR: Invalid path", "");
    //     return -1; }
    // struct dirent* entity;
    // entity = readdir(dir);
    // while (entity != NULL) {
    //     printf("%s\n", entity->d_name);
    //     entity = readdir(dir);}  // closedir(dir);
    return result;
}

ssize_t bn_ps(char **tokens) {
    Process * current = get_last_process();
    while (current != NULL) { 
        char toDisplay[MAX_STR_LEN];
        sprintf(toDisplay, "%s %d\n", strtok(current->pname, " \t\n"), *(current->pid));
        display_message(toDisplay);
        current = current->next;
    } 
    return 0;
}

ssize_t bn_kill(char **tokens) {
    char *pid;
    if (tokens[1] != NULL) {
        pid = tokens[1];
    } else {
        pid = NULL;
    }
    char *signum;
    if (tokens[2] != NULL) {
        signum = tokens[2];
    } else {
        signum = NULL;
    }
    Process *current = get_last_process();
    while (current != NULL) {
        if (*(current->pid) == atoi(pid)) {
            if (signum == NULL) {
                kill(atoi(pid), SIGTERM);
                return 0;
            }
            else if (signum != NULL && kill(atoi(pid), atoi(signum)) == -1) {
                display_error("ERROR: ", "Invalid signal specified");
                return -1;
            } else return 0;
        }
        current = current->next; // Don't need an else because we return in the if clause.
    }
    // If it gets here, no process in the linked list has the given pid in the kill command.
    if (pid != NULL) {
        int killResult;
        if (signum != NULL) {
            killResult = kill(atoi(pid), atoi(signum));
        } else {
            killResult = kill(atoi(pid), SIGTERM);
        }
        if (killResult == 0) {
            return 0;
        } else if (errno == EINVAL) {
            display_error("ERROR: ", "Invalid signal specified");
            return -1;
        } else if (errno = ESRCH){
            display_error("ERROR: ", "The process does not exist");
            return -1;
        }
    }
    display_error("ERROR: ", "The process does not exist");
    return -1;
}

int bn_ls_helper(char * path, int depth, char * substring) {
    bool substring_flag = false;
    if (substring != NULL) substring_flag = true;
    DIR *dir = opendir(path);
    if (dir == NULL) {
        display_error("ERROR: Invalid path", "");
        return -1; }
    struct dirent* entity;
    entity = readdir(dir);
    while (entity != NULL) {
        if (substring_flag) {
            if (strstr(entity->d_name, substring) != NULL) {
                display_message(entity->d_name);
                display_message("\n");
            }
        } else {
            display_message(entity->d_name);
            display_message("\n");          
        } 
        if (depth > 0 && entity->d_type == DT_DIR && strcmp(entity->d_name, ".") != 0 && strcmp(entity->d_name, "..") != 0) {
            char new_path[300] = "./";
            strcat(new_path, path);
            strcat(new_path, "/");
            strcat(new_path, entity->d_name);
            if (depth - 1 >= 0) bn_ls_helper(new_path, depth - 1, substring);
        }
        entity = readdir(dir);
    }
    

    closedir(dir);
    return 0;
}

ssize_t bn_start(char **tokens) {
    ssize_t token_c = get_token_count();
    if (token_c == 1) {
        display_error("ERROR: ", "No port provided");
        return -1;
    } else if (isdigit(tokens[1][0]) && token_c == 2) {
        int port_number = atoi(tokens[1]);
        char buffer[MAX_STR_LEN];
        buffer[MAX_STR_LEN - 1] = '\0';
        counterServer = 1;
        int bckg_server = socket(AF_INET, SOCK_STREAM, 0);
        if (bckg_server == -1) {
		    display_error("ERROR: ", "socket");
            return -1;
	    }
        startServPid = fork();
        if (startServPid == -1) {
            display_error("ERROR: ", "Fork");
            return -1;
        } else if (startServPid == 0) {
            struct sockaddr_in address;
            address.sin_family = AF_INET;
            address.sin_port = htons(port_number);
            address.sin_addr.s_addr = INADDR_ANY;
            memset(&(address.sin_zero), 0, 8);

            // Make sure we can reuse the port immediately after the
            // server terminates. Avoids the "address in use" error
            int on = 1;
            int status = setsockopt(bckg_server, SOL_SOCKET, SO_REUSEADDR,
                (const char *) &on, sizeof(on));
            if (status < 0) {
                display_error("ERROR: ", "setsockopt");
                close(get_server());
                return -1;
            }

            // Bind the selected port to the socket.
            if (bind(bckg_server, (struct sockaddr *) &address , sizeof(struct sockaddr_in)) == -1){
                display_error("ERROR: ", "server: bind");
                close(get_server());
                return -1;
            }

            // Announce willingness to accept connections on this socket.
            if (listen(bckg_server, MAX_CONNECTIONS) < 0) {
                display_error("ERROR: ", "server: listen");
                close(get_server());
                return -1;
            }
            struct sockaddr_in client_address;
            client_address.sin_family = AF_INET;
            unsigned int client_len = sizeof(struct sockaddr_in);

            fd_set FDs;
            FD_ZERO(&FDs);
            
            while (1) {
            FD_SET(STDIN_FILENO, &FDs);
            FD_SET(bckg_server, &FDs);
            
            if (select(bckg_server + 1, &FDs, NULL, NULL, NULL) < 0) {
                display_error("ERROR: ", "server: select"); 
                close(get_server());
                return -1;
            }

            int value = accept(bckg_server, (struct sockaddr *) &client_address, &client_len);
            read(value, buffer, MAX_STR_LEN - 1);
            display_message("\n");
            display_message(buffer);
            display_message("mysh$ ");
            }
        } else {
            set_server(bckg_server);
        }
    } else {
        display_error("ERROR: ", "start-server");
        return -1;
    }
    close(get_server());
    return 0;
}

ssize_t bn_client(char **tokens) {
    if (get_token_count() != 3 || !isdigit(tokens[1][0])) {
        display_error("ERROR: ", "start-client: Invalid Usage");
        return -1;
    }
    int port = atoi(tokens[1]);
    char *host = tokens[2];
    int socketToWrite;
    if ((socketToWrite = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        display_error("ERROR: ", "client: socket");
        return -1;
    }

    struct addrinfo *ai;
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    memset(&server.sin_zero, 0, 8);
    getaddrinfo(host, NULL, NULL, &ai);
    server.sin_addr = ((struct sockaddr_in *) ai->ai_addr)->sin_addr;
    freeaddrinfo(ai); // free the memory that was allocated by getaddrinfo for this list

    int ret;
    if ((ret = connect(socketToWrite, (struct sockaddr *) &server, sizeof(struct sockaddr_in))) == -1) {
        perror("client: connect");
        return -1;
    }

    while (1) {
        clientRunning = 0;
        char buffer[MAX_STR_LEN] = "";
        int bytes_read = read(STDIN_FILENO, buffer, MAX_STR_LEN);
        write(socketToWrite, &buffer, bytes_read);
        
        struct sigaction newact;
        newact.sa_handler = sigC_D_handler;
        newact.sa_flags = 0;
        sigemptyset(&newact.sa_mask);
        sigaction(SIGINT, &newact, NULL);

        if (clientRunning == 1) break;
    }


    
    close(socketToWrite);

    return 0;

}

ssize_t bn_send(char **tokens) {
    if (get_token_count() < 4 || !isdigit(tokens[1][0])) {
        display_error("ERROR: ", "Send");
        return -1;
    }
    // int serverWriteTo = socket(AF_INET, SOCK_STREAM, 0);
    int serverWriteTo;
    char * host = tokens[2];
    int port = atoi(tokens[1]);

    if ((serverWriteTo = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        display_error("ERROR: ", "client: socket");
        return -1;
    }

    struct addrinfo *ai;
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    memset(&server.sin_zero, 0, 8);

    getaddrinfo(host, NULL, NULL, &ai);
    server.sin_addr = ((struct sockaddr_in *) ai->ai_addr)->sin_addr;
    freeaddrinfo(ai);

    int ret = connect(serverWriteTo, (struct sockaddr *)&server, sizeof(struct sockaddr_in));
    if (ret == -1) {
        display_error("ERROR: ", "client: connect");
        return -1;
    }
    char buf[MAX_STR_LEN] = ""; 
    for (int i = 3; i < get_token_count(); i++) {
        strncat(buf, tokens[i], strlen(tokens[i]));
        strcat(buf, " ");
    }
    strcat(buf, "\n");

    // int x = send(serverWriteTo, message, MAX_STR_LEN, 0);
    // int bytes_read = write(serverWriteTo, buf, MAX_STR_LEN);
    if (write(serverWriteTo, buf, MAX_STR_LEN) == -1){
        display_error("ERROR: ", "send");
        return -1;
    }
    close(serverWriteTo);
    return 0;
}

ssize_t bn_close(char **tokens) {
    if (counterServer == 1) {
        int s = get_server();
        close(s);
        // free(s.addr);
        kill(startServPid, SIGKILL);
        counterServer = 0;
    } else {
        display_error("ERROR: ", "No server to close");
    }
    return 0;
}

int get_server() {
    return s_object;
}

void set_server(int s) {
    s_object = s;
}

// int accept_connection(int fd, struct client_sock **clients) {
//     struct sockaddr_in peer;
//     unsigned int peer_len = sizeof(peer);
//     peer.sin_family = AF_INET;

//     int num_clients = 0;
//     struct client_sock *curr = *clients;
//     while (curr != NULL && num_clients < MAX_CONNECTIONS && curr->next != NULL) {
//         curr = curr->next;
//         num_clients++;
//     }

//     int client_fd = accept(fd, (struct sockaddr *)&peer, &peer_len);
    
//     if (client_fd < 0) {
//         display_error("ERROR: ", "server: accept");
//         close(fd);
//         return -1;
//     }

//     if (num_clients == MAX_CONNECTIONS) {
//         display_error("ERROR: ", "Maximum number of connections already established.")
//         close(client_fd);
//         return -1;
//     }

//     struct client_sock *newclient = malloc(sizeof(struct client_sock));
//     newclient->sock_fd = client_fd;
//     newclient->inbuf = newclient->state = 0;
//     newclient->next = NULL;
//     memset(newclient->buf, 0, BUF_SIZE);
//     if (*clients == NULL) {
//         *clients = newclient;
//     }
//     else {
//         curr->next = newclient;
//     }


//     return client_fd;
//     return 0;
// }

// int setup_server_socket(struct listen_sock *s, int port_number) {
//     if(!(s->addr = malloc(sizeof(struct sockaddr_in)))) {
//         display_error("ERROR: ", "malloc");
//         return -1;
//     }
//     // Allow sockets across machines.
//     s->addr->sin_family = AF_INET;
//     // The port the process will listen on.
//     s->addr->sin_port = htons(port_number);
//     // Clear this field; sin_zero is used for padding for the struct.
//     memset(&(s->addr->sin_zero), 0, 8);
//     // Listen on all network interfaces.
//     s->addr->sin_addr.s_addr = INADDR_ANY;

//     s->sock_fd = socket(AF_INET, SOCK_STREAM, 0);
//     if (s->sock_fd < 0) {
//         display_error("ERROR: ", "server socket");
//         return -1;
//     }

//     // Make sure we can reuse the port immediately after the
//     // server terminates. Avoids the "address in use" error
//     int on = 1;
//     int status = setsockopt(s->sock_fd, SOL_SOCKET, SO_REUSEADDR,
//         (const char *) &on, sizeof(on));
//     if (status < 0) {
//         display_error("ERROR: ","setsockopt");
//         return -1;
//     }

//     // Bind the selected port to the socket.
//     if (bind(s->sock_fd, (struct sockaddr *)s->addr, sizeof(*(s->addr))) < 0) {
//         display_error("ERROR: ","server: bind");
//         close(s->sock_fd);
//         return -1;
//     }

//     // Announce willingness to accept connections on this socket.
//     if (listen(s->sock_fd, MAX_CONNECTIONS) < 0) {
//         display_error("ERROR: ","server: listen");
//         close(s->sock_fd);
//         return -1;
//     }
//     return s->sock_fd;
//     return 0;
// }

// void clean_exit(struct listen_sock s, struct client_sock *clients, int exit_status) {
//     struct client_sock *tmp;
//     while (clients) {
//         tmp = clients;
//         close(tmp->sock_fd);
//         clients = clients->next;
//         free(tmp);
//     }
//     close(s.sock_fd);
//     free(s.addr);
//     exit(exit_status);
// }