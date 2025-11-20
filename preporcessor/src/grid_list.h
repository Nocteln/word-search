#ifndef GRID_LIST_H
#define GRID_LIST_H
int euclidian_distance(int x1, int y1, int x2, int y2);
Graph make_graph(struct box *rois, int rois_size, int n);
void draw_closest(struct box *rois, int rois_size, int n, struct img *img);
void draw_all(struct box *rois, int rois_size, struct img *img);
double circular_distance(double a, double b);
double normalize_angle(double angle);
double mean_angle(double *angles, int count);
int compare_doubles(const void *a, const void *b);
int compare_clusters_desc(const void *a, const void *b);
double* compute_cluster_means(double *angles, int size, double tolerance, int min_cluster_size, int *num_means);
int count_square_angles(double *angles, int n, double tolerance);
int* mark_square_angles(const double *angles, int n, double tolerance);
void rm_unaligned(Graph graph);
double angle_between_points(int p1x, int p1y, int p2x, int p2y);
double angle_JIK(double xi, double yi, double xj, double yj, double xk, double yk);
void get_loc(struct box curr, int *x, int *y);
int *get_corners(Graph graph, struct box *rois, int *dets_len);
int *remove_joined_corners(Graph graph, int **dets, int *dets_len);
void remove_non_right_angles_corners(Graph graph, struct box *rois, int **dets, int *dets_len, struct img *img);
void get_line(Graph graph, struct box *rois, int **dets, int *dets_len, struct img *img);
int bbbbb(Graph graph, struct box *rois, int j, double rot, struct img *img, int d);
void same_line(Graph graph, struct box *rois, int *dets, int j, double rot, struct img *img);
void aaaa(Graph graph, struct box *rois, int **dets, int *dets_len, struct img *img);
void draw_all(struct box *rois, int rois_size, struct img *img);


#endif
