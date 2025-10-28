#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <math.h>
#include <stb_image.h>

#include "../preporcessor/src/defs.h"
#include "../preporcessor/src/filters.h"
#include "../preporcessor/src/process.h"
#include "../solver.h"

extern unsigned char *stbi_load(char const *filename, int *x, int *y, int *comp, int req_comp);
extern void stbi_image_free(void *retval_from_stbi_load);
extern void save_img(const char *output_path, struct img img);

#define MAX_DISPLAY_WIDTH 800
#define MAX_DISPLAY_HEIGHT 600

static GtkWidget *image_widget = NULL;
static GtkWidget *path_label = NULL;
static GtkWidget *rotation_entry = NULL;
static struct img *current_img_data = NULL;

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
        
        current_img_data = NULL;
        display_image_scaled("./output.png");
    }

void execute_solver()
{
    printf("execute\n");
    process_image(current_img_data);
    current_img_data = NULL;
    
    printf("process done\n");
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

    // Libérer la mémoire à la fin
    if (current_img_data != NULL) {
        free_img_data(current_img_data);
    }
    
    if (current_image_path != NULL) {
        free(current_image_path);
    }

    return status;
}
