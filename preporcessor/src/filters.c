#include "main.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>


void grayscale(struct img img) {
  for (int i = 0; i < img.width * img.height; i++) {
    int r = img.img[i * img.channels + 0];
    int g = img.img[i * img.channels + 1];
    int b = img.img[i * img.channels + 2];

    // https://en.wikipedia.org/wiki/Grayscale#Luma_coding_in_video_systems
    unsigned char gray = (unsigned char)(0.299 * r + 0.587 * g + 0.114 * b);

    img.img[i * img.channels + 0] = gray;
    img.img[i * img.channels + 1] = gray;
    img.img[i * img.channels + 2] = gray;
    if (img.channels==4)
      img.img[i * img.channels + 3] = 255;
  }
}


void generate_gaussian_kernel(float *kernel, int ksize, float sigma) {
    float sum = 0.0f;
    int half = ksize / 2;

    for (int y = -half; y <= half; y++) {
        for (int x = -half; x <= half; x++) {
            float exponent = -(x * x + y * y) / (2 * sigma * sigma);
            float value = expf(exponent) / (2 * M_PI * sigma * sigma);
            kernel[(y + half) * ksize + (x + half)] = value;
            sum += value;
        }
    }

    // Normalize kernel (so total weight = 1)
    for (int i = 0; i < ksize * ksize; i++) {
        kernel[i] /= sum;
    }
}

void gaussian_blur(struct img img, int ksize, float sigma) {
    int half = ksize / 2;
    float *kernel = malloc(ksize * ksize * sizeof(float));
    generate_gaussian_kernel(kernel, ksize, sigma);

    unsigned char *temp = malloc(img.width * img.height * img.channels);
    memcpy(temp, img.img, img.width * img.height * img.channels); // copy original

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            float sum[4] = {0, 0, 0, 0};

            for (int ky = -half; ky <= half; ky++) {
                for (int kx = -half; kx <= half; kx++) {
                    int ix = x + kx;
                    int iy = y + ky;

                    if (ix < 0) ix = 0;
                    if (iy < 0) iy = 0;
                    if (ix >= img.width) ix = img.width - 1;
                    if (iy >= img.height) iy = img.height - 1;

                    int srcIndex = (iy * img.width + ix) * img.channels;
                    float kvalue = kernel[(ky + half) * ksize + (kx + half)];

                    for (int c = 0; c < img.channels; c++) {
                        sum[c] += temp[srcIndex + c] * kvalue;
                    }
                }
            }

            int dstIndex = (y * img.width + x) * img.channels;
            for (int c = 0; c < img.channels; c++) {
                img.img[dstIndex + c] = (unsigned char)fminf(fmaxf(sum[c], 0.0f), 255.0f);
            }
        }
    }

    free(kernel);
    free(temp);
}


void threshold(unsigned char threshold_value, struct img img) {
  for (int i = 0; i < img.width * img.height; i++) {
    unsigned char pixel_value = img.img[i * img.channels + 0];
    unsigned char new_value = (pixel_value > threshold_value) ? 255 : 0;

    img.img[i * img.channels + 0] = new_value;
    img.img[i * img.channels + 1] = new_value;
    img.img[i * img.channels + 2] = new_value;
  }
}

void local_threshold(int window_size, int C, struct img img) {
    int half = window_size / 2;

    unsigned char *copy = malloc(img.width * img.height * img.channels);
    if (!copy) return;
    memcpy(copy, img.img, img.width * img.height * img.channels);

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            int sum = 0;
            int count = 0;

            for (int wy = -half; wy <= half; wy++) {
                for (int wx = -half; wx <= half; wx++) {
                    int nx = x + wx;
                    int ny = y + wy;

                    if (nx >= 0 && nx < img.width && ny >= 0 && ny < img.height) {
                        sum += copy[(ny * img.width + nx) * img.channels];
                        count++;
                    }
                }
            }

            unsigned char local_thresh = (sum / count) - C;
            unsigned char pixel_value = copy[(y * img.width + x) * img.channels];
            unsigned char new_value = (pixel_value > local_thresh) ? 255 : 0;

            img.img[(y * img.width + x) * img.channels + 0] = new_value;
            img.img[(y * img.width + x) * img.channels + 1] = new_value;
            img.img[(y * img.width + x) * img.channels + 2] = new_value;
        }
    }

    free(copy);
}


