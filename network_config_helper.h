#include <errno.h>
#include <stdlib.h>
#include <string.h>
#ifndef NET_CONF_HELP_H
#define NET_CONF_HELP_H

extern const char* const known_functions[];

int is_activation_func(char *string);
int is_correct_layer_line(char *string);
int is_correct_input_line(char *string);
int string_is_uint(char *string);

int is_entire_line(char *string, int n);
int is_comment_or_empty(char *string, int n);

int is_whitespace(char c);
#endif
