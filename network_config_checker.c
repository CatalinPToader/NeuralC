#include <stdio.h>
#include "helpers/network_config_helper.h"

#define BUFSZ 256

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

    int check_mode = 1;

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
            if (!process_input_line(buffer, check_mode)) {
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
