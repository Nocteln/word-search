#ifndef ACTIVATION_FUNCTIONS_H
#define ACTIVATION_FUNCTIONS_H

double simple_af(double weighted_input);
double step_af(double weighted_input);
double sigmoid_af(double weighted_input);
double sigmoid_af_d(double weighted_input);
double relu_af(double weighted_input);
double relu_af_d(double weighted_input);

#endif // !ACTIVATION_FUNCTIONS_H
