#ifndef LEARN_H
#define LEARN_H

double get_point_loss(struct neural_network *n, double *inputs, double *expected);
double get_dataset_loss(struct neural_network *n, double **inputs, double **expected, int dataset_size);
void apply_gradients_to_layer(struct layer *l, double learn_rate);
void learn_one_epoch_using_discrete_method(struct neural_network *n, double **inputs, double **expected, int dataset_size, double learn_rate);
void learn_unit_with_backpropagation(struct neural_network *n, double *inputs, double *expected, double learn_rate);
void learn_epoch_with_backpropagation(struct neural_network *n, double **inputs, double **expected, int dataset_size, double learn_rate);

#endif // !LEARN_H
