#ifndef NET_CONF_HELP_H
#define NET_CONF_HELP_H
#include <errno.h>
#include <stdlib.h>
#include <string.h>

extern const char* const known_functions[];

int string_is_uint(char *string);
int retrieve_uint(char *string);

int is_activation_func(char *string);
int get_activation_func_index(char *string);
int is_correct_layer_line(char *string);
int process_input_line(char *string, int check);

int is_entire_line(char *string, int n);
int is_comment_or_empty(char *string, int n);

int is_whitespace(char c);
#endif
