#include <sys/stat.h>
#include <string.h>
#include <math.h>
#include <time.h>
//#define STB_IMAGE_IMPLEMENTATION
//#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image.h>
#include <stb_image_write.h>
#include <stdio.h>
#include <stdlib.h>

#include "main.h"
#include "filters.h"
#include "utils.h"
#include "defs.h"
#include "process.h"

const int BOX_SIZE_THRESH_MAX = 300;
const int BOX_SIZE_THRESH_MIN = 10;
const float EDGE_DETECTION_THRESHOLD = 8.0;
const int THRESHOLD_VALUE_FOR_1ST_TRESH_FUNC = 230;

const int THRESHOLD_FOR_TAILING_WORD_CUTTING = 30;
const int THRESHOLD_PIXELS_FOR_WORD_CUTTING = 250;



const float CONFIDENCE_THRESHOLD_TO_LABEL_AS_LETTER = 0.8;
const int LETTER_VARIATION_TOLERANCE = 20;

const int DUMP_IMAGES = 0;


void push_box_array(struct box **array, struct box box, int *size) {
  *size +=1;
  void *tmp = realloc(*array, (*size) * sizeof(struct box));
  if (tmp == NULL) {
    printf("AAAA\n");
  }
  *array = tmp;
  (*array)[*size-1] = box;
}

void push_vec2_array(int **array, int x, int y, int *size) {
  *size +=2;
  *array = realloc(*array, *size * sizeof(int) * 2);
  (*array)[*size-1] = y;
  (*array)[*size-2] = x;
}


int main(int argc, char *argv[]) {
  if (argc < 2) return 1;

  const char *input_path = argv[1];

  int width, height, channels;
  unsigned char *img;

  img = stbi_load(input_path, &width, &height, &channels, 0);
  if (img == NULL) {
    printf("Can't load image (%s)\n", input_path);
    return 1;
  }

  printf("img loaded w %d channels. (%d x %d)\n", channels, width, height);


  struct img *image = malloc(sizeof(struct img));

  *image = (struct img){ .width = width, .height = height, .channels = channels, .img = img };


  process_image(image);

  return 0;

}
