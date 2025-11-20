#include "main.h"
#include <math.h>
#include <stdlib.h>
#include "utils.h"
#include <float.h>

#include <stb_image.h>
#include <stb_image_write.h>
#include <string.h>

#include <pub_neural_network.h>

#include <limits.h>
#include "grid_list.h"


int euclidian_distance(int x1, int y1, int x2, int y2) {
  float dx = x1 - x2;
  float dy = y1 - y2;
  return sqrtf(dx * dx + dy * dy);
}

typedef struct {
    double **adj;
    int **con;
    int size;
} Graph; 



Graph make_graph(struct box *rois, int rois_size, int n) {
  Graph res = {
    .adj = malloc(sizeof(double*) * rois_size),
    .con = malloc(sizeof(int*) * rois_size),
    .size = rois_size,
  };

  for (int i = 0; i < rois_size; i++) {
    res.adj[i] = calloc(rois_size, sizeof(double));
    res.con[i] = calloc(rois_size, sizeof(int));
  }

  for (int i = 0; i < rois_size; i++) {
    struct box curri = rois[i];
    int xi = (curri.max_x + curri.min_x) / 2;
    int yi = (curri.max_y + curri.min_y) / 2;

    int closest_idx[n];
    float closest_dist[n];
    for (int k = 0; k < n; k++) {
      closest_idx[k] = -1;
      closest_dist[k] = 1e9;
    }

    for (int j = 0; j < rois_size; j++) {
      if (j == i) continue;

      struct box currj = rois[j];
      int xj = (currj.max_x + currj.min_x) / 2;
      int yj = (currj.max_y + currj.min_y) / 2;

      float d = euclidian_distance(xi, yi, xj, yj);

      for (int k = 0; k < n; k++) {
        if (d < closest_dist[k]) {
          for (int m = n-1; m > k; m--) {
            closest_dist[m] = closest_dist[m - 1];
            closest_idx[m] = closest_idx[m - 1];
          }
          closest_dist[k] = d;
          closest_idx[k] = j;
          break;
        }
      }
    }

    for (int k = 0; k < n; k++) {
      if (closest_idx[k] == -1) continue;
      struct box currj = rois[closest_idx[k]];
      int xj = (currj.max_x + currj.min_x) / 2;
      int yj = (currj.max_y + currj.min_y) / 2;
    
      res.con[i][closest_idx[k]] = res.con[closest_idx[k]][i] = 1;



      double dx = xj - xi;
      double dy = yj - yi;
      res.adj[i][closest_idx[k]] = res.adj[closest_idx[k]][i] = atan2(dy, dx);
    }
  }

  /*
  for (int i = 0; i < rois_size; i++) 
  { 
    struct box curri = rois[i]; 
    int xi = (curri.max_x + curri.min_x) / 2; 
    int yi = (curri.max_y + curri.min_y) / 2;
    for (int j = 0; j < rois_size; j++) 
    { 
      struct box currj = rois[j]; 
      int xj = (currj.max_x + currj.min_x) / 2;
      int yj = (currj.max_y + currj.min_y) / 2;
      //make_line(xi, yi, xj, yj, 255, 0, 255, *img); 

      double dx = xj- xi;
      double dy = yj - yi;
      adj[i][j] = atan2(dy, dx);

    }
  }
  */


  return res;
}




void draw_closest(struct box *rois, int rois_size, int n, struct img *img) {
  for (int i = 0; i < rois_size; i++) {
    struct box curri = rois[i];
    int xi = (curri.max_x + curri.min_x) / 2;
    int yi = (curri.max_y + curri.min_y) / 2;

    int closest_idx[n];
    float closest_dist[n];
    for (int k = 0; k < n; k++) {
      closest_idx[k] = -1;
      closest_dist[k] = 1e9;
    }

    for (int j = 0; j < rois_size; j++) {
      if (j == i) continue;

      struct box currj = rois[j];
      int xj = (currj.max_x + currj.min_x) / 2;
      int yj = (currj.max_y + currj.min_y) / 2;

      float d = euclidian_distance(xi, yi, xj, yj);

      for (int k = 0; k < n; k++) {
        if (d < closest_dist[k]) {
          for (int m = n-1; m > k; m--) {
            closest_dist[m] = closest_dist[m - 1];
            closest_idx[m] = closest_idx[m - 1];
          }
          closest_dist[k] = d;
          closest_idx[k] = j;
          break;
        }
      }
    }

    for (int k = 0; k < n; k++) {
      if (closest_idx[k] == -1) continue;
      struct box currj = rois[closest_idx[k]];
      int xj = (currj.max_x + currj.min_x) / 2;
      int yj = (currj.max_y + currj.min_y) / 2;
      //make_line(xi, yi, xj, yj, 255, 0, 255, *img);
    }
  }
}


