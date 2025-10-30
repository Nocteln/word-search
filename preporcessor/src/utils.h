/**
 * @file utils.h
 * @brief contains all utilitary functions.
 */
#ifndef UTILS_H
#define UTILS_H
#include "defs.h"



/**
 * @brief gives the bounding box of the image element (destructive)
 *
 * @param img image struct
 * @param x starting point of the search in x
 * @param y starting point of the search in y
 * @param depth maximal depth
 */
struct box flood(int x, int y, int *depth, struct img img);

/**
 * @brief draw the border of the bounding box
 *
 * @param img image struct
 * @param r red channel of the drawn pixels
 * @param g green channel of the drawn pixels
 * @param b blue channel of the drawn pixels
 * @param min_x bounding box min_x
 * @param min_y bounding box min_y
 * @param max_x bounding box max_x
 * @param max_y bounding box max_y
 */
void make_box(int min_x, int min_y, int max_x, int max_y, int r, int g, int b, struct img img);

/**
 * @brief Median Absolute Deviation (MAD) but return confidence
 *
 * @param detections
 * @param detections_size
 */
float *z_score_words_size(struct box *detections, int detections_size);

/**
 * @brief discard bounding boxes by size
 *
 * remove all the bounding boxes that have a size (distance of the +/+ to the -/-)
 * greater than BOX_SIZE_THRESH_MAX or lower than BOX_SIZE_THRESH_MIN
 *
 * @param detections
 * @param detections_size
 */
float *discard_words_by_size(struct box **detections, int *detections_size);

/**
 * @brief discard bounding boxes if they overlap
 *
 * remove all the bounding boxes that are inside another
 *
 * @param detections
 * @param detections_size
 */
float *discard_words_by_overlap(struct box **detections, int *detections_size);

/**
 * @brief tries to cut words into letters
 *
 * @param img image struct
 * @param detections
 * @param detections_size
 */
void cut_words(struct box **detections, int *detections_size, struct img img);

/**
 * @brief filter out 
 *
 * @param detections
 * @param detections_size
 */
void filter_out_on_tresh(struct box **detections, int *detections_size, float *certainty, float tresh);


/**
 * @brief use the neural network to estimate if the detection is a letter or grabage
 *
 * @param img image struct
 * @param detections
 * @param detections_size
 */
float *get_nn_confidence(struct img img, struct box *detections, int detections_size);

/**
 * @brief try to cut words based on the output of get_nn_confidence
 *
 * @param img source image
 * @param box bounding box of the sub image
 * @param confidence result of get_nn_confidence
 * @return sub image
 */
void cut_words_based_on_letter_confidence(struct box **detections, int *detections_size, struct img img, float *confidence);


/**
 * @brief return a sub image of the image
 *
 * @param img source image
 * @param box bounding box of the sub image
 * @return sub image
 */
struct img get_sub_image(struct box box, struct img img);

/**
 * @brief saves a sub image of the image to the specified path
 *
 * @param img source image
 * @param savepath path where to save
 * @param box bounding box of the sub image
 */
void save_sub_image(const char *savepath, struct box box, struct img img);

/**
 * @brief make a copy of the image
 *
 * @param img image to copy
 * @return copied image
 */
struct img *cpyimg(struct img img);


/**
 * @brief saves the image to the specified path
 *
 * @param img image to save
 * @param output_path path where to save
 */
void save_img(const char *output_path, struct img img);


void mark_pixel(int x, int y, struct img img);
int get_pixel_mark(int x, int y, struct img img);

#endif
