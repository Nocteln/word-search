#include "main.h"
#include <math.h>
#include <stdlib.h>
#include "utils.h"

#include <stb_image.h>
#include <stb_image_write.h>
#include <string.h>

#include <pub_neural_network.h>

void make_box(int min_x, int min_y, int max_x, int max_y, int r, int g, int b, struct img img) {

  if (min_x > max_x) {
    int tmp = min_x;
    min_x = max_x;
    max_x = tmp;
  }

  if (min_y > max_y) {
    int tmp = min_y;
    min_y = max_y;
    max_y = tmp;
  }

  for (int x = min_x; x < max_x; x++) {
      img.img[((min_y) * img.width + x) * img.channels] = r;
      img.img[((min_y) * img.width + x) * img.channels +1] = g;
      img.img[((min_y) * img.width + x) * img.channels +2] = b;
  }
  for (int x = min_x; x < max_x; x++) {
      img.img[((max_y) * img.width + x) * img.channels] = r;
      img.img[((max_y) * img.width + x) * img.channels +1] = g;
      img.img[((max_y) * img.width + x) * img.channels +2] = b;
  }
  for (int y = min_y; y < max_y; y++) {
      img.img[((y) * img.width + min_x) * img.channels] = r;
      img.img[((y) * img.width + min_x) * img.channels +1] = g;
      img.img[((y) * img.width + min_x) * img.channels +2] = b;
  }
  for (int y = min_y; y <= max_y; y++) {
      img.img[((y) * img.width + max_x) * img.channels] = r;
      img.img[((y) * img.width + max_x) * img.channels +1] = g;
      img.img[((y) * img.width + max_x) * img.channels +2] = b;
  }

  //img[((max_y) * img.width + max_x) * img.channels] = r;
  //img[((max_y) * img.width + max_x) * img.channels +1] = g;
  //img[((max_y) * img.width + max_x) * img.channels +2] = b;
}




void mark_pixel(int x, int y, struct img img) {
  if (x < 1 || y < 1) return;
  if (x > img.width - 1 || y > img.height - 1) return;

  img.img[(y * img.width + x) * img.channels + 0] = 255;
  img.img[(y * img.width + x) * img.channels + 1] = 0;
  img.img[(y * img.width + x) * img.channels + 2] = 255;
}

int get_pixel_mark(int x, int y, struct img img) {
  if (x < 1 || y < 1) return 1;
  if (x > img.width - 1 || y > img.height - 1) return 1;

  return img.img[(y * img.width + x) * img.channels + 0] == 255;
}


void flood_aux(int x, int y, int *depth, struct box *f, struct img img) {

  if (get_pixel_mark(x, y, img)) return;

  mark_pixel(x, y, img);

  if (f->max_x == -1 || f->max_x < x) f->max_x = x;
  if (f->min_x == -1 || f->min_x > x) f->min_x = x;
  if (f->max_y == -1 || f->max_y < y) f->max_y = y;
  if (f->min_y == -1 || f->min_y > y) f->min_y = y;

  (*depth)--;
  if (*depth < 0) {
    return;
  }

  flood_aux(x + 1, y, depth,f, img);
  flood_aux(x - 1, y, depth,f, img);
  flood_aux(x, y + 1, depth,f, img);
  flood_aux(x, y - 1, depth,f, img);
  flood_aux(x + 1, y + 1, depth,f, img);
  flood_aux(x - 1, y - 1, depth,f, img);
  flood_aux(x + 1, y - 1, depth,f, img);
  flood_aux(x - 1, y + 1, depth,f, img);
}

struct box flood(int x, int y, int *depth, struct img img) {
  struct box res = {
    .max_x = -1,
    .min_x = -1,

    .max_y = -1,
    .min_y = -1,
  };

  flood_aux(x,y,depth,&res,img);

  return res;

}

void swap(float *a, float *b) {
    float tmp = *a;
    *a = *b;
    *b = tmp;
}

// --- Utility: simple in-place bubble sort (sufficient for small arrays)
void bubble_sort(float *arr, int n) {
    int i, j;
    for (i = 0; i < n - 1; i++) {
        for (j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                swap(&arr[j], &arr[j + 1]);
            }
        }
    }
}

/// Median
float median(float *arr, int n) {
    float *copy = (float *)malloc(n * sizeof(float));
    for (int i = 0; i < n; i++) copy[i] = arr[i];

    bubble_sort(copy, n);
    float med;
    if (n % 2 == 0) {
        med = (copy[n/2 - 1] + copy[n/2]) / 2.0f;
    } else {
        med = copy[n/2];
    }
    free(copy);
    return med;
}

/// Median Absolute Deviation (MAD)
float mad(float *arr, int n, float med) {
    float *dev = (float *)malloc(n * sizeof(float));
    for (int i = 0; i < n; i++) {
        dev[i] = fabsf(arr[i] - med);
    }
    float mad_val = median(dev, n);
    free(dev);
    return mad_val;
}

