#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define EULER_NUMBER 2.71828
#define PI 3.141592

typedef enum neuron_type NType;
typedef struct neuron Neuron;
typedef struct layer Layer;
typedef struct network Network;
typedef struct activation Activator;
typedef struct datapoint DataPoint;

enum neuron_type {
    Input,
    Hidden,
    Output
};

struct datapoint {
    double* input;
    double* expectation;
};

struct activation {
    double (*ActivationFunction)(double inputValue);
    double (*DerivatedFunction)(double inputValue);
};

struct neuron {
    NType type;
    int numberInputs;
    double* input_weights;
    Layer* prev_layer;
    Activator activator;
    double inputValue;
    double activatedValue;
    double bias;
};

struct layer {
    NType type;
    int num_nodes;
    double* costGradientBias;
    double** costGradientWeights;
    Neuron** neurons;
    double* costActivationNode;
};

struct network {
    int numberLayers;
    int* layerSizes;
    Layer** layers;
};

void genWeights(int num_weights, double* arr);
int allocInputs(Neuron* neuron, int number_inputs);
Neuron* createNeuron(int num_inputs, Layer* prev_layer, NType type, Activator activator);
void freeNeuron(Neuron* n);
Layer* createLayer(int num_neurons, int prev_layer_size, Layer* prev_layer, NType type, Activator activator);
void freeLayer(Layer* l);
Network* createNetwork(int num_layers, ...);
void freeNetwork(Network* n);

double ID(double input);
double ReLU(double input);
double ReLUDerivative(double input);
double LeakyReLU(double input);
double LeakyReLUDerivative(double input);
double Sigmoid(double input);
double SigmoidDerivative(double input);
double Tanh(double input);
double TanhDerivative(double input);

void calculateInputValue(Neuron* neuron);
void activateNeuron(Neuron* neuron);
void runNetwork(Network* n, DataPoint input);

double cost(int num_outs, double* expectation, double* outputs);
double costDerivative(double expectation, double output);

void calculateCostActivation(Layer* layer, Layer* next_layer, double* expectation);
void calculateGradients(Layer* layer, Layer* prev_layer);
void updateAllGradients(Network* network, DataPoint data);
void applyGradient(Layer* layer, double learn_rate);
void applyAllGradients(Network* network, double learn_rate);
void resetGradients(Layer* layer);
void resetAllGradients(Network* network);

void learn(Network* n, int num_data_points, DataPoint* data, double learn_rate);

double getNetworkCost(Network* n, DataPoint* data, int num_points);
void printNetworkCost(Network* n, DataPoint* data, int num_points);

void printNetworkOutputs(Network* n);
