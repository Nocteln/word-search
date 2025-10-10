#ifndef SAVE_LOAD_H
#define SAVE_LOAD_H


int save_network(const char *filename, struct neural_network *nn);
struct neural_network *load_network(const char *filename);

#endif // !SAVE_LOAD_H
