#ifndef PROCESS_H
#define PROCESS_H

#include "defs.h"
#include "rotation.h"

void remove_salt_and_pepper(struct img im, int radius);
void smooth_jagged_edges(struct img *im, int radius);
double deg_to_rad(int deg);
void process_image(struct img *img, char *path);
double rad_rotation(struct box *rois,int i,int j);
double rad_rotation2(struct box i,struct box j);
struct process_result *process_image_with_data(struct img *img, char *path);

#endif
