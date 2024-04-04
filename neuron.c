#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>

#define EULER_NUMBER 2.71828


typedef enum neuron_type NType;
typedef struct neuron Neuron;
typedef Neuron** Layer;
typedef struct network Network;

enum neuron_type{Input, Hidden, Output};

struct neuron {
	NType type;
	int numberInputs;
	double *input_weights;
	Layer prev_layer;
	double (*ActivationFunction)(double inputValue);
	double inputValue;
	double activatedValue;
	double bias;
};

struct network {
	int numberLayers;
	int *layerSizes;
	Layer* layers;
};

double ID(double input) {
	return input;
}

double ReLU(double input) {
	return input > 0 ? input : 0;
}

double sigmoid(double input) {
    return (1 / (1 + pow(EULER_NUMBER, -input)));
}

void activateNeuron(Neuron *neuron) {
	neuron->activatedValue = neuron->ActivationFunction(neuron->inputValue);
}

void calculateInputValue(Neuron *neuron) {
	if (neuron->type == Input)
		return;

	double sum = neuron->bias;

	for(int i = 0; i < neuron->numberInputs; i++) {
		sum += neuron->prev_layer[i]->activatedValue * neuron->input_weights[i];
	}

	neuron->inputValue = sum;
}

void genWeights(int num_weights, double *arr) {
	double sum = 0;

	for(int i = 0; i < num_weights; i++) {
		arr[i] = rand() % 100;
		sum += arr[i];
	}

	for(int i = 0; i < num_weights; i++) {
		arr[i] /= sum;
	}
}

int allocInputs(Neuron *neuron, int number_inputs) {
	neuron->numberInputs = number_inputs;

	neuron->input_weights = (double *)malloc(sizeof(double) * number_inputs);
	if (!neuron->input_weights) {
		return 1;
	}

	return 0;
}

Neuron* createNeuron(int num_inputs, Layer prev_layer, NType type, double (*ActivationFunction)(double inputValue)) {
	Neuron *neuron = (Neuron *)malloc(sizeof(Neuron));
	if (!neuron)
		return NULL;

	if (allocInputs(neuron, num_inputs) != 0) {
		free(neuron);
		return NULL;
	}
	genWeights(num_inputs, neuron->input_weights);

	neuron->ActivationFunction = ActivationFunction;
	neuron->prev_layer = prev_layer;
	neuron->type = type;
	neuron->bias = 0;
}

void freeNeuron(Neuron *n) {
	free(n->input_weights);
	free(n);
}

Layer createLayer(int num_neurons, int prev_layer_size, Layer prev_layer, NType type, double (*ActivationFunction)(double inputValue)) {
	Layer layer = (Layer)malloc(sizeof(Neuron *) * num_neurons);
	if (!layer)
		return NULL;

	int i = 0;
	for(; i < num_neurons; i++) {
		layer[i] = createNeuron(prev_layer_size, prev_layer, type, ActivationFunction);
		if (!layer[i]) {
			goto cleanup;
		}
	}

	return layer;

cleanup:
	for(i--; i >= 0; i--) {
		freeNeuron(layer[i]);
	}
	free(layer);

	return NULL;
}

void freeLayer(Layer l, int l_size) {
	for(int i = 0; i < l_size; i++) {
		freeNeuron(l[i]);
	}
	free(l);
}

Network* createNetwork(int num_layers, ...)
{
	int i = 0;
	Network *network = (Network *)malloc(sizeof(Network));
	if (!network)
		return NULL;

	network->numberLayers = num_layers;
	network->layerSizes = (int *)malloc(sizeof(int) * num_layers);
	if (!network->layerSizes) {
		goto cleanup_network;
	}

	network->layers = (Layer *)malloc(sizeof(Layer) * num_layers);
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

		if (i == 0) {
			network->layers[i] = createLayer(layer_size, 0, NULL, type, ID);
		} else {
			network->layers[i] = createLayer(layer_size, network->layerSizes[i-1], network->layers[i-1], type, sigmoid);
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
	for(i--; i >= 0; i--) {
		freeLayer(network->layers[i], network->layerSizes[i]);
	}

	free(network->layers);
cleanup_sizes:
	free(network->layerSizes);
cleanup_network:
	free(network);
    return NULL;
}

void freeNetwork(Network *n) {
	for(int i = 0; i < n->numberLayers; i++) {
		freeLayer(n->layers[i], n->layerSizes[i]);
	}

	free(n->layers);
	free(n->layerSizes);
	free(n);
}

void print_network_outputs(Network *n) {
	printf("Network Output: ");
	int layer = n->numberLayers - 1;
	for(int i = 0; i < n->layerSizes[layer]; i++) {
		printf("%lf ", n->layers[layer][i]->activatedValue);
	}
	printf("\n");
}

void main() {
	srand(time(NULL));

	Network *network = createNetwork(3, 1, 5, 2);

	network->layers[0][0]->inputValue = 1;

	for(int i = 0; i < network->numberLayers; i++) {
		for(int j = 0; j < network->layerSizes[i]; j++) {
			calculateInputValue(network->layers[i][j]);
			activateNeuron(network->layers[i][j]);
		}
	}

	print_network_outputs(network);

	freeNetwork(network);
}