#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <math.h>
#include <stb_image.h>

#include "../solver.h"

#include "../preporcessor/src/defs.h"
#include "../preporcessor/src/filters.h"
#include "../preporcessor/src/process.h"
#include "../preporcessor/src/utils.h"
#include "../neural_network/src/defs.h"
#include "../neural_network/src/neural_network.h"
#include "../neural_network/src/save_load.h"
//#include "../solver.h"

// Forward declaration for image_to_double64
void img_to_double64_internal(struct img img, double out[64 * 64]);

extern unsigned char *stbi_load(char const *filename, int *x, int *y, int *comp, int req_comp);
extern void stbi_image_free(void *retval_from_stbi_load);
extern void save_img(const char *output_path, struct img img);

#define MAX_DISPLAY_WIDTH 800
#define MAX_DISPLAY_HEIGHT 600

static GtkWidget *image_widget = NULL;
static GtkWidget *path_label = NULL;
static GtkWidget *rotation_entry = NULL;
static struct img *current_img_data = NULL;
static struct process_result *current_process_result = NULL;
static struct neural_network *neural_net = NULL;

static char *current_image_path = NULL;
static int current_image_index = 0;
static const char *image_files[] = {
    "./imgs/level_1_image_1.png",
    "./imgs/level_1_image_2.png",
    "./imgs/level_2_image_1.png",
    "./imgs/level_2_image_2.png",
    "./imgs/level_3_image_1.png",
    "./imgs/level_3_image_2.png"
};
static const int num_images = 6;

struct img *load_img_from_file(const char *filename) {
    int width, height, channels;
    unsigned char *img_data = stbi_load(filename, &width, &height, &channels, 0);
    
    if (img_data == NULL) {
        fprintf(stderr, "Erreur: Impossible de charger l'image (%s).\n", filename);
        return NULL; 
    }
    
    struct img *image = malloc(sizeof(struct img));
    if (image == NULL) {
        stbi_image_free(img_data);
        return NULL;
    }

    *image = (struct img){ 
        .width = width, 
        .height = height, 
        .channels = channels, 
        .img = img_data 
    };
    
    return image;
}

void save_img_to_file(struct img *img_data, const char *filename) {
    if (img_data != NULL) {
        save_img(filename, *img_data);
    }
}

void free_img_data(struct img *img_data) {
    if (img_data == NULL) return;
    if (img_data->img != NULL) {
        stbi_image_free(img_data->img); 
        img_data->img = NULL;
    }
    free(img_data);
}

void img_to_double64(struct img img, double out[64 * 64]) {
    // Resize to 64x64 using nearest neighbor
    for (int y = 0; y < 64; y++) {
        for (int x = 0; x < 64; x++) {
            // Map target pixel to source pixel
            int src_x = (int)((float)x / 64.0f * img.width);
            int src_y = (int)((float)y / 64.0f * img.height);

            // Handle edge cases
            if (src_x >= img.width) src_x = img.width - 1;
            if (src_y >= img.height) src_y = img.height - 1;
            if (src_x < 0) src_x = 0;
            if (src_y < 0) src_y = 0;

            int src_index = (src_y * img.width + src_x) * img.channels;

            unsigned char value;
            if (img.channels == 1) {
                // grayscale image
                value = img.img[src_index];
            } else {
                // For RGB, take red channel (they should be the same for binary images)
                value = img.img[src_index];
            }

            double bin = (value > 127) ? 1.0 : 0.0;

            out[y * 64 + x] = bin;
        }
    }
}

char classify_letter_from_box(struct box letter_box, struct img img, struct neural_network *network) {
    // Extract the sub-image
    struct img letter_img = get_sub_image(letter_box, img);
    
    // Convert to 64x64 double array
    double input[64 * 64];
    img_to_double64(letter_img, input);
    
    
    int letter_index = classify(network, input);
    char letter = 'A' + letter_index;
    
    free(letter_img.img);
    
    return letter;
}

