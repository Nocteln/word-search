#include <sys/stat.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <stb_image.h>
#include <stb_image_write.h>
#include <stdio.h>
#include <stdlib.h>

#include "main.h"
#include "filters.h"
#include "utils.h"
#include "defs.h"


void add_vec2_array(int **array, int x, int y, int size) {
  *array = realloc(*array, size * 2 * sizeof(int));
  (*array)[(size*2)-1] = y;
  (*array)[(size*2)-2] = x;
}

void mark_pixell(int x, int y, struct img *img) {

  img->img[(y * img->width + x) * img->channels + 0] = 255;
  img->img[(y * img->width + x) * img->channels + 1] = 0;
  img->img[(y * img->width + x) * img->channels + 2] = 255;
}


struct img *process_image_aux(struct img *img) {

  grayscale(*img);
  border(10, 10, 255,255,255, img);

  local_threshold(7,9,*img);

  gaussian_blur(*img, 3, 7.);
  threshold(150,*img);
  
  scale(3,img);


  //erode(3,*img);
  //dilate(3,*img);


  //rotate(255,255,255,-0.14*M_PI, img);

  save_img("interm/before_noise_rm.png", *img);
  
  /*
  struct img *img_edge = cpyimg(*img);
  edge_detection(*img_edge);
  save_img("interm/edge_detection.png", *img_edge);

  free(img_edge->img);
  free(img_edge);
  */


  struct img *flood_img = cpyimg(*img);

  struct box *rois = malloc(0);
  int *rois_start_pos = malloc(0);
  int rois_size = 0;

  for (int y = 0; y < img->height - 1; ++y) {
    for (int x = 0; x < img->width - 1; ++x) {
      if (flood_img->img[(y * flood_img->width + x) * flood_img->channels] == 255) continue;
      int depth = 10000000;
      struct box box = flood(x+1, y+1, &depth, *flood_img);
      if (box.max_x != -1) {
        push_box_array(&rois, box, &rois_size);

        add_vec2_array(&rois_start_pos, x, y, rois_size);
      }
    }
  }
  free(flood_img->img);
  free(flood_img);
 
  
  float *cl;
  cut_words(&rois, &rois_size, *img);

  float *supretion_rate = z_score_words_size(rois,rois_size); 
  filter_out_on_tresh(&rois,&rois_size,supretion_rate,0.6); // suppr toutes les cases qui ont un pourcentage d'err sup a 60%
  int averagedistance = average_distance(rois,rois_size,30); // sur la mm ligne à 30pix près
  int nbwords = 0;
  int *words_length = NULL;
  int width = 0;
  int length = 0;
  struct box ***words_and_grid = NULL; 
  make_words_and_grid(&words_and_grid,rois,rois_size,averagedistance,30,&words_length,&width,&length,&nbwords);
  if(words_and_grid == NULL){
	printf("ca marche pas\n");
  }
  for(int i = 0;i<length;i++){
	for(int j = 0;j<width;j++){
		struct box value = words_and_grid[1][i][j];
		printf("%i    ",value.min_y);
	}
	printf("\n");
  }
  printf("%i\n",length);
  printf("%i\n",width);
  printf("%i\n",nbwords);
  printf("%p\n",words_and_grid);
  for(int i = 0; i<nbwords;i++){
	  for(int j = 0;j<words_length[i];j++){
		struct box value = words_and_grid[0][i][j];
		printf("x = %i, y = %i     ",value.min_x,value.min_y);
	  }
	  printf("\n");
  }

  cl = z_score_words_size(rois, rois_size);
  //filter_out_on_tresh(&rois, &rois_size, cl, 0.95);
  free(cl);

  //cut_words(&rois, &rois_size, *img);


  if (0) {
    char buffer[100];
    srand(time(NULL));
    int dump_int = rand();

    sprintf(buffer, "dump%d", dump_int);
    printf("DUMP FOLDER:\n");
    printf("dump%d\n", dump_int);
    mkdir(buffer, 0777);

    for (int i = 0; i < rois_size; i++) {
      struct box curr = rois[i];

      sprintf(buffer, "dump%d/%d.png",dump_int, rand());


      struct box roi_to_save = {
        .min_x = curr.min_x-1,
        .min_y = curr.min_y-1,
        .max_x = curr.max_x+1,
        .max_y = curr.max_y+2 };
      save_sub_image(buffer, roi_to_save, *img);
    }
  }

  /*


     cl = discard_words_by_overlap(&rois, &rois_size);
  //filter_out_on_tresh(&detections, &detections_size, cl, 0.95);
  free(cl);

  cl = z_score_words_size(rois, rois_size);
  filter_out_on_tresh(&rois, &rois_size, cl, 0.95);
  free(cl);


*/

  for (int i = 0; i < rois_size; i++) {
    struct box curr = rois[i];
     make_box(curr.min_x-1, curr.min_y-1, curr.max_x+1, curr.max_y+1, 255,0,0, *img);
  }

  save_img("output.png", *img);

  //save_img("output.png", *img);

  return img;
}

void process_image(struct img *img) {
  img = process_image_aux(img);

  free(img->img);
  free(img);
}

