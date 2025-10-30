/**
 * @file defs.h
 * @brief declaration of the structures
 */
#ifndef DEFS_H
#define DEFS_H



/**
 * @brief represents a bonding box inside an image.
 */
struct box {
  int min_x;
  int max_x;
  int min_y;
  int max_y;
};

struct word {
  int length;
  struct box *letter;
};

struct words_list {
  int length;
  struct word words;
};

struct grid {
  int width;
  int height;
  struct box **letter;
};

struct parsed_image {
  struct grid grid;
  struct words_list words_list;
};



/**
 * @brief represents an image.
 */
struct img {
  int width; /**< width of the image */
  int height; /**< height of the image */
  /**
   * min 3 for rgb. sometimes 4 for alpha.
   */
  int channels; /**< number of channels of a pixel */
  /**
   * each values is an unsigned char (so 0-255)
   * if you want to to access the pixel at (x,y):
   *
   * img.img[(y * img.width + x) * img.channels + 0] = red;
   * img.img[(y * img.width + x) * img.channels + 1] = green;
   * img.img[(y * img.width + x) * img.channels + 2] = blue;
   * img.img[(y * img.width + x) * img.channels + 3] = alpha; // (check if the channel is >4)
   */
  unsigned char *img; /**< array of the pixels */
};

#endif
