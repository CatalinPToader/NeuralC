#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFSZ 256

const char* const known_functions[] = { "sigmoid", "leakyrelu", "relu", "tanh", "identical", 0 };

int is_activation_func(char* string)
{
    for (int i = 0; known_functions[i] != 0; i++) {
        if (strcmp(string, known_functions[i]) == 0)
            return 1;
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

int is_correct_input_line(char* string)
{
    char* tok = strtok(string, " ");

    if (tok == NULL) {
        return 0;
    }

    if (strcmp(tok, "IN") == 0) {
        tok = strtok(NULL, " ");

        return string_is_uint(tok);
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

int main(int argc, char** argv)
{
    if (argc < 2) {
        printf("Usage: ./network_config_checker <network_config_file>\n");
        return -1;
    }

    FILE* network = fopen(argv[1], "r+");
    if (network == NULL) {
        perror("File handling error: ");
        return -1;
    }

    char buffer[BUFSZ] = { 0 };
    int line_net_num = 0;
    int line_num = 0;

    for (char* ret = fgets(buffer, BUFSZ, network); ret != NULL; ret = fgets(buffer, BUFSZ, network)) {
        line_num++;

        if (is_comment_or_empty(buffer, BUFSZ)) {
            if (is_entire_line(buffer, BUFSZ))
                continue;
            else {
                while (!is_entire_line(buffer, BUFSZ) && ret != NULL)
                    ret = fgets(buffer, BUFSZ, network);

                if (ret == NULL && !feof(network)) {
                    perror("File reading error: ");
                    return -1;
                }

                continue;
            }
        }

        if (!is_entire_line(buffer, BUFSZ)) {
            printf("%s:%d was over %d characters and not a comment! Exiting!\n", argv[1], line_num, BUFSZ);
            return -1;
        }

        if (line_net_num == 0) {
            if (!is_correct_input_line(buffer)) {
                printf("%s:%d expected input neurons, format: IN <num_neurons>!\n", argv[1], line_num);
                return -1;
            }
        } else {
            if (!is_correct_layer_line(buffer)) {
                printf("%s:%d expected hidden/output layer, format: <activation_func> <num_neurons>!\n", argv[1], line_num);
                return -1;
            }
        }

        line_net_num++;
    }

    fclose(network);

    if (feof(network)) {
        printf("%s is a valid network configuration file!\n", argv[1]);
        return 0;
    } else {
        perror("Error while reading file: ");
        return -1;
    }
}
