#ifndef __IO_HELPERS_H__
#define __IO_HELPERS_H__

#include <sys/types.h>
#include <stdio.h>
#include "builtins.h"

#define MAX_STR_LEN 64
#define DELIMITERS " \t\n"     // Assumption: all input tokens are whitespace delimited


/* Prereq: pre_str, str are NULL terminated string
 */
void display_message(char *str);
void display_error(char *pre_str, char *str);


/* Prereq: in_ptr points to a character buffer of size > MAX_STR_LEN
 * Return: number of bytes read
 */
ssize_t get_input(char *in_ptr);


/* Prereq: in_ptr is a string, tokens is of size >= len(in_ptr)
 * Warning: in_ptr is modified
 * Return: number of tokens.
 */
size_t tokenize_input(char *in_ptr, char **tokens);


void set_token_count(size_t token_counter);


size_t get_token_count(); // returns number of tokens of most recent command.


size_t *fileCounter(FILE *stream); // Counts number of words, characters, and newlines in a file.

// int write_to_socket(int sock_fd, char *buf, int len);
// int read_from_socket(int sock_fd, char *buf, int *inbuf);
// int get_message(char **dst, char *src, int *inbuf);
// int find_network_newline(const char *buf, int inbuf);
// int remove_client(struct client_sock **curr, struct client_sock **clients);
// int read_from_client(struct client_sock *object);


#endif