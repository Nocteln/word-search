#ifndef PUB_NEURAL_NETWORK_H
#define PUB_NEURAL_NETWORK_H

typedef struct neural_network * nn_handle;


nn_handle load_network(const char *filename);

void image_to_double64(const char *input_path, double out[64 * 64]);
double *calculate_neural_network_outputs(nn_handle n, double *inputs);

#endif // !PUB_NEURAL_NETWORK_H