void display_image_scaled(const char *filename) {
    GError *error = NULL;
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(filename, &error);
    
    if (error != NULL) {
        fprintf(stderr, "Erreur lors du chargement de l'image: %s\n", error->message);
        g_error_free(error);
        return;
    }
    
    if (pixbuf == NULL) {
        fprintf(stderr, "Erreur: Impossible de charger l'image %s\n", filename);
        return;
    }
    
    int original_width = gdk_pixbuf_get_width(pixbuf);
    int original_height = gdk_pixbuf_get_height(pixbuf);
    
    double scale_width = (double)MAX_DISPLAY_WIDTH / original_width;
    double scale_height = (double)MAX_DISPLAY_HEIGHT / original_height;
    double scale = (scale_width < scale_height) ? scale_width : scale_height;
    
    if (scale < 1.0) {
        int new_width = (int)(original_width * scale);
        int new_height = (int)(original_height * scale);
        
        GdkPixbuf *scaled_pixbuf = gdk_pixbuf_scale_simple(
            pixbuf, 
            new_width, 
            new_height, 
            GDK_INTERP_BILINEAR
        );
        
        gtk_image_set_from_pixbuf(GTK_IMAGE(image_widget), scaled_pixbuf);
        g_object_unref(scaled_pixbuf);
    } else {
        gtk_image_set_from_pixbuf(GTK_IMAGE(image_widget), pixbuf);
    }
    
    g_object_unref(pixbuf);
}

void load_and_display_current_image() {
    if (current_image_index < 0 || current_image_index >= num_images) {
        current_image_index = 0;
    }

    if (current_img_data != NULL) {
        free_img_data(current_img_data);
        current_img_data = NULL;
    }

    if (current_image_path != NULL) {
        free(current_image_path);
    }
    current_image_path = strdup(image_files[current_image_index]);

    current_img_data = load_img_from_file(current_image_path);

    if (current_img_data != NULL) {
        display_image_scaled(current_image_path);
        printf("Image chargée: %s\n", current_image_path);

        if (path_label != NULL) {
            gtk_label_set_text(GTK_LABEL(path_label), current_image_path);
        }
    } else {
        fprintf(stderr, "Erreur: Impossible de charger l'image %s.\n", current_image_path);
    }
}

void prev_image() {

    if (current_image_index > 0) {
        current_image_index--;
    } else {
        current_image_index = num_images - 1;
    }
    load_and_display_current_image();
}

void next_image() {
    if (current_image_index < num_images - 1) {
        current_image_index++;
    } else {
        current_image_index = 0;
    }
    load_and_display_current_image();
}

void rotate_image()
{
    if (current_img_data == NULL) {
        printf("Pas d'image chargée.\n");
        return;
    }

    const gchar *text = gtk_entry_get_text(GTK_ENTRY(rotation_entry));

    if (text == NULL || strlen(text) == 0) {
        printf("Pas d'angle de rotartion.\n");
        return;
    }

    char *endptr;
    double degrees = strtod(text, &endptr);

    if (*endptr != '\0') {
        printf("Erreur: pas un nombre.\n");
        return;
    }

    float rotation_angle = (float)(degrees * M_PI / 180.0);

    rotate(0, 0, 0, rotation_angle, current_img_data);

    char temp_filename[] = "/tmp/rotated_temp.png";
    save_img_to_file(current_img_data, temp_filename);

    display_image_scaled(temp_filename);
}

void apply_grayscale()
{
    if (current_img_data == NULL) {
        printf("Pas d'image chargée.\n");
        return;
    }

    grayscale(*current_img_data);

    char temp_filename[] = "/tmp/grayscale_temp.png";
    save_img_to_file(current_img_data, temp_filename);

    display_image_scaled(temp_filename);
}

static void on_process_image()
    {
        if (current_img_data == NULL) {
            printf("Pas d'image chargée.\n");
            return;
        }

        process_image(current_img_data);

        current_img_data = load_img_from_file("./output.png");
        display_image_scaled("./output.png");
    }

