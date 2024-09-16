#include "neuron.h"

void genWeights(int num_weights, double* arr)
{
    double scale = 1.0 / sqrt(num_weights);

    for (int i = 0; i < num_weights; i++)
        arr[i] = scale * (rand() % 10000 - 5000) / 10000;
}

int allocInputs(Neuron* neuron, int number_inputs)
{
    neuron->numberInputs = number_inputs;

    neuron->input_weights = (double*)malloc(sizeof(double) * number_inputs);
    if (!neuron->input_weights) {
        return 0;
    }

    return 1;
}

Neuron* createNeuron(int num_inputs, Layer* prev_layer, NType type, Activator activator)
{
    Neuron* neuron = (Neuron*)malloc(sizeof(Neuron));
    if (!neuron)
        return NULL;

    if (!allocInputs(neuron, num_inputs)) {
        free(neuron);
        return NULL;
    }
    genWeights(num_inputs, neuron->input_weights);

    neuron->activator = activator;
    neuron->prev_layer = prev_layer;
    neuron->type = type;
    neuron->bias = 0.001;

    return neuron;
}

void freeNeuron(Neuron* n)
{
    free(n->input_weights);
    free(n);
}

Layer* createLayer(int num_neurons, int prev_layer_size, Layer* prev_layer, NType type, Activator activator)
{
    Layer* layer = (Layer*)malloc(sizeof(Layer) * num_neurons);
    if (!layer)
        return NULL;

    layer->num_nodes = num_neurons;
    layer->type = type;

    layer->costGradientBias = (double*)calloc(num_neurons, sizeof(double));
    if (!layer->costGradientBias)
        goto cleanup_layer;

    layer->costActivationNode = (double*)calloc(num_neurons, sizeof(double));
    if (!layer->costActivationNode)
        goto cleanup_bias;

    layer->costGradientWeights = (double**)malloc(sizeof(double*) * num_neurons);
    if (!layer->costGradientWeights)
        goto cleanup_activation;

    int i;
    for (i = 0; i < num_neurons; i++) {
        layer->costGradientWeights[i] = (double*)calloc(prev_layer_size, sizeof(double));

        if (!layer->costGradientWeights[i])
            goto cleanup_weights;
    }

    layer->neurons = (Neuron**)malloc(sizeof(Neuron*) * num_neurons);
    if (!layer->neurons)
        goto cleanup_weights;

    for (i = 0; i < num_neurons; i++) {
        layer->neurons[i] = createNeuron(prev_layer_size, prev_layer, type, activator);
        if (!layer->neurons[i]) {
            goto cleanup_neurons;
        }
    }

    return layer;

cleanup_neurons:
    for (i--; i >= 0; i--) {
        freeNeuron(layer->neurons[i]);
    }
    free(layer->neurons);
cleanup_weights:
    for (i--; i >= 0; i--) {
        free(layer->costGradientWeights[i]);
    }
    free(layer->costGradientWeights);
cleanup_activation:
    free(layer->costActivationNode);
cleanup_bias:
    free(layer->costGradientBias);
cleanup_layer:
    free(layer);

    return NULL;
}

void freeLayer(Layer* l)
{
    for (int i = 0; i < l->num_nodes; i++) {
        freeNeuron(l->neurons[i]);
    }
    free(l->neurons);
    for (int i = 0; i < l->num_nodes; i++) {
        free(l->costGradientWeights[i]);
    }
    free(l->costGradientWeights);
    free(l->costActivationNode);
    free(l->costGradientBias);
    free(l);
}

