#include "math_funcs.h"

double ID(double input)
{
    return input;
}

double ReLU(double input)
{
    return input > 0 ? input : 0;
}

double ReLUDerivative(double input)
{
    return input > 0 ? 1 : 0;
}

double LeakyReLU(double input)
{
    return input > 0 ? input : input / 10;
}

double LeakyReLUDerivative(double input)
{
    return input > 0 ? 1 : 0.1;
}

double cosh(double input)
{
    return (pow(EULER_NUMBER, 2 * input) + 1) / (2 * pow(EULER_NUMBER, input));
}

double Tanh(double input)
{
    return (pow(EULER_NUMBER, 2 * input) - 1) / (pow(EULER_NUMBER, 2 * input) + 1);
}

double TanhDerivative(double input)
{
    return 1 / (cosh(input) * cosh(input));
}

double Sigmoid(double input)
{
    return (1 / (1 + pow(EULER_NUMBER, -input)));
}

double SigmoidDerivative(double input)
{
    return Sigmoid(input) * (1 - Sigmoid(input));
}