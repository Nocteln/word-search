#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include <time.h>

#include "defs.h"

//#define DEBUG_LAYER_ACTIVATION
#include "activation_functions.h"
#include "neural_network.h"
#include "train.h"
#include "nodes_loss.h"
#include "save_load.h"

void top4_indices(const double *arr, int out_idx[4]) {
    for (int i = 0; i < 4; i++) {
        out_idx[i] = -1;
    }

    double top_val[4] = { -1, -1, -1, -1 };
    int top_index[4] = { -1, -1, -1, -1 };

    for (int i = 0; i < 26; i++) {
        double v = arr[i];

        for (int j = 0; j < 4; j++) {
            if (v > top_val[j]) {
                for (int k = 3; k > j; k--) {
                    top_val[k] = top_val[k-1];
                    top_index[k] = top_index[k-1];
                }
                top_val[j] = v;
                top_index[j] = (int)i;
                break;
            }
        }
    }

    for (int i = 0; i < 4; i++) {
        out_idx[i] = top_index[i];
    }
}


void print_res(double *out) {
  int res[4];
  top4_indices(out, res);
  for (int i = 0; i < 4; i++) {
    printf("%c : %f\n", res[i] + 'a', out[res[i]]);
  }
}

int main()
{
  double out[64 * 64];
  struct neural_network *n;
  if (0) {
    int layers[] = { 64 * 64, 220, 60, 26 };
    n = create_neural_network(layers, 4,
        &sigmoid_af, &mse_nl,
        &sigmoid_af_d, &mse_nl_d);
    srandom(time(NULL));
    fill_random_neural_network(n);


    train_on_image(n, "letters/filtered", 0.3, 300, 50, 13);


    save_network("./neural_network/network.bin", n);
  } else {
    n = load_network("./neural_network/network.bin");
  }

  /*
  char buffer[1024];
  while (1) {
    printf("> ");

    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
      break;
    }

    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
      buffer[len - 1] = '\0';
    }

    if (buffer[0] == '\0') {
      continue;
    }

    char *file = &buffer[0];
    int num = extract_letter_num_from_filename(file);

    image_to_double64(file, out);

    double *outp = calculate_neural_network_outputs(n, out);
    printf("file : %s\n", file);
    printf("\nexpect %c\n", 'a' + num);
    print_res(outp);
    free(outp);

  }
  */


  drop_neural_network();
  return 0;
}
