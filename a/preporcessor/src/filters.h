/**
 * @file filters.h
 * @brief contains all the filters operations
 */
#ifndef FILTERS_H
#define FILTERS_H


/**
 * @brief grayscale filter
 *
 * hardcoded values from https://en.wikipedia.org/wiki/Grayscale#Luma_coding_in_video_systems
 *
 * @param img image struct
 */
void grayscale(struct img img);

/**
 * @brief gaussian blur filter
 *
 * https://en.wikipedia.org/wiki/Gaussian_blur
 *
 * @param img image struct
 * @param ksize size of the kernel. must be odd (e.g. 3, 5, 7)
 * @param sigma ~strongness of the blur
 */
void gaussian_blur(struct img img, int ksize, float sigma);

/**
 * @brief threshold filter
 *
 * make the image binnary i.e each pixel is either completly black
 * or completly white.
 *
 * https://en.wikipedia.org/wiki/Thresholding_(image_processing)
 *
 * @param img image struct
 * @param threshold_value brightness the pixel must have to be white
 */
void threshold(unsigned char threshold_value, struct img img);

/**
 * @brief local threshold filter
 *
 * https://en.wikipedia.org/wiki/Thresholding_(image_processing)
 *
 *
 * @param img image struct
 * @param C ~strongness of the local threshold
 * @param window_size size of the window (like a kernel). must be odd (e.g., 3, 5, 7)
 */
void local_threshold(int window_size, int C, struct img img);

/**
 * @brief erode filter
 *
 * https://medium.com/@wilson.linzhe/digital-image-processing-in-c-chapter-8-erosion-dilation-opening-closing-boundary-5f505c731f19
 * or to prevent the loginwall: https://archive.ph/ch7gs
 *
 * @param img image struct
 * @param kernel size of the kernel. must be odd (e.g., 3, 5, 7)
 */
void erode(int kernel, struct img img);

/**
 * @brief dilate filter
 *
 * https://medium.com/@wilson.linzhe/digital-image-processing-in-c-chapter-8-erosion-dilation-opening-closing-boundary-5f505c731f19
 * or to prevent the loginwall: https://archive.ph/ch7gs
 *
 * @param img image struct
 * @param kernel size of the kernel. must be odd (e.g., 3, 5, 7)
 */
void dilate(int kernel, struct img img);

/**
 * @brief open_operation filter
 *
 * https://medium.com/@wilson.linzhe/digital-image-processing-in-c-chapter-8-erosion-dilation-opening-closing-boundary-5f505c731f19
 * or to prevent the loginwall: https://archive.ph/ch7gs
 *
 * @param img image struct
 * @param kernel size of the kernel. must be odd (e.g., 3, 5, 7)
 */
void open_operation(int kernel, struct img img);

/**
 * @brief close_operation filter
 *
 * https://medium.com/@wilson.linzhe/digital-image-processing-in-c-chapter-8-erosion-dilation-opening-closing-boundary-5f505c731f19
 * or to prevent the loginwall: https://archive.ph/ch7gs
 *
 * @param img image struct
 * @param kernel size of the kernel. must be odd (e.g., 3, 5, 7)
 */
void close_operation(int kernel, struct img img);

/**
 * @brief edge_detection filter
 *
 * set the bightness of the pixel to the difference it has
 * from the neighbouring pixels
 *
 * @param img image struct
 */
void edge_detection(struct img img);



/**
 * @brief border filter
 *
 * adds borders to the images.
 *
 * @param img image struct
 * @param r red channel of the filled pixels
 * @param g green channel of the filled pixels
 * @param b blue channel of the filled pixels
 * @param border_width
 * @param border_height
 */
void border(int border_width, int border_height, int r, int g, int b, struct img *img);

/**
 * @brief rotate filter
 *
 * rotate the image by rot (in radians)
 *
 * @param img image struct
 * @param rot rotation to apply in radians
 * @param r red channel of the filled pixels
 * @param g green channel of the filled pixels
 * @param b blue channel of the filled pixels
 */
void rotate(int r, int g, int b, float rot, struct img *img);

/**
 * @brief scale filter
 *
 * scale the image by the corresponding factor.
 * e.g. the factor is 2 and the image is 800x600.
 * factor makes it a 1600x1200.
 * lossless
 *
 * @param img image struct
 * @param factor 
 */
void scale(int factor, struct img *img);

#endif