/// Main function: returns array of discard confidence scores
float *z_score_words_size(struct box *detections, int detections_size) {
    float *scores = (float *)malloc(detections_size * sizeof(float));
    if (!scores) return NULL;


    float *sizes = (float *)malloc(detections_size * sizeof(float));
    for (int i = 0; i < detections_size; i++) {
      struct box curr = detections[i];
      int dx = curr.max_x - curr.min_x;
      int dy = curr.max_y - curr.min_y;
      float d = sqrt((dy * dy) + (dx * dx));
      sizes[i] = d;
    }



    float med = median(sizes, detections_size);
    float mad_val = mad(sizes, detections_size, med);

    for (int i = 0; i < detections_size; i++) {
        float score = 0.0f;
        if (mad_val != 0.0f) {
            score = fabsf(sizes[i] - med) / mad_val;
        }

        scores[i] = score / (score + 1.0f);
    }
    free(sizes);

    return scores;
}


float *discard_words_by_size(struct box **detections, int *detections_size) {
  float *res = malloc((*detections_size) * sizeof(float));

  for (int i = 0; i < *detections_size; i++) {
    res[i] = 1;

    struct box curr = (*detections)[i];
    int dx = curr.max_x - curr.min_x;
    int dy = curr.max_y - curr.min_y;
    float d = sqrt((dy * dy) + (dx * dx));

    if (d > BOX_SIZE_THRESH_MAX || d < BOX_SIZE_THRESH_MIN) continue;
    res[i] = 0;
  }
  return res;
}

float *discard_words_by_overlap(struct box **detections, int *detections_size) {
  float *res = malloc((*detections_size) * sizeof(float));

  for (int i = 0; i < *detections_size; i++) {
    res[i] = 1;

    struct box curr = (*detections)[i];

    char overlap_another = 0;
    for (int j = 0; j < *detections_size; j++) {
      struct box currj = (*detections)[j];

      if (curr.max_x < currj.max_x && curr.min_x > currj.min_x && curr.max_y < currj.max_y && curr.min_y > currj.min_y) {
        overlap_another = 1;
      }
    }
    if (overlap_another) continue;
    
    res[i] = 0;
  }
  return res;
}


void cut_words(struct box **detections, int *detections_size, struct img img) {

  struct box *cdetections = malloc(0);
  int cdetections_size = 0;

  for (int i = 0; i < *detections_size; i++) {
    struct box curr = (*detections)[i];

    int cols = 0;
    int moy = 0;
    int max = 0;

    for (int x = curr.min_x; x <= curr.max_x; x++) {

      int cmax = 0;
      for (int y = curr.min_y; y <= curr.max_y; y++) {
        if (img.img[(y * img.width + x) * img.channels] > 50) {
          moy++;
          cmax++;
        }
      }
      if (cmax > max) max = cmax;
      cols++;

    }

    int curr_col = 0;
    int last_cut = 0;
    for (int x = curr.min_x; x <= curr.max_x; x++) {

      int n = 0;
      for (int y = curr.min_y; y <= curr.max_y; y++) {
        if (img.img[(y * img.width + x) * img.channels] > 50) {
          n++;
        }
      }
      curr_col++;

      int shade = ((float)n/(float)max) * 255;

      //int res = 255;
      if (curr_col - last_cut > THRESHOLD_FOR_TAILING_WORD_CUTTING && cols - curr_col > THRESHOLD_FOR_TAILING_WORD_CUTTING) {
          if (shade > THRESHOLD_PIXELS_FOR_WORD_CUTTING) {
            struct box box = { .min_x = curr.min_x + last_cut, .max_x = curr.min_x + curr_col, .min_y = curr.min_y, .max_y = curr.max_y };
            push_box_array(&cdetections, box, &cdetections_size);
            last_cut = curr_col;
            //res = 0;
          }
      }
      /*
      img[((curr.min_y -2) * img.width + x) * img.channels] = shade;
      img[((curr.min_y -2) * img.width + x) * img.channels + 1] = 0;
      img[((curr.min_y -2) * img.width + x) * img.channels + 2] = 0;

      img[((curr.min_y -4) * img.width + x) * img.channels] = res;
      img[((curr.min_y -4) * img.width + x) * img.channels + 1] = 0;
      img[((curr.min_y -4) * img.width + x) * img.channels + 2] = 0;
      */
    }
    struct box box = { .min_x = curr.min_x + last_cut, .max_x = curr.min_x + curr_col, .min_y = curr.min_y, .max_y = curr.max_y };
    push_box_array(&cdetections, box, &cdetections_size);
  }
  free(*detections);

  *detections = cdetections;
  *detections_size = cdetections_size;
}

void filter_out_on_tresh(struct box **detections, int *detections_size, float *certainty, float tresh) {
  struct box *fdetections = malloc(0);
  int fdetections_size = 0;

  for (int i = 0; i < *detections_size; i++) {
    float c = certainty[i];

    if (c < tresh)
      push_box_array(&fdetections, (*detections)[i], &fdetections_size);
  }

  free(*detections);

  *detections = fdetections;
  *detections_size = fdetections_size;
}


