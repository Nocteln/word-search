#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <string.h>

int directions[8][2] = {
    {1, 0}, {-1, 0}, {0, 1}, {0, -1},
    {1, 1}, {-1, -1}, {1, -1}, {-1, 1}
};

void upper(char *s) 
{
    for (; *s; s++) {
        if (*s >= 'a' && *s <= 'z')
            *s = *s - 'a' + 'A';
    }
}

void load_array(char* filename, char ***grid, int *rows, int *cols) 
{
    FILE *file = fopen(filename, "r");
    if (!file) errx(EXIT_FAILURE, "error while opening the file!");

    char line[1024];
    int r =0, c = 0;
    char **tmp = NULL;
    while (fgets(line, sizeof(line), file)) 
    {
        line[strcspn(line, "\n")] = '\0';
        if (c==0) c = strlen(line);
        tmp = realloc(tmp, sizeof(char *) * (r+1));
        tmp[r] = strdup(line);
        upper(tmp[r]);
        r++;
    }

    fclose(file);

    *grid = tmp;
    *rows = r;
    *cols = c;
}

int solver(char **grid, int rows, int cols, const char* word, int* sx, int* sy, int* ex, int* ey) // s = start, e = end
{
    int len = strlen(word);

    for (int y = 0; y < rows; y++)
    {
        for (int x = 0; x < cols; x++)
        {
            if (grid[y][x] != word[0]) continue;

            for (int d = 0; d < 8; d++) 
            {
                int dx = directions[d][0];
                int dy = directions[d][1];

                int nx = x, ny = y;
                int i;

                for (i = 1; i < len; i++)
                {
                    nx += dx;
                    ny += dy;

                    if (nx < 0 || ny < 0 || nx >= cols || ny >= rows) break;
                    if (grid[ny][nx] != word[i]) break;
                }

                if (i == len) 
                {
                    *sx = x;
                    *sy = y;
                    *ex = nx; 
                    *ey = ny;
                    return 1;
                }
            }
        }
    }
    return 0;
}

int main(int argc, char * argv[]) { 
    char **grid;
    int rows, cols;
    if (argc > 2) {

        load_array(argv[1], &grid, &rows, &cols);
        int sx,sy,ex,ey;

        char* word = argv[2];
        upper(word);

        if (solver(grid, rows, cols, word, &sx, &sy, &ex, &ey))
        {
            printf("Word found : (%d,%d)(%d,%d)\n", sx, sy, ex, ey);
        } else printf("Word not found!\n");
    } else {
        printf("please give me a grid and a word!\n");
    }

    return 0;
}
