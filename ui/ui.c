#include <gtk/gtk.h>
#include <stdio.h>

static GtkWidget *image_widget = NULL; // pour changer l'image après import

void import_image(GtkWidget *widget, gpointer user_data)
{
    GtkWidget *dialog;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
    gint res;

    // Création de la fenêtre de sélection de fichier
    dialog = gtk_file_chooser_dialog_new("Sélectionner une image",
                                         GTK_WINDOW(user_data),
                                         action,
                                         "_Annuler", GTK_RESPONSE_CANCEL,
                                         "_Ouvrir", GTK_RESPONSE_ACCEPT,
                                         NULL);

    // Filtre pour n'afficher que les images
    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_set_name(filter, "Images");
    gtk_file_filter_add_mime_type(filter, "image/png");
    gtk_file_filter_add_mime_type(filter, "image/jpeg");
    gtk_file_filter_add_mime_type(filter, "image/jpg");
    gtk_file_filter_add_mime_type(filter, "image/webp");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

    res = gtk_dialog_run(GTK_DIALOG(dialog));
    if (res == GTK_RESPONSE_ACCEPT)
    {
        char *filename;
        GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
        filename = gtk_file_chooser_get_filename(chooser);

        // Mettre à jour l'image affichée
        gtk_image_set_from_file(GTK_IMAGE(image_widget), filename);
        g_free(filename);
    }

    gtk_widget_destroy(dialog);
}

void execute_solver()
{
    printf("execute");
}

static void activate(GtkApplication *app, gpointer user_data)
{
    GtkWidget *window;
    GtkWidget *paned;
    GtkWidget *button_box;
    GtkWidget *button;

    // Image par défaut
    image_widget = gtk_image_new_from_file("./gtk-logo.png");

    // Fenêtre principale
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Word Search Solver");
    gtk_window_set_default_size(GTK_WINDOW(window), 1250, 750);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);

    // Paned → deux zones redimensionnables
    paned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_container_add(GTK_CONTAINER(window), paned);

    // Zone boutons
    button_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_valign(button_box, GTK_ALIGN_START);
    gtk_paned_pack1(GTK_PANED(paned), button_box, FALSE, FALSE);

    // Bouton Import
    button = gtk_button_new_with_label("Import Image");
    g_signal_connect(button, "clicked", G_CALLBACK(import_image), window);
    gtk_box_pack_start(GTK_BOX(button_box), button, FALSE, FALSE, 0);

    button = gtk_button_new_with_label("Solve");
    g_signal_connect(button, "clicked", G_CALLBACK(execute_solver), window);
    gtk_box_pack_start(GTK_BOX(button_box), button, FALSE, FALSE, 0);

    // Bouton Exit
    button = gtk_button_new_with_label("Exit");
    g_signal_connect_swapped(button, "clicked", G_CALLBACK(gtk_widget_destroy), window);
    gtk_box_pack_start(GTK_BOX(button_box), button, FALSE, FALSE, 0);

    // Zone image
    gtk_paned_pack2(GTK_PANED(paned), image_widget, TRUE, FALSE);

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

    return status;
}