Network* createNetwork(int num_layers, int* layersNeuronCounts, Activator* layersActivators)
{
    int i = 0;
    Network* network = (Network*)malloc(sizeof(Network));
    if (!network)
        return NULL;

    network->numberLayers = num_layers;
    network->layerSizes = (int*)malloc(sizeof(int) * num_layers);
    if (!network->layerSizes) {
        goto cleanup_network;
    }

    network->layers = (Layer**)malloc(sizeof(Layer*) * num_layers);
    if (!network->layers) {
        goto cleanup_sizes;
    }

    NType type;

    for (; i < num_layers; i++) {
        if (i == 0)
            type = Input;
        else if (i != num_layers - 1)
            type = Hidden;
        else
            type = Output;

        int layer_size = layersNeuronCounts[i];
        Activator activator = layersActivators[i];

        if (i == 0) {
            network->layers[i] = createLayer(layer_size, 0, NULL, type, activator);
        } else {
            network->layers[i] = createLayer(layer_size, network->layerSizes[i - 1], network->layers[i - 1], type, activator);
        }
        if (!network->layers[i]) {
            goto cleanup_layers;
        }

        network->layerSizes[i] = layer_size;
    }

    return network;

cleanup_layers:
    for (i--; i >= 0; i--) {
        freeLayer(network->layers[i]);
    }

    free(network->layers);
cleanup_sizes:
    free(network->layerSizes);
cleanup_network:
    free(network);
    return NULL;
}

Network* va_createNetwork(int num_layers, ...)
{
    int i = 0;
    Network* network = (Network*)malloc(sizeof(Network));
    if (!network)
        return NULL;

    network->numberLayers = num_layers;
    network->layerSizes = (int*)malloc(sizeof(int) * num_layers);
    if (!network->layerSizes) {
        goto cleanup_network;
    }

    network->layers = (Layer**)malloc(sizeof(Layer*) * num_layers);
    if (!network->layers) {
        goto cleanup_sizes;
    }

    // Declaring pointer to the
    // argument list
    va_list ptr;

    // Initializing argument to the
    // list pointer
    va_start(ptr, num_layers);
    NType type;

    for (; i < num_layers; i++) {
        if (i == 0)
            type = Input;
        else if (i != num_layers - 1)
            type = Hidden;
        else
            type = Output;

        // Accessing current variable
        // and pointing to next one
        int layer_size = va_arg(ptr, int);
        Activator activator = va_arg(ptr, Activator);

        if (i == 0) {
            network->layers[i] = createLayer(layer_size, 0, NULL, type, activator);
        } else {
            network->layers[i] = createLayer(layer_size, network->layerSizes[i - 1], network->layers[i - 1], type, activator);
        }
        if (!network->layers[i]) {
            va_end(ptr);
            goto cleanup_layers;
        }

        network->layerSizes[i] = layer_size;
    }

    // Ending argument list traversal
    va_end(ptr);

    return network;

cleanup_layers:
    for (i--; i >= 0; i--) {
        freeLayer(network->layers[i]);
    }

    free(network->layers);
cleanup_sizes:
    free(network->layerSizes);
cleanup_network:
    free(network);
    return NULL;
}

void freeNetwork(Network* n)
{
    for (int i = 0; i < n->numberLayers; i++) {
        freeLayer(n->layers[i]);
    }

    free(n->layers);
    free(n->layerSizes);
    free(n);
}

void calculateInputValue(Neuron* neuron)
{
    if (neuron->type == Input)
        return;

    double sum = neuron->bias;

    for (int i = 0; i < neuron->numberInputs; i++) {
        sum += neuron->prev_layer->neurons[i]->activatedValue * neuron->input_weights[i];
    }

    neuron->inputValue = sum;
}

void activateNeuron(Neuron* neuron)
{
    neuron->activatedValue = neuron->activator.ActivationFunction(neuron->inputValue);
}

void runNetwork(Network* n, DataPoint input)
{
    for (int i = 0; i < n->layerSizes[0]; i++) {
        n->layers[0]->neurons[i]->inputValue = input.input[i];
    }

    for (int i = 0; i < n->numberLayers; i++) {
        for (int j = 0; j < n->layerSizes[i]; j++) {
            calculateInputValue(n->layers[i]->neurons[j]);
            activateNeuron(n->layers[i]->neurons[j]);
        }
    }
}

