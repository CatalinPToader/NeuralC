#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>

#define EULER_NUMBER 2.71828


typedef enum neuron_type NType;
typedef struct neuron Neuron;
typedef struct layer Layer;
typedef struct network Network;

enum neuron_type{Input, Hidden, Output};

struct neuron {
	NType type;
	int numberInputs;
	double *input_weights;
	Layer *prev_layer;
	double (*ActivationFunction)(double inputValue);
	double inputValue;
	double activatedValue;
	double bias;
};

struct layer {
	int num_nodes;
	double *costGradientBias;
	double **costGradientWeights;
	Neuron** neurons;
	double *costActivationNode;
};

struct network {
	int numberLayers;
	int *layerSizes;
	Layer** layers;
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
		sum += neuron->prev_layer->neurons[i]->activatedValue * neuron->input_weights[i];
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

Neuron* createNeuron(int num_inputs, Layer *prev_layer, NType type, double (*ActivationFunction)(double inputValue)) {
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

Layer *createLayer(int num_neurons, int prev_layer_size, Layer *prev_layer, NType type, double (*ActivationFunction)(double inputValue)) {
	Layer *layer = (Layer *)malloc(sizeof(Layer) * num_neurons);
	if (!layer)
		return NULL;

	layer->num_nodes = num_neurons;

	layer->costGradientBias = (double *)malloc(sizeof(double) * num_neurons);
	if (!layer->costGradientBias)
		goto cleanup_layer;

	layer->costActivationNode = (double *)malloc(sizeof(double) * num_neurons);
	if (!layer->costActivationNode)
		goto cleanup_bias;

	layer->costGradientWeights = (double **)malloc(sizeof(double *) * num_neurons);
	if (!layer->costGradientWeights)
		goto cleanup_activation;

	int i;
	for(i = 0; i < num_neurons; i++) {
		layer->costGradientWeights[i] = (double *)malloc(sizeof(double) * prev_layer_size);

		if (!layer->costGradientWeights[i])
			goto cleanup_weights;
	}

	layer->neurons = (Neuron **)malloc(sizeof(Neuron *) * num_neurons);
	if (!layer->neurons)
		goto cleanup_weights;
	
	for(i = 0; i < num_neurons; i++) {
		layer->neurons[i] = createNeuron(prev_layer_size, prev_layer, type, ActivationFunction);
		if (!layer->neurons[i]) {
			goto cleanup_neurons;
		}
	}

	return layer;

cleanup_neurons:
	for(i--; i >= 0; i--) {
		freeNeuron(layer->neurons[i]);
	}
	free(layer->neurons);
cleanup_weights:
	for(i--; i >= 0; i--) {
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

void freeLayer(Layer *l) {
	for(int i = 0; i < l->num_nodes; i++) {
		freeNeuron(l->neurons[i]);
	}
	free(l->neurons);
	for(int i = 0; i < l->num_nodes; i++) {
		free(l->costGradientWeights[i]);
	}
	free(l->costGradientWeights);
	free(l->costGradientBias);
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

	network->layers = (Layer **)malloc(sizeof(Layer *) * num_layers);
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
		freeLayer(network->layers[i]);
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
		freeLayer(n->layers[i]);
	}

	free(n->layers);
	free(n->layerSizes);
	free(n);
}

double costDerivative(double expectation, double output) {
	return 2 * (expectation - output);
}

double cost(int num_outs, double* expectation, double *outputs) {
	double cost = 0;

	for(int i = 0; i < num_outs; i++) {
		double err = outputs[i] - expectation[i];
		cost += err * err;
	}

	return cost;
}

void printNetworkOutputs(Network *n) {
	printf("Network Output: ");
	int layer = n->numberLayers - 1;
	for(int i = 0; i < n->layerSizes[layer]; i++) {
		printf("%lf ", n->layers[layer]->neurons[i]->activatedValue);
	}
	printf("\n");
}

void main() {
	srand(time(NULL));

	Network *network = createNetwork(3, 1, 5, 2);


	for(int run = 0; run < 6; run++) {

		network->layers[0]->neurons[0]->inputValue = run;

		double expectation[2] = {run % 2, run % 2};

		for(int i = 0; i < network->numberLayers; i++) {
			for(int j = 0; j < network->layerSizes[i]; j++) {
				calculateInputValue(network->layers[i]->neurons[j]);
				activateNeuron(network->layers[i]->neurons[j]);
			}
		}

		printNetworkOutputs(network);

		double costs[2] = {0, 0};

		for(int i = 0; i < 2; i++) {
			costs[i] = network->layers[network->numberLayers - 1]->neurons[i]->activatedValue;
		}

		printf("Cost: %lf\n", cost(2, expectation, costs));
	}

	freeNetwork(network);
}