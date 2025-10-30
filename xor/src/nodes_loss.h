#ifndef NODES_LOSS
#define NODES_LOSS

double mean_squared_error_nl(double output_activation, double expected_output);
double mean_squared_error_nl_d(double output_activation, double expected_output);

double mean_squared_error_w_constant_nl(double output_activation, double expected_output);

double abs_nl(double output_activation, double expected_output);

#endif // !NODES_LOSS