double cost(int num_outs, double* expectation, double* outputs)
{
    double cost = 0;

    for (int i = 0; i < num_outs; i++) {
        double err = outputs[i] - expectation[i];
        cost += err * err;
    }

    return 0.5 * cost;
}

double costDerivative(double expectation, double output)
{
    return output - expectation;
}

void calculateCostActivation(Layer* layer, Layer* next_layer, double* expectation)
{
    if (layer->type == Input)
        return;

    if (layer->type == Output) {
        for (int i = 0; i < layer->num_nodes; i++) {
            Neuron* neuron = layer->neurons[i];
            double costNode = costDerivative(expectation[i], neuron->activatedValue);
            layer->costActivationNode[i] = costNode * neuron->activator.DerivatedFunction(neuron->inputValue);
        }
    } else {
        for (int i = 0; i < layer->num_nodes; i++) {
            Neuron* neuron = layer->neurons[i];

            double costNode = 0;
            for (int j = 0; j < next_layer->num_nodes; j++) {
                double weightOut = next_layer->neurons[j]->input_weights[i];
                costNode += weightOut * next_layer->costActivationNode[j];
            }

            layer->costActivationNode[i] = costNode * neuron->activator.DerivatedFunction(neuron->inputValue);
        }
    }
}

void calculateGradients(Layer* layer, Layer* prev_layer)
{
    for (int i = 0; i < layer->num_nodes; i++) {
        for (int j = 0; j < layer->neurons[i]->numberInputs; j++)
            layer->costGradientWeights[i][j] += prev_layer->neurons[j]->activatedValue * layer->costActivationNode[i];
        layer->costGradientBias[i] += 1 * layer->costActivationNode[i];
    }
}

void updateAllGradients(Network* network, DataPoint data)
{
    runNetwork(network, data);
    for (int i = network->numberLayers - 1; i > 0; i--) {
        if (i == network->numberLayers - 1)
            calculateCostActivation(network->layers[i], NULL, data.expectation);
        else
            calculateCostActivation(network->layers[i], network->layers[i + 1], data.expectation);
        calculateGradients(network->layers[i], network->layers[i - 1]);
    }
}

void applyGradient(Layer* layer, double learn_rate)
{
    for (int i = 0; i < layer->num_nodes; i++) {
        for (int j = 0; j < layer->neurons[i]->numberInputs; j++)
            layer->neurons[i]->input_weights[j] -= layer->costGradientWeights[i][j] * learn_rate;
        layer->neurons[i]->bias -= layer->costGradientBias[i] * learn_rate;
    }
}

void applyAllGradients(Network* network, double learn_rate)
{
    for (int i = network->numberLayers - 1; i > 0; i--)
        applyGradient(network->layers[i], learn_rate);
}

void resetGradients(Layer* layer)
{
    for (int i = 0; i < layer->num_nodes; i++) {
        layer->costGradientBias[i] = 0;
        for (int j = 0; j < layer->neurons[i]->numberInputs; j++)
            layer->costGradientWeights[i][j] = 0;
    }
}

void resetAllGradients(Network* network)
{
    for (int i = network->numberLayers - 1; i > 0; i--) {
        Layer* layer = network->layers[i];
        resetGradients(layer);
    }
}

void learn(Network* n, int num_data_points, DataPoint* data, double learn_rate)
{
    for (int i = 0; i < num_data_points; i++) {
        updateAllGradients(n, data[i]);
    }

    applyAllGradients(n, learn_rate / num_data_points);

    resetAllGradients(n);
}

double getNetworkCost(Network* n, DataPoint* data, int num_points)
{
    int num_outs = n->layers[n->numberLayers - 1]->num_nodes;
    double* output = (double*)malloc(sizeof(double) * num_outs);
    if (!output)
        return -1;

    double total_cost = 0;
    for (int i = 0; i < num_points; i++) {
        runNetwork(n, data[i]);
        for (int j = 0; j < num_outs; j++) {
            output[j] = n->layers[n->numberLayers - 1]->neurons[j]->activatedValue;
        }

        total_cost += cost(num_outs, data[i].expectation, output);
    }
    free(output);

    return total_cost;
}

