#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#include <stdio.h>

char *levels = " .:-=+*#%@";

int main(int argc, char **argv) {
    if (argc < 2) return 1;
    int w, h, c;
    unsigned char *img = stbi_load(argv[1], &w, &h, &c, 1);
    if (!img) { printf("Error loading\n"); return 1; }
    
    for(int y=0; y<h; y+=2) { // skip lines to make it look square-ish in terminal
        for(int x=0; x<w; x++) {
            int val = img[y*w + x];
            int idx = val * 9 / 255;
            putchar(levels[idx]);
        }
        putchar('\n');
    }
    return 0;
}
