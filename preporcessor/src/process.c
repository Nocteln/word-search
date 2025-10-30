#include <sys/stat.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <stb_image.h>
#include <stb_image_write.h>
#include <stdio.h>
#include <stdlib.h>

#include "main.h"
#include "filters.h"
#include "utils.h"
#include "defs.h"


void add_vec2_array(int **array, int x, int y, int size) {
  *array = realloc(*array, size * 2 * sizeof(int));
  (*array)[(size*2)-1] = y;
  (*array)[(size*2)-2] = x;
}

void mark_pixell(int x, int y, struct img *img) {

  img->img[(y * img->width + x) * img->channels + 0] = 255;
  img->img[(y * img->width + x) * img->channels + 1] = 0;
  img->img[(y * img->width + x) * img->channels + 2] = 255;
}

void remove_salt_and_pepper(struct img im, int radius) {
    int w = im.width;
    int h = im.height;
    int ch = im.channels;
    unsigned char *src = im.img;

    unsigned char *copy = malloc(w * h * ch);
    if (!copy) return;
    memcpy(copy, src, w * h * ch);

    for (int y = radius; y < h - radius; ++y) {
        for (int x = radius; x < w - radius; ++x) {
            int white_count = 0, black_count = 0;

            for (int dy = -radius; dy <= radius; ++dy) {
                for (int dx = -radius; dx <= radius; ++dx) {
                    unsigned char v = copy[((y + dy) * w + (x + dx)) * ch];
                    if (v > 127) white_count++;
                    else black_count++;
                }
            }

            unsigned char new_val = (white_count > black_count) ? 255 : 0;

            src[(y * w + x) * ch + 0] = new_val;
            src[(y * w + x) * ch + 1] = new_val;
            src[(y * w + x) * ch + 2] = new_val;
        }
    }

    free(copy);

}

void smooth_jagged_edges(struct img *im, int radius) {
    int w = im->width;
    int h = im->height;
    int ch = im->channels;
    unsigned char *src = im->img;

    unsigned char *dst = malloc(w * h * ch);
    if (!dst) return;
    memcpy(dst, src, w * h * ch);

    for (int y = radius; y < h - radius; ++y) {
        for (int x = radius; x < w - radius; ++x) {
            int white_count = 0, total_count = 0;

            for (int dy = -radius; dy <= radius; ++dy) {
                for (int dx = -radius; dx <= radius; ++dx) {
                    unsigned char v = src[((y + dy) * w + (x + dx)) * ch];
                    if (v > 127) white_count++;
                    total_count++;
                }
            }

            int pixel_index = (y * w + x) * ch;
            float density = (float)white_count / total_count;

            unsigned char new_val;
            if (src[pixel_index] > 127) {
                new_val = (density < 0.5f) ? 0 : 255;
            } else {
                new_val = (density > 0.5f) ? 255 : 0;
            }

            dst[pixel_index + 0] = new_val;
            dst[pixel_index + 1] = new_val;
            dst[pixel_index + 2] = new_val;
        }
    }

    memcpy(src, dst, w * h * ch);
    free(dst);
}

struct img *process_image_aux(struct img *img) {

  grayscale(*img);
  border(10, 10, 255,255,255, img);

  //rotate(255,255,255,-0.14*M_PI, img);

  //local_threshold(9,12,*img);
  local_threshold(9,2,*img);
  //threshold(150,*img);

  smooth_jagged_edges(img, 1);
  remove_salt_and_pepper(*img, 1);


  
  scale(3,img);
  //gaussian_blur(*img, 3, 7.);


  erode(3,*img);
  dilate(3,*img);


  save_img("interm/before_noise_rm.png", *img);

  
  /*
  struct img *img_edge = cpyimg(*img);
  edge_detection(*img_edge);
  save_img("interm/edge_detection.png", *img_edge);

  free(img_edge->img);
  free(img_edge);
  */


  struct img *flood_img = cpyimg(*img);

  struct box *rois = malloc(0);
  int *rois_start_pos = malloc(0);
  int rois_size = 0;

  for (int y = 0; y < img->height - 1; ++y) {
    for (int x = 0; x < img->width - 1; ++x) {
      if (flood_img->img[(y * flood_img->width + x) * flood_img->channels] == 255) continue;
      int depth = 100000;
      struct box box = flood(x+1, y+1, &depth, *flood_img);
      if (box.max_x != -1) {
        push_box_array(&rois, box, &rois_size);

        add_vec2_array(&rois_start_pos, x, y, rois_size);
      }
    }
  }
  free(flood_img->img);
  free(flood_img);




  float *cl;

  //cut_words(&rois, &rois_size, *img);

  //cl = get_nn_confidence(*img, rois, rois_size);
  //cut_words_based_on_letter_confidence(&rois, &rois_size, *img, cl);

  cl = z_score_words_size(rois, rois_size);
  filter_out_on_tresh(&rois, &rois_size, cl, 0.95);

  //cut_words(&rois, &rois_size, *img);


  if (0) {
    char buffer[100];
    srand(time(NULL));
    int dump_int = rand();

    sprintf(buffer, "dump%d", dump_int);
    printf("DUMP FOLDER:\n");
    printf("dump%d\n", dump_int);
    mkdir(buffer, 0777);

    for (int i = 0; i < rois_size; i++) {
      struct box curr = rois[i];

      sprintf(buffer, "dump%d/%d.png",dump_int, rand());


      struct box roi_to_save = {
        .min_x = curr.min_x-1,
        .min_y = curr.min_y-1,
        .max_x = curr.max_x+1,
        .max_y = curr.max_y+2 };
      save_sub_image(buffer, roi_to_save, *img);
    }
  }

  /*


     cl = discard_words_by_overlap(&rois, &rois_size);
  //filter_out_on_tresh(&detections, &detections_size, cl, 0.95);
  free(cl);

  cl = z_score_words_size(rois, rois_size);
  filter_out_on_tresh(&rois, &rois_size, cl, 0.95);
  free(cl);


*/

  for (int i = 0; i < rois_size; i++) {
    struct box curr = rois[i];
     make_box(curr.min_x-1, curr.min_y-1, curr.max_x+1, curr.max_y+1, 255,0,0, *img);
  }

  save_img("output.png", *img);

  //save_img("output.png", *img);

  return img;
}

void process_image(struct img *img) {
  img = process_image_aux(img);

  free(img->img);
  free(img);
}