/*
   void draw_all(struct box *rois, int rois_size, struct img *img) {
   for (int i = 0; i < rois_size; i++) 
   { 
   struct box curri = rois[i]; 
   int xi = (curri.max_x + curri.min_x) / 2; 
   int yi = (curri.max_y + curri.min_y) / 2;
   for (int j = 0; j < rois_size; j++) 
   { 
   struct box currj = rois[j]; 
   int xj = (currj.max_x + currj.min_x) / 2;
   int yj = (currj.max_y + currj.min_y) / 2;
   make_line(xi, yi, xj, yj, 255, 0, 255, *img); 
   }
   }
   }
   */


#define N_PI 3.14159265359

double circular_distance(double a, double b) {
  double diff = fabs(a - b);
  while (diff > N_PI * 2.0)
    diff -= (2.0 * N_PI);
  while (diff < 0)
    diff += (2.0 * N_PI);
  return diff;
}


double normalize_angle(double angle) {
    while (angle > N_PI) angle -= 2 * N_PI;
    while (angle < -N_PI) angle += 2 * N_PI;
    return angle;
}

double mean_angle(double *angles, int count) {
    double sin_sum = 0.0, cos_sum = 0.0;
    for (int i = 0; i < count; i++) {
        sin_sum += sin(angles[i]);
        cos_sum += cos(angles[i]);
    }
    return atan2(sin_sum / count, cos_sum / count);
}

int compare_doubles(const void *a, const void *b) {
    double da = *(double*)a;
    double db = *(double*)b;
    return (da > db) - (da < db);
}

typedef struct {
    double mean;
    int size;
} ClusterInfo;

int compare_clusters_desc(const void *a, const void *b) {
    const ClusterInfo *ca = (const ClusterInfo *)a;
    const ClusterInfo *cb = (const ClusterInfo *)b;
    return cb->size - ca->size; // descending order
}

double* compute_cluster_means(double *angles, int size, double tolerance, int min_cluster_size, int *num_means) {
    if (size == 0) {
        *num_means = 0;
        return NULL;
    }

    double *angles_copy = malloc(size * sizeof(double));
    for (int i = 0; i < size; i++) {
        angles_copy[i] = normalize_angle(angles[i]);
    }

    qsort(angles_copy, size, sizeof(double), compare_doubles);

    double *temp_means = malloc(size * sizeof(double));
    int cluster_count = 0;

    int start = 0;
    while (start < size) {
        int end = start;
        while (end < size && fabs(normalize_angle(angles_copy[end] - angles_copy[start])) <= tolerance) {
            end++;
        }

        int cluster_size = end - start;
        if (cluster_size >= min_cluster_size) {
            temp_means[cluster_count++] = mean_angle(&angles_copy[start], cluster_size);
        }

        start = end;
    }

    free(angles_copy);

    double *means = malloc(cluster_count * sizeof(double));
    for (int i = 0; i < cluster_count; i++) {
        means[i] = temp_means[i];
    }
    free(temp_means);

    *num_means = cluster_count;
    return means;
}


int count_square_angles(double *angles, int n, double tolerance) {
  int count = 0;

  for (int i = 0; i < n; i++) {
    for (int j = i + 1; j < n; j++) {
      double diff = fabs(angles[i] - angles[j]);
      if (fabs(diff - N_PI/2.0) < tolerance) {
        count++;
      }
    }
  }
  return count;
}



int* mark_square_angles(const double *angles, int n, double tolerance) {
    if (!angles || n <= 0) return NULL;

    int *mask = calloc(n, sizeof(int));
    if (!mask) return NULL;

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (i == j) continue;

            double diff = fabs(angles[i] - angles[j]);
            if (diff > M_PI)
                diff = 2 * M_PI - diff; // wrap-around (mod 2π)

            if (fabs(diff - M_PI / 2.0) < tolerance) {
                mask[i] = 1;
                break; // no need to check more for this angle
            }
        }
    }

    return mask;
}


