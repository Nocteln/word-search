
#include <ctype.h>
#include <sys/stat.h>
#include <stdio.h>

#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include <time.h>
#include "defs.h"
#include "learn.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image.h>
#include <stb_image_write.h>
#include <stdio.h>
#include <stdlib.h>

#include <stdio.h>
#include <stdlib.h>

// chatgpt, à relire
void image_to_double64(const char *input_path, double out[64 * 64]) {
    int width, height, channels;
    unsigned char *img;

    // Load image
    img = stbi_load(input_path, &width, &height, &channels, 0);
    if (img == NULL) {
        fprintf(stderr, "Can't load image (%s)\n", input_path);
        exit(1);
    }

    // Resize to 64x64 using nearest neighbor
    for (int y = 0; y < 64; y++) {
        for (int x = 0; x < 64; x++) {
            // Map target pixel to source pixel
            int src_x = (int)((float)x / 64.0f * width);
            int src_y = (int)((float)y / 64.0f * height);

            int src_index = (src_y * width + src_x) * channels;

            unsigned char value;
            if (channels == 1) {
                // grayscale image
                value = img[src_index];
            } else {
                // binary images usually have R=G=B, so take red
                value = img[src_index];
            }

            // Threshold (binary image, but just in case)
            double bin = (value > 127) ? 1.0 : 0.0;

            // Store in row-major flat array
            out[y * 64 + x] = bin;
        }
    }

    stbi_image_free(img);
}


// chatgpt, à relire
char* pick_random_file(const char *folder) {
    DIR *dir;
    struct dirent *entry;
    struct stat file_stat;
    char **files = NULL;
    size_t file_count = 0;

    dir = opendir(folder);
    if (!dir) {
        perror("opendir");
        return NULL;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        char path[4096];
        snprintf(path, sizeof(path), "%s/%s", folder, entry->d_name);

        if (stat(path, &file_stat) == 0 && S_ISREG(file_stat.st_mode)) {
            files = realloc(files, sizeof(char*) * (file_count + 1));
            files[file_count] = strdup(path);
            file_count++;
        }
    }
    closedir(dir);

    if (file_count == 0) {
        fprintf(stderr, "No files found in folder: %s\n", folder);
        return NULL;
    }


    size_t idx = rand() % file_count;

    char *result = strdup(files[idx]);

    for (size_t i = 0; i < file_count; i++) {
        free(files[i]);
    }
    free(files);

    return result;
}

int extract_letter_num_from_filename(const char *filename) {
    const char *dot = strrchr(filename, '.');

    const char *p = dot - 1;
    while (p > filename && isdigit(*p)) {
        p--;
    }

    if (*p == '_') return -1;
    return tolower(*p) - 'a';
}

void fill_input_and_expect_img(char *folder, double **inputs, double **expected, int dataset_size) {
  for (int i = 0; i < dataset_size; i++) {
    char *file = pick_random_file(folder);
    int num = extract_letter_num_from_filename(file);
    double *out = malloc(sizeof(double) * 64 * 64);
    image_to_double64(file, out);
    
    expected[i] = calloc(26, sizeof(double));
    if (num != -1)
      expected[i][num] = 1.;

    inputs[i] = out;
  }
}

void train_on_image(struct neural_network *n, char *folder,
    double learn_rate, int epoch_numbers,
    int mini_batch_size, int loop_per_mini_batch) {

  double **inputs = malloc(sizeof(double*) * mini_batch_size);
  double **expect = malloc(sizeof(double*) * mini_batch_size); 


  printf("training %d times...\n\n", epoch_numbers);
  for (int i = 0; i < epoch_numbers; i++) {
    printf("epoch %d, loading mini batch...\n", i);
    fill_input_and_expect_img(folder, inputs, expect, mini_batch_size);
    printf("mini batch loading ended, training %d times on %d imgs...\n", loop_per_mini_batch, mini_batch_size);
    for (int j = 0; j < loop_per_mini_batch; j++) {
      learn_epoch_with_backpropagation(n, inputs, expect, mini_batch_size, learn_rate);
    }
    for (int i = 0; i < mini_batch_size; i++) {
      free(expect[i]);
      free(inputs[i]);
    }
    printf("ended.\n\n");
  }
}
