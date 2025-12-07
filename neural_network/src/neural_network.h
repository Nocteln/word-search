#ifndef NEURAL_NETWORK_H
#define NEURAL_NETWORK_H

struct neural_network *create_neural_network(int *layers_size, int size,
    double (*activation_function)(double),
    double (*output_activation_function)(double),
    double (*node_loss)(double, double),
    double (*activation_function_d)(double),
    double (*output_activation_function_d)(double),
    double (*node_loss_d)(double, double)
    );

void fill_random_layer(struct layer *l);
void fill_random_neural_network(struct neural_network *n);

double *calculate_layer_outputs(struct layer *l, double *inputs);
double *calculate_neural_network_outputs(struct neural_network *n, double *inputs);
int classify(struct neural_network *n, double *inputs);

void drop_neural_network();

void print_layer_with_dump(struct layer *l);
void print_neural_network_with_dump(struct neural_network *n);
void print_layer(struct layer *l);
void print_neural_network(struct neural_network *n);

#endif // !NEURAL_NETWORK_H
