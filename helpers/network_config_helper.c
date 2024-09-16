#include "network_config_helper.h"

const char* const known_functions[] = { "sigmoid", "leakyrelu", "relu", "tanh", "identical", 0 };

int is_activation_func(char* string)
{
    for (int i = 0; known_functions[i] != 0; i++) {
        if (strcmp(string, known_functions[i]) == 0)
            return 1;
    }

    return 0;
}

int get_activation_func_index(char *string)
{
    for (int i = 0; known_functions[i] != 0; i++) {
        if (strcmp(string, known_functions[i]) == 0)
            return i;
    }

    return 0;
}

int string_is_uint(char* string)
{
    char* endptr = NULL;

    strtoul(string, &endptr, 10);
    if (endptr == string)
        return 0;
    if (errno != 0)
        return 0;

    return 1;
}

int retrieve_uint(char *string)
{
    char* endptr = NULL;

    unsigned int val = strtoul(string, &endptr, 10);
    if (endptr == string)
        return 0;
    if (errno != 0)
        return 0;

    return val; // Converts to int, not an issue unless 2B input neurons or more
}

int is_correct_layer_line(char* string)
{
    char* tok = strtok(string, " ");

    if (tok == NULL) {
        return 0;
    }

    if (is_activation_func(tok)) {
        tok = strtok(NULL, " ");

        return string_is_uint(tok);
    }

    return 0;
}

int process_input_line(char *string, int check)
{
    char* tok = strtok(string, " ");

    if (tok == NULL) {
        return 0;
    }

    if (strcmp(tok, "IN") == 0) {
        tok = strtok(NULL, " ");

        return check ? string_is_uint(tok) : retrieve_uint(tok);
    }

    return 0;
}

int is_whitespace(char c)
{
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

int is_entire_line(char* string, int n)
{
    for (int i = 0; i < n; i++)
        if (string[i] == '\n')
            return 1;
        else if (string[i] == 0)
            return 0;

    return 0;
}

int is_comment_or_empty(char* string, int n)
{
    for (int i = 0; i < n; i++)
        if (is_whitespace(string[i]) || string[i] == 0)
            continue;
        else
            return string[i] == '#';
    return 1;
}