void erode(int kernel, struct img img) {
  unsigned char *temp = (unsigned char *)malloc(img.width * img.height * img.channels);
  memcpy(temp, img.img, img.width * img.height * img.channels);

  int kernel_size = kernel;
  int offset = kernel_size / 2;

  for (int y = offset; y < img.height - offset; y++) {
    for (int x = offset; x < img.width - offset; x++) {
      int min_value = 255;

      for (int ky = -offset; ky <= offset; ky++) {
        for (int kx = -offset; kx <= offset; kx++) {
          int px = x + kx;
          int py = y + ky;

          int pixel_value = temp[(py * img.width + px) * img.channels];

          if (pixel_value < min_value) {
            min_value = pixel_value;
          }
        }
      }

      img.img[(y * img.width + x) * img.channels + 0] = min_value;
      img.img[(y * img.width + x) * img.channels + 1] = min_value;
      img.img[(y * img.width + x) * img.channels + 2] = min_value;
    }
  }

  free(temp);
}

void dilate(int kernel, struct img img) {
  unsigned char *temp = (unsigned char *)malloc(img.width * img.height * img.channels);
  memcpy(temp, img.img, img.width * img.height * img.channels);

  int kernel_size = kernel;
  int offset = kernel_size / 2;

  for (int y = offset; y < img.height - offset; y++) {
    for (int x = offset; x < img.width - offset; x++) {
      int max_value = 0;

      for (int ky = -offset; ky <= offset; ky++) {
        for (int kx = -offset; kx <= offset; kx++) {
          int px = x + kx;
          int py = y + ky;

          int pixel_value = temp[(py * img.width + px) * img.channels];

          if (pixel_value > max_value) {
            max_value = pixel_value;
          }
        }
      }

      img.img[(y * img.width + x) * img.channels + 0] = max_value;
      img.img[(y * img.width + x) * img.channels + 1] = max_value;
      img.img[(y * img.width + x) * img.channels + 2] = max_value;
    }
  }

  free(temp);
}


void open_operation(int kernel, struct img img) {
  erode(kernel, img);
  dilate(kernel, img);
}
void close_operation(int kernel, struct img img) {
  dilate(kernel, img);
  erode(kernel, img);
}



float get_distance_between_color(
    unsigned char r1, unsigned char g1, unsigned char b1,
    unsigned char r2, unsigned char g2, unsigned char b2) {

  int dr = r2 - r1;
  int dg = g2 - g1;
  int db = b2 - b1;

  float distance = sqrt((dr * dr) + (dg * dg) + (db * db));

  return distance; // Distance between 0 and ~441.672943
}


void edge_detection(struct img img) {

  for (int y = 0; y < img.height - 1; ++y) {
    for (int x = 0; x < img.width - 1; ++x) {
      int index = (y * img.width + x) * img.channels;

      int bottom = ((y + 1) * img.width + x) * img.channels;
      int left = (y * img.width + x) * img.channels;
      int bottom_left = ((y + 1) * img.width + (x + 1)) * img.channels;

      unsigned char r = img.img[index];  
      unsigned char g = img.img[index + 1]; 
      unsigned char b = img.img[index + 2]; 

      float dist1 = get_distance_between_color(r, g, b, img.img[bottom], img.img[bottom + 1], img.img[bottom + 2]);
      float dist2 = get_distance_between_color(r, g, b, img.img[left], img.img[left + 1], img.img[left + 2]);
      float dist3 = get_distance_between_color(r, g, b, img.img[bottom_left], img.img[bottom_left + 1], img.img[bottom_left + 2]);

      float total_dist = (dist1 + dist2 + dist3) / 3.0f;

      if (total_dist > EDGE_DETECTION_THRESHOLD) {
        //img.img[index] = total_dist * 5;
        //img.img[index + 1] = total_dist * 5;
        //img.img[index + 2] = total_dist * 5;
        img.img[index] = 255;
        img.img[index + 1] = 255;
        img.img[index + 2] = 255;
      } else {
        img.img[index] = 0;
        img.img[index + 1] = 0;
        img.img[index + 2] = 0;
      }
    }
  }
}

