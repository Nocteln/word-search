#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "defs.h"

struct neural_network *create_neural_network(int *layers_size, int size,
    double (*activation_function)(double), double (*node_loss)(double, double),
    double (*activation_function_d)(double), double (*node_loss_d)(double, double)) {


  struct neural_network *res = malloc(sizeof(struct neural_network));
  struct layer *layers = malloc(sizeof(struct layer) * (size - 1));

  res->activation_function = activation_function;
  res->node_loss = node_loss;
  res->activation_function_d = activation_function_d;
  res->node_loss_d = node_loss_d;

  res->layers = layers;
  res->number_of_layers = size - 1;

  for (int i = 0; i < size - 1; i++) {
    int nodes_in = layers_size[i];
    int nodes_out = layers_size[i+1];

    double *biases = malloc(sizeof(double) * nodes_out);
    double **weights = malloc(sizeof(double*) * nodes_in);
    for (int j = 0; j < nodes_in; j++) {
      weights[j] = malloc(sizeof(double) * nodes_out);
    }

    double *gradient_biases = malloc(sizeof(double) * nodes_out);
    double **gradient_weights = malloc(sizeof(double*) * nodes_in);
    for (int j = 0; j < nodes_in; j++) {
      gradient_weights[j] = malloc(sizeof(double) * nodes_out);
    }

    double *deltas = malloc(sizeof(double) * nodes_out);

    res->layers[i] = (struct layer){
      .nodes_in = nodes_in,
        .n = res,
        .nodes_out = nodes_out,
        .biases = biases,
        .weights = weights,

        .loss_gradient_weights = gradient_weights,
        .loss_gradient_biases = gradient_biases,
        .nodes_val_dump = NULL,
        .deltas = deltas,
    };
  }

  return res;
}

void fill_random_layer(struct layer *l) {
  double limit = sqrt(6.0 / (l->nodes_in + l->nodes_out));
  for(int i = 0; i < l->nodes_out; i++) {
    l->biases[i] = 0;
  }
  for(int i = 0; i < l->nodes_in; i++) {
    for(int j = 0; j < l->nodes_out; j++) {
      l->weights[i][j] = ((double)rand() / RAND_MAX) * 2 * limit - limit;
    }
  }
}

void fill_random_neural_network(struct neural_network *n) {
  for(int i = 0; i < n->number_of_layers; i++) {
    fill_random_layer(&n->layers[i]);
  }
}

void print_layer_with_dump(struct layer *l) {
  printf(" dump : \n   ");
  for(int i = 0; i < l->nodes_out; i++) {
    printf("%f ", l->nodes_val_dump[i]);
  }
  printf("\n biases : \n   ");
  for(int i = 0; i < l->nodes_out; i++) {
    printf("%f ", l->biases[i]);
  }
  printf("\n weights : \n");
  for(int i = 0; i < l->nodes_in; i++) {
    printf("  from %d:\n    ", i);
    for(int j = 0; j < l->nodes_out; j++) {
      printf("%f ", l->weights[i][j]);
    }
    printf("\n");
  }
  printf("\n");
}

void print_neural_network_with_dump(struct neural_network *n) {
  printf("---- Neural Network S ----\n\n");
  printf("Number of inputs : %d\n\n", n->layers[0].nodes_in);
  for(int i = 0; i < n->number_of_layers; i++) {
    printf("Layer %d (%d):\n", i, n->layers[i].nodes_out);
    print_layer_with_dump(&n->layers[i]);
  }
  printf("---- Neural Network E ----\n");
}

void print_layer(struct layer *l) {
  printf(" biases : \n   ");
  for(int i = 0; i < l->nodes_out; i++) {
    printf("%f ", l->biases[i]);
  }
  printf("\n weights : \n");
  for(int i = 0; i < l->nodes_in; i++) {
    printf("  from %d:\n    ", i);
    for(int j = 0; j < l->nodes_out; j++) {
      printf("%f ", l->weights[i][j]);
    }
    printf("\n");
  }
  printf("\n");
}

void print_neural_network(struct neural_network *n) {
  printf("---- Neural Network S ----\n\n");
  printf("Number of inputs : %d\n\n", n->layers[0].nodes_in);
  for(int i = 0; i < n->number_of_layers; i++) {
    printf("Layer %d (%d):\n", i, n->layers[i].nodes_out);
    print_layer(&n->layers[i]);
  }
  printf("---- Neural Network E ----\n");
}

double *calculate_layer_outputs(struct layer *l, double *inputs) {
  double *activations = malloc(sizeof(double) * l->nodes_out);


  for (int i = 0; i < l->nodes_out; i++) {
    double weighted_input = l->biases[i];
    for (int j = 0; j < l->nodes_in; j++) {
      weighted_input += inputs[j] * l->weights[j][i];
    }
    activations[i] = l->n->activation_function(weighted_input);
#ifdef DEBUG_LAYER_ACTIVATION
    printf("%f ", activations[i]);
#endif
  }
#ifdef DEBUG_LAYER_ACTIVATION
  printf("\n");
#endif

  return activations;
}

double *calculate_neural_network_outputs(struct neural_network *n, double *inputs) {

  double *res = inputs;

#ifdef DEBUG_LAYER_ACTIVATION
  printf("\n");
#endif

  for(int i = 0; i < n->number_of_layers; i++) {

#ifdef DEBUG_LAYER_ACTIVATION
    if (i == n->number_of_layers-1) {
      printf("layer out : ");
    } else {
      printf("layer %d : ", i);
    }
#endif

    double *tmp = calculate_layer_outputs(&n->layers[i], res);
    if (i > 0) {
      free(res);
    }
    res = tmp;
  }
  return res;
}

// get prediction of the nn
int classify(struct neural_network *n, double *inputs) {
  double *res = calculate_neural_network_outputs(n, inputs);
  int max = 0;
  for (int i = 1; i < n->layers[n->number_of_layers-1].nodes_out; i++) {
    if (res[max] < res[i]) {
      max = i;
    }
  }
  free(res);
  return max;
}

void drop_neural_network() {
  // drop... flemme
  // TODO
}