void rm_unaligned(Graph graph) {
  for (int i = 0; i < graph.size; i++) {
    for (int j = 0; j < i; j++) {
      if (graph.con[i][j] == 0) continue;

      for (int k = 0; k < i; k++) {
        if (graph.con[j][k] == 0 || k == j) continue;

        if (circular_distance(graph.adj[i][j], graph.adj[j][k]) < 0.1) {
          graph.con[i][j] = graph.con[j][i] = 2;
          graph.con[i][k] = graph.con[j][k] = 2;
        }
      }
    }
  }
}

// https://www.desmos.com/calculator/fjwyfo3q2y
double angle_between_points(int p1x, int p1y, int p2x, int p2y) {
  double cx = p1x - p2x;
  double cy = p1y - p2y;

  double m = sqrt(cx * cx + cy * cy);

  double nx = cx / m;
  double ny = cy / m;

  return atan2(ny, nx);
}


double angle_JIK(double xi, double yi, double xj, double yj, double xk, double yk) {
    double v1x = xj - xi;
    double v1y = yj - yi;
    double v2x = xk - xi;
    double v2y = yk - yi;

    double dot = v1x * v2x + v1y * v2y;
    double mag1 = sqrt(v1x * v1x + v1y * v1y);
    double mag2 = sqrt(v2x * v2x + v2y * v2y);

    double cos_theta = dot / (mag1 * mag2);
    if (cos_theta > 1.0) cos_theta = 1.0;
    if (cos_theta < -1.0) cos_theta = -1.0;

    return acos(cos_theta);
}

void get_loc(struct box curr, int *x, int *y) {
  *x = (curr.max_x + curr.min_x) / 2; 
  *y = (curr.max_y + curr.min_y) / 2;
}

int *get_corners(Graph graph, struct box *rois, int *dets_len) {

  int *dets = NULL;
  int dets_len_l = 0;

  for (int i = 0; i < graph.size; i++) {
    int t = 0;
    for (int j = 0; j < graph.size; j++) {
      if (graph.con[i][j] == 0) continue;

      for (int k = 0; k < graph.size; k++) {
        if (graph.con[i][k] == 0 || j == k) continue;



        int xi, yi, xj, yj, xk, yk;
        get_loc(rois[i], &xi, &yi);
        get_loc(rois[j], &xj, &yj);
        get_loc(rois[k], &xk, &yk);


        if (fabs(angle_JIK(xi, yi, xj, yj, xk, yk) - M_PI) < 0.1) {
          t = 1;
          break;
        }
      }
      if (t) break;
    }
    if (!t) {
      dets_len_l++;
      dets = realloc(dets, sizeof(int) * dets_len_l);
      dets[dets_len_l-1] = i;
    }
  }

  *dets_len = dets_len_l;
  return dets;
}


int *remove_joined_corners(Graph graph, int **dets, int *dets_len) {
  for (int i = 0; i < *dets_len; i++) {
    for (int j = 0; j < *dets_len; j++) {
      if ((*dets)[i] == -1 || (*dets)[j] == -1) continue;
      if (graph.con[(*dets)[i]][(*dets)[j]] || graph.con[(*dets)[j]][(*dets)[i]]) {
        (*dets)[i] = -1;
        (*dets)[j] = -1;
      }
    }
  }

  int c = 0;
  for (int i = 0; i < *dets_len; i++) {
    if ((*dets)[i] != -1) {
      (*dets)[c] = (*dets)[i];
      c++;
    }
  }
  
  *dets = realloc(*dets, sizeof(int) * c);
  *dets_len = c;
}

