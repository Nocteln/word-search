#ifndef SOLVER_H
#define SOLVER_H

/**
 * @brief Load a grid from a file
 * 
 * @param filename Path to the grid file
 * @param grid Pointer to store the grid
 * @param rows Pointer to store number of rows
 * @param cols Pointer to store number of columns
 */
void load_array(char* filename, char ***grid, int *rows, int *cols);

/**
 * @brief Search for a word in the grid
 * 
 * @param grid The grid to search in
 * @param rows Number of rows
 * @param cols Number of columns
 * @param word The word to search for
 * @param sx Start x coordinate (output)
 * @param sy Start y coordinate (output)
 * @param ex End x coordinate (output)
 * @param ey End y coordinate (output)
 * @return 1 if word found, 0 otherwise
 */
int solver(char **grid, int rows, int cols, const char* word, int* sx, int* sy, int* ex, int* ey);

/**
 * @brief Convert string to uppercase
 * 
 * @param s String to convert
 */
void upper(char *s);

#endif
