#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <errno.h>

#include "io_helpers.h"
#include "builtins.h"


// ===== Output helpers =====

/* Prereq: str is a NULL terminated string
 */
void display_message(char *str) {
    write(STDOUT_FILENO, str, strnlen(str, MAX_STR_LEN));
}

/* Prereq: pre_str, str are NULL terminated string
 */
void display_error(char *pre_str, char *str) {
    write(STDERR_FILENO, pre_str, strnlen(pre_str, MAX_STR_LEN));
    write(STDERR_FILENO, str, strnlen(str, MAX_STR_LEN));
    write(STDERR_FILENO, "\n", 1);
}

// ===== Input tokenizing =====

/* Prereq: in_ptr points to a character buffer of size > MAX_STR_LEN
 * Return: number of bytes read
 */
ssize_t get_input(char *in_ptr) {
    int retval = read(STDIN_FILENO, in_ptr, MAX_STR_LEN+1); // Not a sanitizer issue since in_ptr is allocated as MAX_STR_LEN+1
    int read_len = retval;
    if (retval == -1) {
        read_len = 0;
    }
    if (read_len > MAX_STR_LEN) {
        read_len = 0;
        retval = -1;
        write(STDERR_FILENO, "ERROR: input line too long\n", strlen("ERROR: input line too long\n"));
        int junk = 0;
        while((junk = getchar()) != EOF && junk != '\n');
    }
    in_ptr[read_len] = '\0';
    return retval;
}

/* Prereq: in_ptr is a string, tokens is of size >= len(in_ptr)
 * Warning: in_ptr is modified
 * Return: number of tokens.
 */
size_t tokenize_input(char *in_ptr, char **tokens) {
    // TODO, uncomment the next line.
    char *curr_ptr = strtok (in_ptr, DELIMITERS);
    size_t token_count = 0;

    while (curr_ptr != NULL) {  // TODO: Fix this
        // TODO: Fix this
        tokens[token_count] = curr_ptr;
        token_count += 1;
        curr_ptr = strtok(NULL, DELIMITERS);
    }
    tokens[token_count] = NULL;
    set_token_count(token_count);
    return token_count;
}

int token_counter; // New global variable to represent number of tokens in most recent command.  NOTICE: INITIALIZE TO 0 MAYBE
void set_token_count(size_t token_count) {
    token_counter = token_count;
}

size_t get_token_count() {
    return token_counter;
}

size_t *fileCounter(FILE *f) {
    static size_t result[3];
    if (f == NULL) {
        display_error("ERROR: Cannot open file", "");
        return NULL; }
    bool whitespace_flag = false;
    size_t wc = 0; size_t cc = 0; size_t nc = 0;
    char character = fgetc(f);
    while (character != EOF) {
        cc++;
        if (character == '\n'){
            nc++;
            whitespace_flag = false;
        } else if (character == '\t' || character == '\r' || character == ' ') {
            whitespace_flag = false;
        } else if (whitespace_flag == false) {
            whitespace_flag = true;
            wc++;
        }
        character =fgetc(f);
    }
    result[0] = wc; result[1] = cc; result[2] = nc;
    return result;
}

// int read_from_client(struct client_sock *object) {
//     return read_from_socket(object->sock_fd, object->buf, &(object->inbuf));
// }

// int read_from_socket(int sock_fd, char *buf, int *inbuf) {
//     int num_read = read(sock_fd, buf + *inbuf, BUF_SIZE - *inbuf);
//     if (num_read == 0) {
//         return 1;
//     } 
//     else if (num_read == -1) {
//         display_error("ERROR: ", "read");
//         return -1;
//     }
//     *inbuf += num_read;
//     for (int i = 0; i <= *inbuf-2; i++) {
//         if (buf[i] == '\r' && buf[i+1] == '\n') {
//             return 0;
//         }
//     }
//     if (*inbuf == BUF_SIZE) {
//         return -1;
//     }
//     return 2;
// }

// int get_message(char **dst, char *src, int *inbuf) {
//     // Implement the find_network_newline() function
//     // before implementing this function.
//     int msg_len = find_network_newline(src, *inbuf);
//     if (msg_len == -1) return 1;
//     *dst = malloc(BUF_SIZE);
//     if (*dst == NULL) {
//         display_error("ERROR: ","malloc");
//         return 1;
//     }
//     memmove(*dst, src, msg_len - 2);
//     (*dst)[msg_len - 2] = '\0';
//     memmove(src, src + msg_len, BUF_SIZE - msg_len);
//     *inbuf -= msg_len;
//     return 0;
// }

// int find_network_newline(const char *buf, int inbuf) {
//     for (int i = 0; i < inbuf - 1; i++) {
//         if (buf[i] == '\r' && buf[i+1] == '\n') {
//             return i+2;
//         }
//     }
//     return -1;
// }

// int remove_client(struct client_sock **curr, struct client_sock **clients) {
//     if (curr == NULL) { // Deleting NULL.
//         return 1;
//     } else if (clients == NULL) { // List of clients is empty.
//         return 1;
//     }
//     struct client_sock *iterator = *clients;
//     if ((*curr)->sock_fd == (*clients)->sock_fd) { // Case 1: Client to remove is at the top of the list.
//         *clients = (*clients)->next;
//         free(iterator);
//         *curr = *clients;
//         return 0;
//     }
//     else {
//         struct client_sock *tmp;
//         while (iterator->next != NULL) {
//             if ((iterator->next)->sock_fd == (*curr)->sock_fd) {
//                 tmp = *curr;
//                 // iterator->next = (iterator->next)->next;
//                 // *curr = iterator->next;
//                 *curr = (iterator->next)->next;
//                 iterator->next = *curr;
//                 free(tmp);
//                 return 0;
//             } iterator = iterator->next;
//         }
//     }   
//     return 1; // Couldn't find the client in the list, or empty list
// }

// int write_to_socket(int sock_fd, char *buffer, int length) {
//     ssize_t bytes_read = write(sock_fd, buffer, length);
//     while (length != 0 && bytes_read != 0) {
//         if (bytes_read == -1) {
//             if (errno == EPIPE) {
//                 display_error("ERROR: ", "Disconnected");
//                 return 2; // If errno is 2, then we are disconnected. Return 2.
//             } else if (errno == EINTR) {
//                 continue; // System call was interrupted by signal. We continue.
//             } 
//             display_error("ERROR: ", "Write to Socket");
//             return 1; 
//         }
//         buffer += bytes_read;
//         length -= bytes_read;
//     }
//     return 0;
// }