#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define STB_IMAGE_IMPLEMENTATION
#include "rotation.h"

// -----------------------------------------------------
// conversion RGB -> gris
// -----------------------------------------------------
unsigned char toGray(unsigned char r, unsigned char g, unsigned char b) {
    return (unsigned char)(0.299*r + 0.587*g + 0.114*b);
}

// -----------------------------------------------------
// Seuillage d'Otsu
// -----------------------------------------------------
unsigned char computeOtsu(unsigned char *gray, int total) {
    int hist[256] = {0};
    for (int i = 0; i < total; i++) hist[gray[i]]++;

    double sum = 0.0;
    for (int i = 0; i < 256; i++)
        sum += i * hist[i];

    double sumB = 0.0;
    int wB = 0, wF = 0;
    double varMax = 0.0;
    int threshold = 0;

    for (int i = 0; i < 256; i++) {
        wB += hist[i];
        if (wB == 0) continue;

        wF = total - wB;
        if (wF == 0) break;

        sumB += (double)(i * hist[i]);

        double mB = sumB / wB;
        double mF = (sum - sumB) / wF;

        double var = (double)wB * (double)wF * (mB - mF) * (mB - mF);

        if (var > varMax) {
            varMax = var;
            threshold = i;
        }
    }
    return (unsigned char)threshold;
}

// -----------------------------------------------------
// Morphologie verticale (érosion + dilatation)
// -----------------------------------------------------
void verticalMorph(unsigned char *img, int w, int h, int size) {
    unsigned char *tmp = (unsigned char*)malloc(w * h);

    // --- Érosion ---
    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++) {
            int ok = 1;
            for (int dy = -size; dy <= size; dy++) {
                int yy = y + dy;
                if (yy < 0 || yy >= h) continue;
                if (img[yy*w + x] != 0) {
                    ok = 0;
                    break;
                }
            }
            tmp[y*w + x] = ok ? 0 : 255;
        }

    // --- Dilatation ---
    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++) {
            int ok = 0;
            for (int dy = -size; dy <= size; dy++) {
                int yy = y + dy;
                if (yy < 0 || yy >= h) continue;
                if (tmp[yy*w + x] == 0) {
                    ok = 1;
                    break;
                }
            }
            img[y*w + x] = ok ? 0 : 255;
        }

    free(tmp);
}

// -----------------------------------------------------
// Détection très simplifiée d'angle (pseudo-Hough)
// -----------------------------------------------------
float detectAngle(unsigned char *img, int w, int h) {
    int count = 0;
    float sum = 0;

    for (int y = 0; y < h; y++) {
        int blackStreak = 0;
        for (int x = 0; x < w; x++) {
            int isBlack = (img[y*w + x] == 0);

            if (isBlack) {
                blackStreak++;
                if (blackStreak > 30) {
                    // Angle approximatif : proche d'horizontal
                    float angle = atan2(1.0, (double)w) * 180.0 / M_PI;
                    sum += angle;
                    count++;
                }
            } else {
                blackStreak = 0;
            }
        }
    }

    if (count == 0) return 0;
    return sum / count;
}

// -----------------------------------------------------
// MAIN
// -----------------------------------------------------
int deg_rotation(struct img *imgage){
    int w = imgage->width;
    int h = imgage->height;
    int c = imgage->channels;

    unsigned char *img = imgage->img;

    int total = w * h;
    unsigned char *gray = (unsigned char*)malloc(total);
    unsigned char *bin  = (unsigned char*)malloc(total);

    for (int i = 0; i < total; i++){
        gray[i] = toGray(img[3*i], img[3*i+1], img[3*i+2]);
    }

    unsigned char th = computeOtsu(gray, total);

    for (int i = 0; i < total; i++){
        bin[i] = (gray[i] > th ? 0 : 255);
    }

    verticalMorph(bin, w, h, 25);

    float angle = detectAngle(bin, w, h);
    

    free(gray);
    free(bin);
    //stbi_image_free(img);
    return angle;
}

// int main(int argc, char **argv) {
//     if (argc < 2) {
//         printf("Usage: %s image.jpg\n", argv[0]);
//         return -1;
//     }

//     int w, h, c;
    
//     unsigned char *img = stbi_load(argv[1], &w, &h, &c, 3);
//     if (!img) {
//         printf("Erreur chargement image.\n");
//         return -1;
//     }

//     int total = w * h;
//     unsigned char *gray = (unsigned char*)malloc(total);
//     unsigned char *bin  = (unsigned char*)malloc(total);

//     // 1. Gris
//     for (int i = 0; i < total; i++){
//         gray[i] = toGray(img[3*i], img[3*i+1], img[3*i+2]);
//     }

//     // 2. Otsu
//     unsigned char th = computeOtsu(gray, total);

//     // 3. Binarisation + inversion
//     for (int i = 0; i < total; i++){
//         bin[i] = (gray[i] > th ? 0 : 255);
//     }
//     // 4. Morphologie verticale
//     verticalMorph(bin, w, h, 25);

//     // 5. Angle
//     float angle = detectAngle(bin, w, h);

//     //printf("Orientation estimée : %.2f degrés\n", angle);
    

//     free(gray);
//     free(bin);
//     stbi_image_free(img);
//     return angle;
// }