void printNetworkCost(Network* n, DataPoint* data, int num_points)
{
    printf("Cost: %lf\n", getNetworkCost(n, data, num_points));
}

void printNetworkOutputs(Network* n)
{
    printf("Network Output: ");
    int layer = n->numberLayers - 1;
    for (int i = 0; i < n->layerSizes[layer]; i++) {
        printf("%lf ", n->layers[layer]->neurons[i]->activatedValue);
    }
    printf("\n");
}

void save_network(Network* n, char* name, int name_len, unsigned int epoch)
{
    FILE* ptr;

    char epoch_str[name_len + 20];
    sprintf(epoch_str, "%s-%d", name, epoch);
    ptr = fopen(epoch_str, "w");
}

// TODO: Data Loader (create array of size 10, double when full using realloc, read line by line)
// TODO: Network Loader (use network checker code for inspiration)

int main()
{
    srand(time(NULL));

    Activator identical = { .ActivationFunction = ID, .DerivatedFunction = ID };
    Activator sig = { .ActivationFunction = Sigmoid, .DerivatedFunction = SigmoidDerivative };
    Activator relu = { .ActivationFunction = ReLU, .DerivatedFunction = ReLUDerivative };
    Activator leakyrelu = { .ActivationFunction = LeakyReLU, .DerivatedFunction = LeakyReLUDerivative };
    Activator tanh = { .ActivationFunction = Tanh, .DerivatedFunction = TanhDerivative };

    int data_points = 4;
    DataPoint* data = (DataPoint*)malloc(sizeof(DataPoint) * data_points);

    for (int i = 0; i < data_points; i++) {
        data[i].input = (double*)malloc(sizeof(double) * 2);
        data[i].expectation = (double*)malloc(sizeof(double) * 2);

        int cp = i % 4;

        data[i].input[0] = cp / 2;
        data[i].input[1] = cp % 2;
        data[i].expectation[0] = (cp % 2) == (cp / 2) ? 0.0 : 1.0;
    }

    //Network* network = va_createNetwork(4, 2, identical, 4, sig, 2, leakyrelu, 1, leakyrelu);

    FILE* network_f = fopen("../examples/example.network", "r+");
    if (network_f == NULL) {
        perror("File handling error: ");
        return -1;
    }

    Network* network = load(network_f);

    unsigned int epoch = 0;
    double last_cost = 1.0;
    int same_cost = 0;
    while (last_cost > 1e-6 && same_cost < 1e6) {
        double learn_rate = 0.5 * log(same_cost + 10) / log10(epoch + 10);
        learn(network, data_points, data, learn_rate);
        printf("EPOCH %d, LEARN RATE %lf\n", epoch, learn_rate);
        printNetworkCost(network, data, data_points);
        epoch++;
        if (last_cost == getNetworkCost(network, data, data_points)) {
            same_cost++;
        } else {
            same_cost = 0;
        }
        last_cost = getNetworkCost(network, data, data_points);
    }

    for (int i = 0; i < data_points; i++) {
        runNetwork(network, data[i]);
        printf("Input: x=%lf y=%lf\n", data[i].input[0], data[i].input[1]);
        printf("\t\tXOR\n");
        printNetworkOutputs(network);
    }

    /*
            runNetwork(network, data[data_points - 542]);
            printf("Input: x=%lf y=%lf\n", data[data_points - 542].input[0], data[data_points - 542].input[1]);
            printf("\t\tINSIDE\t OUTSIDE CIRCLE\n");
            printNetworkOutputs(network);
    */
    printNetworkCost(network, data, data_points - 1);

    for (int i = 0; i < data_points; i++) {
        free(data[i].input);
        free(data[i].expectation);
    }

    free(data);

    freeNetwork(network);
}