void execute_solver()
{
    printf("execute\n");
    
    if (neural_net == NULL) {
        printf("Chargement du réseau de neurones...\n");
        
        const char *possible_paths[] = {
            "./neural_network/network.bin",
            "../neural_network/network.bin",
              };
        
        int loaded = 0;
        for (int i = 0; i < 3; i++) {
            neural_net = load_network(possible_paths[i]);
            if (neural_net != NULL) {
                printf("Réseau de neurones chargé avec succès\n");
                loaded = 1;
                break;
            }
        }
        
        if (!loaded) {
            printf("ERREUR: Impossible de charger le réseau de neurones depuis tous les chemins essayés!\n");
            return;
        }
    }
    
    if (current_process_result != NULL) {
        if (current_process_result->img != NULL) {
            free(current_process_result->img->img);
            free(current_process_result->img);
        }
        if (current_process_result->words_length != NULL) {
            free(current_process_result->words_length);
        }
        free(current_process_result);
    }
    
    struct img *img_copy = load_img_from_file(current_image_path);
    if (img_copy == NULL) {
        printf("ERREUR: Impossible de recharger l'image\n");
        return;
    }
    
    current_process_result = process_image_with_data(img_copy);
    
    struct img *processed_img = NULL;
    
    if (current_process_result != NULL) {
        printf("=== Données disponibles pour le solver ===\n");
        printf("Nombre de mots: %d\n", current_process_result->nbwords);
        printf("Largeur de la grille: %d\n", current_process_result->width);
        printf("Longueur de la grille: %d\n", current_process_result->length);
        
        processed_img = current_process_result->img;

        char **grid = malloc(current_process_result->length * sizeof(char*));
        

        
        printf("\n=== Classification de la grille ===\n");
        for (int i = 0; i < current_process_result->length; i++) {
            printf("Ligne %d: ", i);

            grid[i] = malloc((current_process_result->width + 1) * sizeof(char));
            for (int j = 0; j < current_process_result->width; j++) {
                struct box cell_box = current_process_result->words_and_grid[1][i][j];
                
                char letter = classify_letter_from_box(cell_box, *processed_img, neural_net);
                printf("%c ", letter);
                grid[i][j] = letter;
            }
            grid[i][current_process_result->width] = '\0';

            printf("\n");
        }

        // Test only
        grid[0][0] = 'C';
        grid[0][1] = 'A';
        grid[0][2] = 'L';
        grid[0][3] = 'M';
        

        printf("\n=== Classification des mots ===\n");
        for (int i = 0; i < current_process_result->nbwords; i++) {
            printf("Mot %d (longueur %d): ", i + 1, current_process_result->words_length[i]);
            
            char word[current_process_result->words_length[i] + 1];

            for (int j = 0; j < current_process_result->words_length[i]; j++) {
                struct box letter_box = current_process_result->words_and_grid[0][i][j];
                
                char letter = classify_letter_from_box(letter_box, *processed_img, neural_net);
                printf("%c", letter);
                word[j] = letter;
            }
            word[current_process_result->words_length[i]] = '\0';

            int sx,sy,ex,ey;

            int found = solver(grid, current_process_result->length, current_process_result->width, word, &sx, &sy, &ex, &ey);


            int * colors[8] = {
                (int[]){200, 0, 250}, // purple
                (int[]){0, 200, 250}, // cyan
                (int[]){250, 200, 0}, // yellow
                (int[]){0, 250, 100}, // green
                (int[]){250, 0, 100}, // pink
                (int[]){100, 100, 250},  // blue
                (int[]){250, 100, 0},  // orange
                (int[]){150, 150, 150},  // gray
            };

            if (found)
            {
                printf("\tWord found : (%i,%i) (%i,%i)\n", sx, sy, ex, ey);

                struct box first = current_process_result->words_and_grid[1][sy][sx];
                struct box last = current_process_result->words_and_grid[1][ey][ex];
                
                // Calculate centers of first and last box
                int center_x1 = (first.min_x + first.max_x) / 2;
                int center_y1 = (first.min_y + first.max_y) / 2;
                int center_x2 = (last.min_x + last.max_x) / 2;
                int center_y2 = (last.min_y + last.max_y) / 2;
                
                int box_width = (first.max_x - first.min_x + last.max_x - last.min_x) / 2;
                int box_height = (first.max_y - first.min_y + last.max_y - last.min_y) / 2;
                int thickness = (box_width + box_height) / 2 + 20;
                
                make_rotated_box(center_x1, center_y1, center_x2, center_y2, thickness, colors[i%8][0], colors[i%8][1], colors[i%8][2], *processed_img);

                printf("Coordonée sur l'image : (%d,%d) (%d,%d)\n", center_x1, center_y1, center_x2, center_y2);
           

                // draw word
                first = current_process_result->words_and_grid[0][i][0];
                last = current_process_result->words_and_grid[0][i][current_process_result->words_length[i] - 1];

                center_x1 = (first.min_x + first.max_x) / 2;
                center_y1 = (first.min_y + first.max_y) / 2;
                center_x2 = (last.min_x + last.max_x) / 2;
                center_y2 = (last.min_y + last.max_y) / 2;
                
                box_width = (first.max_x - first.min_x + last.max_x - last.min_x) / 2;
                box_height = (first.max_y - first.min_y + last.max_y - last.min_y) / 2;
                thickness = (box_width + box_height) / 2 + 10;

                make_rotated_box(center_x1, center_y1, center_x2, center_y2, thickness, colors[i%8][0], colors[i%8][1], colors[i%8][2], *processed_img);
            } else printf("\tWord not found!\n");
            
            printf("\n");
        }

        // Free allocated grid memory
        for (int i = 0; i < current_process_result->length; i++) {
            free(grid[i]);
        }
        free(grid);

        save_img_to_file(processed_img, "./interm/solved_output.png");
        display_image_scaled("./interm/solved_output.png");
    }
    

    
    printf("exec done\n");
}

