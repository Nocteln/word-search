#ifndef PROCESS_H
#define PROCESS_H

#include "defs.h"

void remove_salt_and_pepper(struct img im, int radius);
void smooth_jagged_edges(struct img *im, int radius);
void process_image(struct img *img);
struct process_result *process_image_with_data(struct img *img);

#endif
