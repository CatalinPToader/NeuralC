#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define EULER_NUMBER 2.71828

typedef struct neuron Neuron;
typedef Neuron** Layer;
typedef enum neuron_type NType;

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
	double sum = neuron->bias;

	for(int i = 0; i < neuron->numberInputs; i++) {
		sum += neuron->prev_layer[i]->activatedValue * neuron->input_weights[i];
	}

	neuron->inputValue = sum;
}

int allocInputs(Neuron *neuron, int number_inputs) {
	neuron->numberInputs = number_inputs;

	neuron->prev_layer= (Layer)malloc(sizeof(Neuron *) * number_inputs);
	if (!neuron->prev_layer)
		return 1;

	neuron->input_weights = (double *)malloc(sizeof(double) * number_inputs);
	if (!neuron->input_weights) {
		free(neuron->prev_layer);
		return 1;
	}

	return 0;
}

void main() {
	Neuron testNeuron1;

	testNeuron1.type = Input;
	testNeuron1.ActivationFunction = ReLU;
	testNeuron1.inputValue = 0.05;

	Neuron testNeuron2;
	testNeuron2.type = Output;
	testNeuron2.ActivationFunction = sigmoid;
	testNeuron2.bias = 0.05;
	
	if (allocInputs(&testNeuron2, 1) != 0) {
		printf("Bruh");
		return;
	}

	testNeuron2.prev_layer[0] = &testNeuron1;
	testNeuron2.input_weights[0] = 1;

	activateNeuron(&testNeuron1);
	calculateInputValue(&testNeuron2);
	activateNeuron(&testNeuron2);

	printf("%lf %lf\n", testNeuron2.activatedValue, testNeuron2.inputValue);
}