void border(int border_width, int border_height, int r, int g, int b, struct img *img) {
    int new_width = img->width + 2 * border_width;
    int new_height = img->height + 2 * border_height;

    unsigned char *new_img = (unsigned char *)malloc(new_width * new_height * img->channels);
    
    for (int y = 0; y < new_height; ++y) {
        for (int x = 0; x < new_width; ++x) {
            int idx = (y * new_width + x) * img->channels;
            if (y < border_height || y >= new_height - border_height || x < border_width || x >= new_width - border_width) {
                new_img[idx + 0] = r;
                new_img[idx + 1] = g;
                new_img[idx + 2] = b;
                if (img->channels == 4) {
                    new_img[idx + 3] = 255;
                }
            } else {
                int original_idx = ((y - border_height) * img->width + (x - border_width)) * img->channels;
                new_img[idx + 0] = (img->img)[original_idx + 0];
                new_img[idx + 1] = (img->img)[original_idx + 1];
                new_img[idx + 2] = (img->img)[original_idx + 2];
                if (img->channels == 4) {
                    new_img[idx + 3] = (img->img)[original_idx + 3];
                }
            }
        }
    }

    free(img->img);
    img->img = new_img;

    img->width = new_width;
    img->height = new_height;
}


void rotate(int r, int g, int b, float rot, struct img *img) {
    int new_width, new_height;

    // Compute new dimensions to fit rotated image
    float cos_rot = fabsf(cos(rot));
    float sin_rot = fabsf(sin(rot));

    new_width  = (int)(img->height * sin_rot + img->width * cos_rot);
    new_height = (int)(img->height * cos_rot + img->width * sin_rot);

    // Allocate new image
    unsigned char *new_img = malloc(new_width * new_height * img->channels);
    if (!new_img) return;

    // Fill new image with RGB background
    for (int i = 0; i < new_width * new_height; i++) {
        new_img[i * img->channels + 0] = r;
        new_img[i * img->channels + 1] = g;
        new_img[i * img->channels + 2] = b;
        if (img->channels == 4)
          new_img[i * img->channels + 3] = 255;
    }

    // Coordinates of the center of original and new image
    float cx = img->width / 2.0f;
    float cy = img->height / 2.0f;
    float ncx = new_width / 2.0f;
    float ncy = new_height / 2.0f;

    // Iterate over each pixel in new image
    for (int y = 0; y < new_height; y++) {
        for (int x = 0; x < new_width; x++) {
            // Compute coordinates in original image using inverse rotation
            float tx = x - ncx;
            float ty = y - ncy;

            float ox =  cos(rot) * tx + sin(rot) * ty + cx;
            float oy = -sin(rot) * tx + cos(rot) * ty + cy;

            int ix = (int)roundf(ox);
            int iy = (int)roundf(oy);

            if (ix >= 0 && ix < img->width && iy >= 0 && iy < img->height) {
                for (int c = 0; c < img->channels; c++) {
                    new_img[(y * new_width + x) * img->channels + c] =
                        (img->img)[(iy * img->width + ix) * img->channels + c];
                }
            }
        }
    }

    // Replace old image with new one
    free(img->img);
    img->img = new_img;

    // Update globals
    img->width = new_width;
    img->height = new_height;
}

void scale(int factor, struct img *img) {
    if (factor <= 0) return;

    int new_width = img->width * factor;
    int new_height = img->height * factor;

    unsigned char *new_img = malloc(new_width * new_height * img->channels);
    if (!new_img) return;

    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            for (int dy = 0; dy < factor; dy++) {
                for (int dx = 0; dx < factor; dx++) {
                    int nx = x * factor + dx;
                    int ny = y * factor + dy;
                    for (int c = 0; c < img->channels; c++) {
                        new_img[(ny * new_width + nx) * img->channels + c] =
                            (img->img)[(y * img->width + x) * img->channels + c];
                    }
                }
            }
        }
    }

    free(img->img);
    img->img = new_img;

    img->width = new_width;
    img->height = new_height;
}
