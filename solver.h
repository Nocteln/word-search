#ifndef SOLVER_H
#define SOLVER_H

void load_array(char* filename, char ***grid, int *rows, int *cols);

int solver(char **grid, int rows, int cols, const char* word, int* sx, int* sy, int* ex, int* ey);

void upper(char *s);

#endif
