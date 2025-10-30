#ifndef MAIN_H
#define MAIN_H

#include "defs.h"


extern const int BOX_SIZE_THRESH_MAX;
extern const int BOX_SIZE_THRESH_MIN;
extern const float EDGE_DETECTION_THRESHOLD;
extern const int THRESHOLD_VALUE_FOR_1ST_TRESH_FUNC;


extern const int THRESHOLD_FOR_TAILING_WORD_CUTTING;
extern const int THRESHOLD_PIXELS_FOR_WORD_CUTTING;

extern const float CONFIDENCE_THRESHOLD_TO_LABEL_AS_LETTER;
extern const int LETTER_VARIATION_TOLERANCE;



extern const int DUMP_IMAGES;


void push_box_array(struct box **array, struct box box, int *size);


#endif
