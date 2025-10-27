#ifndef TRAIN_H
#define TRAIN_H


void train_on_image(struct neural_network *n, char *folder,
    double learn_rate, int epoch_numbers,
    int mini_batch_size, int loop_per_mini_batch);
void fill_input_and_expect_img(char *folder, double **inputs, double **expected, int dataset_size);
int extract_letter_num_from_filename(const char *filename);
char* pick_random_file(const char *folder);
void image_to_double64(const char *input_path, double out[64 * 64]);


#endif // !TRAIN_H