void remove_non_right_angles_corners(Graph graph, struct box *rois, int **dets, int *dets_len, struct img *img) {
  for (int i = 0; i < *dets_len; i++) {
  //for (int i = 3; i < 4; i++) {
    
    int found = 0;
    int dup = 0;
    double angleX;
    double angleY;

    for (int j = 0; j < graph.size; j++) {
      if (!graph.con[(*dets)[i]][j]) continue;
      for (int k = 0; k < graph.size; k++) {
        if (!graph.con[(*dets)[i]][k] || k > j) continue;


        int xi, yi, xj, yj, xk, yk;
        get_loc(rois[(*dets)[i]], &xi, &yi);
        get_loc(rois[j], &xj, &yj);
        get_loc(rois[k], &xk, &yk);

        if (found && fabs(angle_JIK(xi, yi, xj, yj, xk, yk) - M_PI / 2.0) < 0.1) {

          if (fabs(angle_JIK(xi, yi, xk, yk, angleX, angleY)) >= 0.1
              &&
              fabs(angle_JIK(xi, yi, xj, yj, angleX, angleY)) >= 0.1) {
            //dup = 1;

          }
        }


        if (fabs(angle_JIK(xi, yi, xj, yj, xk, yk) - M_PI / 2.0) < 0.1 && !found) {
          found = 1;

          angleX = xk;
          angleY = yk;
        }
      }
    }

    if (!found || dup) (*dets)[i] = -1;
  }


  int c = 0;
  for (int i = 0; i < *dets_len; i++) {
    if ((*dets)[i] != -1) {
      (*dets)[c] = (*dets)[i];
      c++;
    }
  }

  *dets = realloc(*dets, sizeof(int) * c);
  *dets_len = c;
}

void get_line(Graph graph, struct box *rois, int **dets, int *dets_len, struct img *img) {
  for (int i = 0; i < *dets_len; i++) {
    
    int found = 0;
    
    for (int j = 0; j < graph.size; j++) {
      if (!graph.con[(*dets)[i]][j]) continue;
      for (int k = 0; k < graph.size; k++) {
        if (!graph.con[(*dets)[i]][k]) continue;


        int xi, yi, xj, yj, xk, yk;
        get_loc(rois[(*dets)[i]], &xi, &yi);
        get_loc(rois[j], &xj, &yj);
        get_loc(rois[k], &xk, &yk);


        if (fabs(angle_JIK(xi, yi, xj, yj, xk, yk) - M_PI / 2.0) < 0.1) {
          //make_line(xi, yi, xj, yj, 0, 0, 0, *img);
          //make_line(xi, yi, xk, yk, 255, 0, 255, *img);
          found = 1;
        }
      }
    }

    if (!found) (*dets)[i] = -1;
  }


  int c = 0;
  for (int i = 0; i < *dets_len; i++) {
    if ((*dets)[i] != -1) {
      (*dets)[c] = (*dets)[i];
      c++;
    }
  }

  *dets = realloc(*dets, sizeof(int) * c);
  *dets_len = c;
}


int bbbbb(Graph graph, struct box *rois, int j, double rot, struct img *img, int d) {

  if (d == 0) return 0;
  for (int k = 0; k < graph.size; k++) {
    if (!graph.con[j][k]) continue;
      int xj, yj, xk, yk;
      get_loc(rois[j], &xj, &yj);
      get_loc(rois[k], &xk, &yk);
    if (circular_distance(angle_between_points(xj, yj, xk, yk), rot) < 0.04) {
      return bbbbb(graph, rois, k, rot, img, d-1);
    }
  }
  return d;
}


void same_line(Graph graph, struct box *rois, int *dets, int j, double rot, struct img *img) {
  for (int k = 0; k < graph.size; k++) {
    if (!graph.con[j][k]) continue;
    int xj, yj, xk, yk;
    get_loc(rois[j], &xj, &yj);
    get_loc(rois[k], &xk, &yk);
    if (circular_distance(angle_between_points(xj, yj, xk, yk), rot) < 0.08) {
      struct box curr = rois[k];
      make_box(curr.min_x-1, curr.min_y-1, curr.max_x+1, curr.max_y+1, 255,0,0, *img);
      same_line(graph, rois,dets, k, rot, img);
    }
  }
}


void aaaa(Graph graph, struct box *rois, int **dets, int *dets_len, struct img *img) {
  int smallest = 31;
  int jj = -1;
  int ii = -1;
  for (int i = 0; i < *dets_len; i++) {
    for (int j = 0; j < graph.size; j++) {
      if (!graph.con[(*dets)[i]][j]) continue;

      int xi, yi, xj, yj;
      get_loc(rois[(*dets)[i]], &xi, &yi);
      get_loc(rois[j], &xj, &yj);
      int a = bbbbb(graph, rois, j, angle_between_points(xi, yi, xj, yj), img, 30);
      if (a < smallest) {
        smallest = a;
        jj = j;
        ii = i;
      }

    }
  }


  if (smallest != 31) {
    int xi, yi, xj, yj;
    get_loc(rois[(*dets)[ii]], &xi, &yi);
    get_loc(rois[jj], &xj, &yj);
    struct box curr = rois[(*dets)[ii]];
    make_box(curr.min_x-1, curr.min_y-1, curr.max_x+1, curr.max_y+1, 255,0,0, *img);

    same_line(graph, rois,*dets, (*dets)[ii], angle_between_points(xi, yi, xj, yj), img);
  }
}


