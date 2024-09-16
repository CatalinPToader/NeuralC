#include "network_loader.h"

#define BUFSZ 256
#define LAYERS_NUM 10

Activator createID() {
    Activator identical = { .ActivationFunction = ID, .DerivatedFunction = ID };
    return identical;
}

Activator createLeakyRELU() {
    Activator leaky = { .ActivationFunction = LeakyReLU, .DerivatedFunction = LeakyReLUDerivative };
    return leaky;
}

Activator createRELU() {
    Activator relu = { .ActivationFunction = ReLU, .DerivatedFunction = ReLUDerivative };
    return relu;
}

Activator createTanh() {
    Activator tanh = { .ActivationFunction = Tanh, .DerivatedFunction = TanhDerivative };
    return tanh;
}

Activator createSigmoid() {
    Activator sig = { .ActivationFunction = Sigmoid, .DerivatedFunction = SigmoidDerivative };
    return sig;
}

Activator get_activation_func_from_index(int index) {
    switch(index)
    {
        case 0:
            return createSigmoid();
        case 1:
            return createLeakyRELU();
        case 2:
            return createRELU();
        case 3:
            return createTanh();
        default:
            return createID();
    }
}

TupleLayer process_layer_line(char* string)
{
    TupleLayer returnVal;
    char* tok = strtok(string, " ");

    if (tok == NULL) {
        return returnVal;
    }

    returnVal.activator = get_activation_func_from_index(get_activation_func_index(tok));
    tok = strtok(NULL, " ");

    returnVal.neuron_count = retrieve_uint(tok);

    return returnVal;
}


Network* load(FILE *network_file)
{
    int check_mode = 0;
    char buffer[BUFSZ] = { 0 };
    int line_net_num = 0;
    int line_num = 0;


    int network_neurons[LAYERS_NUM] = { 0 };
    Activator network_activators[LAYERS_NUM];

    for (char* ret = fgets(buffer, BUFSZ, network_file); ret != NULL; ret = fgets(buffer, BUFSZ, network_file)) {
        line_num++;

        if (is_comment_or_empty(buffer, BUFSZ)) {
            if (is_entire_line(buffer, BUFSZ))
                continue;
            else {
                while (!is_entire_line(buffer, BUFSZ) && ret != NULL)
                    ret = fgets(buffer, BUFSZ, network_file);

                if (ret == NULL && !feof(network_file)) {
                    perror("File reading error: ");
                    return NULL;
                }

                continue;
            }
        }

        if (!is_entire_line(buffer, BUFSZ)) {
            printf("Line %d was over %d characters and not a comment! Exiting!\n", line_num, BUFSZ);
            return NULL;
        }

        if (line_net_num == 0) {
            int num_neurons = process_input_line(buffer, check_mode);
            network_neurons[line_net_num] = num_neurons;
            network_activators[line_net_num] = createID();
        } else {
            TupleLayer layer = process_layer_line(buffer);
            network_neurons[line_net_num] = layer.neuron_count;
            network_activators[line_net_num] = layer.activator;
        }

        line_net_num++;
    }

    return createNetwork(line_net_num, network_neurons, network_activators);
}