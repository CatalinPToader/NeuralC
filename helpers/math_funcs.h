#ifndef NET_MATH_HELP_H
#define NET_MATH_HELP_H
#include <math.h>

#define EULER_NUMBER 2.71828
#define PI 3.141592

double ID(double input);
double ReLU(double input);
double ReLUDerivative(double input);
double LeakyReLU(double input);
double LeakyReLUDerivative(double input);
double Sigmoid(double input);
double SigmoidDerivative(double input);
double Tanh(double input);
double TanhDerivative(double input);
#endif