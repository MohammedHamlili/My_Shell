#ifndef __BUILTINS_H__
#define __BUILTINS_H__

#ifndef MAX_CONNECTIONS
    #define MAX_CONNECTIONS 12
#endif

#ifndef MAX_BACKLOG
    #define MAX_BACKLOG 5
#endif

#ifndef MAX_NAME
    #define MAX_NAME 10
#endif

#ifndef MAX_USER_MSG
    #define MAX_USER_MSG 64
#endif

#ifndef MAX_PROTO_MSG
    #define MAX_PROTO_MSG MAX_NAME+1+MAX_USER_MSG+2
#endif

#ifndef BUF_SIZE
    #define BUF_SIZE MAX_PROTO_MSG+1 
#endif

#include <unistd.h>
#include "variables.h"

pid_t startServPid;
int serverRunning;

/* Type for builtin handling functions
 * Input: Array of tokens
 * Return: >=0 on success and -1 on error
 */
typedef ssize_t (*bn_ptr)(char **);
ssize_t bn_echo(char **tokens);
ssize_t bn_wc(char **tokens);
ssize_t bn_cat(char **tokens);
ssize_t bn_ls(char **tokens);
ssize_t bn_cd(char **tokens);
ssize_t bn_ps(char **tokens);
ssize_t bn_kill(char **tokens);
ssize_t bn_start(char **tokens);
ssize_t bn_close(char **tokens);
ssize_t bn_send(char **tokens);
ssize_t bn_client(char **tokens);

/* Return: index of builtin or -1 if cmd doesn't match a builtin
 */
bn_ptr check_builtin(const char *cmd);


/* BUILTINS and BUILTINS_FN are parallel arrays of length BUILTINS_COUNT
 */
static const char * const BUILTINS[] = {"echo", "wc", "cat", "ls", "cd", "ps", "kill", "start-server", "close-server", "send", "start-client"};
static const bn_ptr BUILTINS_FN[] = {bn_echo, bn_wc, bn_cat, bn_ls, bn_cd, bn_ps, bn_kill, bn_start, bn_close, bn_send, bn_client, NULL};    // Extra null element for 'non-builtin'
static const size_t BUILTINS_COUNT = sizeof(BUILTINS) / sizeof(char *);


int bn_ls_helper(char * path, int depth, char * substring);

// void sigint_handler();
void sigC_D_handler(int signum);
int get_server();
void set_server(int s);

// int setup_server_socket(struct listen_sock *s, int port_number);
// int accept_connection(int fd, struct client_sock **clients);
// void clean_exit(struct listen_sock s, struct client_sock *clients, int exit_status);

#endif
