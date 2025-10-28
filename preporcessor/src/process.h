#ifndef PROCESS_H
#define PROCESS_H

#include "defs.h"

void process_image(struct img *img);
struct process_result *process_image_with_data(struct img *img);

#endif