void draw_all(struct box *rois, int rois_size, struct img *img) {

  Graph graph = make_graph(rois, rois_size, 8);
  //rm_unaligned(graph);
  int dets_size;
  int *dets = get_corners(graph, rois, &dets_size);

  remove_joined_corners(graph, &dets, &dets_size);
  printf("%i\n", dets_size);
  remove_non_right_angles_corners(graph, rois, &dets, &dets_size, img);
  printf("%i\n", dets_size);
  //get_line(graph, rois, &dets, &dets_size, img);
  aaaa(graph, rois, &dets, &dets_size, img);

  for (int i = 0; i < dets_size; i++) {
    struct box curr = rois[dets[i]];
    //make_box(curr.min_x-1, curr.min_y-1, curr.max_x+1, curr.max_y+1, 255,0,0, *img);

    for (int j = 0; j < rois_size; j++) {
      if (graph.con[dets[i]][j] == 1) {

        struct box curri = rois[dets[i]];
        int xi = (curri.max_x + curri.min_x) / 2; 
        int yi = (curri.max_y + curri.min_y) / 2;

        struct box currj = rois[j]; 
        int xj = (currj.max_x + currj.min_x) / 2;
        int yj = (currj.max_y + currj.min_y) / 2;

        //make_line(xi, yi, xj, yj, 255, 0, 255, *img);
      }
    }
    //printf("%i\n",dets[i]);
  }

  for(int i = 0; i < rois_size; i++) {
    //printf("\n");
    for(int j = 0; j < rois_size; j++) {
      //printf(" %i", graph.con[i][j]);
    }
  }double angle_between_points(int p1x, int p1y, int p2x, int p2y) {
    // Compute angles of each point from the origin
    double a1 = atan2((double)p1y, (double)p1x);
    double a2 = atan2((double)p2y, (double)p2x);

    // Compute signed difference
    double d = a2 - a1;

    // Normalize angle to (-π, π]
    if (d > M_PI)
      d -= 2.0 * M_PI;
    else if (d <= -M_PI)
      d += 2.0 * M_PI;

    return d;
  }

  double tolerance = 0.1;
  int min_cluster_size = 10;

  int num_means = 0;


  //double *means = compute_cluster_meansl((double*)graph.adj, rois_size * rois_size, tolerance, min_cluster_size, &num_means, 4);
  //printf("%i\n", num_means);



  for (int i = 0; i < rois_size; i++) {
    struct box curri = rois[i]; 
    int xi = (curri.max_x + curri.min_x) / 2; 
    int yi = (curri.max_y + curri.min_y) / 2;


    //int count = count_square_angles(graph.adj[i], rois_size, tolerance);

    //printf("%i\n", count);
    //if(count <= 8) continue;

    //int *m = mark_square_angles(graph.adj[i], rois_size, 0.05);

    //for (int k = 0; k < num_means; k++) {
    //make_line(xi, yi, xi + (cos(means[k]) * 100), yi + (sin(means[k]) * 100), 255, 0, 255, *img);
    //}
    for (int j = 0; j < rois_size; j++) {
      //if (m[j] == 0) continue;

      struct box currj = rois[j]; 
      int xj = (currj.max_x + currj.min_x) / 2;
      int yj = (currj.max_y + currj.min_y) / 2;

      if (graph.con[i][j] == 1) {
        //make_line(xi, yi, xj, yj, 255, 0, 255, *img);
      }

      if (graph.con[i][j] == 2) {
        //make_line(xi, yi, xj, yj, 0, 0, 0, *img);
      }

      continue;

      for (int k = 0; k < num_means; k++) {
        //make_line(xj, yj, xj + (cos(means[k]) * 100), yj + (sin(means[k]) * 100), 255, 0, 255, *img);
        //if (circular_distance(graph.adj[i][j], means[k]) <= tolerance) {
        //make_line(xi, yi, xj, yj, 255, 0, 255, *img);
        //} 
      }
    }
  }
  //free(means);
}

