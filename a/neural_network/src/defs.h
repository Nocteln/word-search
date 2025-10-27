#ifndef DEFS_H
#define DEFS_H


struct neural_network {
  int number_of_layers;
  double (*activation_function)(double);
  double (*node_loss)(double, double);
  double (*activation_function_d)(double);
  double (*node_loss_d)(double, double);
  struct layer *layers;
};

struct layer {
  struct neural_network *n;
  int nodes_in;
  int nodes_out;
  double **weights;
  double *biases;

  double **loss_gradient_weights;
  double *loss_gradient_biases;

  double *nodes_val_dump;
  double *deltas;
};

enum activation_type {
    ACTIVATION_SIGMOID = 0,
    // ACTIVATION_RELU    = 1,
    // ACTIVATION_TANH    = 2,
};

enum loss_type {
    LOSS_MSE  = 0,
    // LOSS_CROSS_ENTROPY = 1,
};


#endif // !DEFS_H
