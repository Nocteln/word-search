#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "defs.h"
#include "activation_functions.h"
#include "neural_network.h"
#include "learn.h"
#include "nodes_loss.h"

//#define DEBUG_LAYER_ACTIVATION

int main()
{
  double **inputs = malloc(sizeof(double*) * 4);
  double **expect = malloc(sizeof(double*) * 4);

  inputs[0] = (double[]){1.,1.};
  expect[0] = (double[]){0.};

  inputs[1] = (double[]){1.,0.};
  expect[1] = (double[]){1.};
  
  inputs[2] = (double[]){0.,1.};
  expect[2] = (double[]){1.};
  
  inputs[3] = (double[]){0.,0.};
  expect[3] = (double[]){0.};


  int layers[] = { 2,3,2 };
  struct neural_network *n = create_neural_network(layers, 3,
      &sigmoid_af, &mean_squared_error_w_constant_nl,
      &sigmoid_af_d, &mean_squared_error_nl_d);
  srandom(time(NULL));
  fill_random_neural_network(n);

  //print_neural_network(n);

  int epoch = 100000;
  printf("training for %d epoches...\n\n", epoch);

  double lr = 0.5;
  for (int i = 0; i < epoch; i++) {
    learn_epoch_with_backpropagation(n, inputs, expect, 4, lr);
  }
  //print_neural_network(n);

  double loss = get_dataset_loss(n, inputs, expect, 4);
  printf("global loss : %f\n\n",loss);

  double *a = calculate_neural_network_outputs(n, inputs[0]);
  printf("output for 1 xor 1 :  %f\n",*a);
  
  a = calculate_neural_network_outputs(n, inputs[1]);
  printf("output for 1 xor 0 :  %f\n",*a);

  a = calculate_neural_network_outputs(n, inputs[2]);
  printf("output for 0 xor 1 :  %f\n",*a);

  a = calculate_neural_network_outputs(n, inputs[3]);
  printf("output for 0 xor 0 :  %f\n",*a);

  drop_neural_network();
  return 0;
}
