#ifndef ROTATION_H
#define ROTATION_H

#include "defs.h"
#include "main.h"

unsigned char toGray(unsigned char r, unsigned char g, unsigned char b);
unsigned char computeOtsu(unsigned char *gray, int total);
void verticalMorph(unsigned char *img, int w, int h, int size);
float detectAngle(unsigned char *img, int w, int h);
int deg_rotation(struct img *img);

#endif