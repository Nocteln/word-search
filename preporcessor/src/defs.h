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


/**
 * @brief represents the result of image processing with word and grid detection
 */
struct process_result {
  struct img *img;                /**< processed image */
  struct box ***words_and_grid;   /**< array containing words and grid */
  int *words_length;              /**< length of each word */
  int width;                      /**< width of the grid */
  int length;                     /**< length/height of the grid */
  int nbwords;                    /**< number of words detected */
};

#endif