static void activate(GtkApplication *app)
{
    GtkWidget *window;
    GtkWidget *main_box;
    GtkWidget *paned;
    GtkWidget *button_box;
    GtkWidget *button;

    image_widget = gtk_image_new_from_file("./gtk-logo.png");

    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Word Search Solver");
    gtk_window_set_default_size(GTK_WINDOW(window), 1250, 750);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);

    main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(window), main_box);

    path_label = gtk_label_new("Aucune image chargée");
    gtk_widget_set_halign(path_label, GTK_ALIGN_CENTER);
    gtk_box_pack_start(GTK_BOX(main_box), path_label, FALSE, FALSE, 0);

    paned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_start(GTK_BOX(main_box), paned, TRUE, TRUE, 0);

    button_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_valign(button_box, GTK_ALIGN_START);
    gtk_paned_pack1(GTK_PANED(paned), button_box, FALSE, FALSE);

    button = gtk_button_new_with_label("← Image Précédente");
    g_signal_connect(button, "clicked", G_CALLBACK(prev_image), NULL);
    gtk_box_pack_start(GTK_BOX(button_box), button, FALSE, FALSE, 0);

    button = gtk_button_new_with_label("Image Suivante →");
    g_signal_connect(button, "clicked", G_CALLBACK(next_image), NULL);
    gtk_box_pack_start(GTK_BOX(button_box), button, FALSE, FALSE, 0);

    GtkWidget *separator1 = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_start(GTK_BOX(button_box), separator1, FALSE, FALSE, 10);

    GtkWidget *rotation_label = gtk_label_new("Angle de rotation:");
    gtk_box_pack_start(GTK_BOX(button_box), rotation_label, FALSE, FALSE, 0);

    rotation_entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(rotation_entry), "90");
    gtk_entry_set_max_length(GTK_ENTRY(rotation_entry), 10);
    gtk_box_pack_start(GTK_BOX(button_box), rotation_entry, FALSE, FALSE, 0);

    button = gtk_button_new_with_label("Rotate Image");
    g_signal_connect(button, "clicked", G_CALLBACK(rotate_image), NULL);
    gtk_box_pack_start(GTK_BOX(button_box), button, FALSE, FALSE, 0);

    button = gtk_button_new_with_label("Grayscale");
    g_signal_connect(button, "clicked", G_CALLBACK(apply_grayscale), NULL);
    gtk_box_pack_start(GTK_BOX(button_box), button, FALSE, FALSE, 0);

    button = gtk_button_new_with_label("Process image");
    g_signal_connect(button, "clicked", G_CALLBACK(on_process_image), NULL);
    gtk_box_pack_start(GTK_BOX(button_box), button, FALSE, FALSE, 0);

    button = gtk_button_new_with_label("Solve");
    g_signal_connect(button, "clicked", G_CALLBACK(execute_solver), window);
    gtk_box_pack_start(GTK_BOX(button_box), button, FALSE, FALSE, 0);

    GtkWidget *separator2 = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_start(GTK_BOX(button_box), separator2, FALSE, FALSE, 10);

    button = gtk_button_new_with_label("Exit");
    g_signal_connect_swapped(button, "clicked", G_CALLBACK(gtk_widget_destroy), window);
    gtk_box_pack_start(GTK_BOX(button_box), button, FALSE, FALSE, 0);

    gtk_paned_pack2(GTK_PANED(paned), image_widget, TRUE, FALSE);

    load_and_display_current_image();

    gtk_widget_show_all(window);
}

int main(int argc, char **argv)
{
    GtkApplication *app;
    int status;

    app = gtk_application_new("org.ocr.project", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    if (current_img_data != NULL) {
        free_img_data(current_img_data);
    }
    
    if (current_process_result != NULL) {
        if (current_process_result->img != NULL) {
            free(current_process_result->img->img);
            free(current_process_result->img);
        }
        if (current_process_result->words_length != NULL) {
            free(current_process_result->words_length);
        }
        free(current_process_result);
    }
    
    if (current_image_path != NULL) {
        free(current_image_path);
    }

    return status;
}