struct img get_sub_image(struct box box, struct img img) {
  int dx = box.max_x - box.min_x;
  int dy = box.max_y - box.min_y;

  unsigned char *res_img = malloc(dx * dy * img.channels * sizeof(unsigned char));

  for (int x = 0; x < dx; x++) {
    for (int y = 0; y < dy; y++) {
        res_img[(y * dx + x) * img.channels+0] = img.img[((y + box.min_y) * img.width + (x + box.min_x)) * img.channels+0];
        res_img[(y * dx + x) * img.channels+1] = img.img[((y + box.min_y) * img.width + (x + box.min_x)) * img.channels+1];
        res_img[(y * dx + x) * img.channels+2] = img.img[((y + box.min_y) * img.width + (x + box.min_x)) * img.channels+2];
        if (img.channels == 4)
          res_img[(y * dx + x) * img.channels+3] = img.img[((y + box.min_y) * img.width + (x + box.min_x)) * img.channels+3];
      
    }
  }

  struct img nimg;

  nimg.channels = img.channels;
  nimg.width = dx;
  nimg.height = dy;
  nimg.img = res_img;

  return nimg;
}

void save_img(const char *output_path, struct img img) {
  if (stbi_write_png(output_path, img.width, img.height, img.channels, img.img, img.width * img.channels)) {
    //printf("img saved to %s\n", output_path);
  } else {
    printf("faield to save %s\n", output_path);
  }
}

void save_sub_image(const char *savepath, struct box box, struct img img) {
  struct img res_img = get_sub_image(box,img);
  
  save_img(savepath, res_img);

  free(res_img.img);
}


struct img *cpyimg(struct img img) {
  struct img *res = malloc(sizeof(struct img));

  res->width = img.width;
  res->height = img.height;
  res->channels = img.channels;

  unsigned char *res_img =  malloc(img.width * img.height * img.channels * sizeof(unsigned char));
  memcpy(res_img, img.img, img.width * img.height * img.channels * sizeof(unsigned char));
  
  res->img = res_img;
  return res;
}




// calc the entropy of the double array after doing a softmax on it.
float confidence(double *outp, int size) {
    float max = 0.0f;

    for (int i = 0; i < size; i++) {
      if (max < outp[i]) max = outp[i];
    }

    return max;
}


float *get_nn_confidence(struct img img, struct box *detections, int detections_size) {
    float *scores = (float *)malloc(detections_size * sizeof(float));
    if (!scores) return NULL;

    double out[64 * 64];
    nn_handle n = load_network("./neural_network/network.bin");

    float *sizes = (float *)malloc(detections_size * sizeof(float));
    for (int i = 0; i < detections_size; i++) {
      struct box curr = detections[i];
      save_sub_image("tmp.png", curr, img);
      image_to_double64("tmp.png", out);
      double *outp = calculate_neural_network_outputs(n, out);

      scores[i] = confidence(outp, 26);


      free(outp);
    }

    return scores;
}


// TODO: dirty implementation
void cut_words_based_on_letter_confidence(struct box **detections, int *detections_size, struct img img, float *confidence) {

  int i = 0;
  int j = 0;
  int xacc = 0;
  int yacc = 0;
  for(; i < *detections_size; i++) {
    if (confidence[i] > CONFIDENCE_THRESHOLD_TO_LABEL_AS_LETTER) {
      xacc += (*detections)[i].max_x - (*detections)[i].min_x;
      yacc += (*detections)[i].max_y - (*detections)[i].min_y;
      j++;
    }
  }

  int avg_width_of_letter = xacc/j;
  int avg_height_of_letter = yacc/j;



  struct box *cdetections = malloc(0);
  int cdetections_size = 0;



  for(int i = 0; i < *detections_size; i++) {
    struct box curr = (*detections)[i];

    int width = curr.max_x - curr.min_x;
    int height = curr.max_y - curr.min_y;

    // expectable height for a letter
    if (abs(height - avg_height_of_letter) > LETTER_VARIATION_TOLERANCE) {
      //push_box_array(&cdetections, curr, &cdetections_size);
      continue;
    }


    //if (confidence[i] < 0.1) {
    //  push_box_array(&cdetections, curr, &cdetections_size);
    //  continue;
    //}


    int curr_col = avg_width_of_letter;
    int last_cut = 0;
    for (int x = curr.min_x + avg_width_of_letter; x <= curr.max_x; x += avg_width_of_letter) {
        struct box box = { .min_x = curr.min_x + last_cut, .max_x = curr.min_x + curr_col, .min_y = curr.min_y, .max_y = curr.max_y };
        push_box_array(&cdetections, box, &cdetections_size);
        last_cut = curr_col;
    }
    struct box box = { .min_x = curr.min_x + last_cut, .max_x = curr.min_x + curr_col, .min_y = curr.min_y, .max_y = curr.max_y };
    push_box_array(&cdetections, box, &cdetections_size);
  }
  free(*detections);

  *detections = cdetections;
  *detections_size = cdetections_size;
}
