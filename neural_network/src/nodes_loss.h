#ifndef NODES_LOSS
#define NODES_LOSS

double mse_nl(double output_activation, double expected_output);
double mse_nl_d(double output_activation, double expected_output);

double mse_w_constant_nl(double output_activation, double expected_output);

double abs_nl(double output_activation, double expected_output);

#endif // !NODES_LOSS
