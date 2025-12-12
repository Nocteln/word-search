#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Structure minimale pour lire l'ancien format
typedef struct {
    int number_of_layers;
    int activation_id;
    int loss_id;
} NetworkHeader;

typedef struct {
    int nodes_in;
    int nodes_out;
} LayerInfo;

int convert_network(const char *old_file, const char *new_file) {
    FILE *f_old = fopen(old_file, "rb");
    if (!f_old) {
        fprintf(stderr, "Erreur: impossible d'ouvrir %s\n", old_file);
        return -1;
    }

    FILE *f_new = fopen(new_file, "wb");
    if (!f_new) {
        fprintf(stderr, "Erreur: impossible de créer %s\n", new_file);
        fclose(f_old);
        return -1;
    }

    // Lire et copier le header (identique dans les deux formats)
    NetworkHeader header;
    fread(&header.number_of_layers, sizeof(int), 1, f_old);
    fread(&header.activation_id, sizeof(int), 1, f_old);
    fread(&header.loss_id, sizeof(int), 1, f_old);

    fwrite(&header.number_of_layers, sizeof(int), 1, f_new);
    fwrite(&header.activation_id, sizeof(int), 1, f_new);
    fwrite(&header.loss_id, sizeof(int), 1, f_new);

    printf("Conversion du réseau: %d couches\n", header.number_of_layers);

    // Convertir chaque couche
    for (int l = 0; l < header.number_of_layers; l++) {
        LayerInfo layer;
        fread(&layer.nodes_in, sizeof(int), 1, f_old);
        fread(&layer.nodes_out, sizeof(int), 1, f_old);

        fwrite(&layer.nodes_in, sizeof(int), 1, f_new);
        fwrite(&layer.nodes_out, sizeof(int), 1, f_new);

        printf("  Couche %d: %d entrées -> %d sorties\n", 
               l + 1, layer.nodes_in, layer.nodes_out);

        // Convertir les poids de double vers float
        for (int i = 0; i < layer.nodes_in; i++) {
            for (int j = 0; j < layer.nodes_out; j++) {
                double weight_d;
                fread(&weight_d, sizeof(double), 1, f_old);
                float weight_f = (float)weight_d;
                fwrite(&weight_f, sizeof(float), 1, f_new);
            }
        }

        // Convertir les biais de double vers float
        for (int j = 0; j < layer.nodes_out; j++) {
            double bias_d;
            fread(&bias_d, sizeof(double), 1, f_old);
            float bias_f = (float)bias_d;
            fwrite(&bias_f, sizeof(float), 1, f_new);
        }

        int total_weights = layer.nodes_in * layer.nodes_out;
        int old_size = (total_weights + layer.nodes_out) * sizeof(double);
        int new_size = (total_weights + layer.nodes_out) * sizeof(float);
        printf("    Poids+biais: %d bytes -> %d bytes (%.1f%%)\n", 
               old_size, new_size, (100.0 * new_size) / old_size);
    }

    fclose(f_old);
    fclose(f_new);

    printf("\n✓ Conversion terminée avec succès!\n");
    printf("  Fichier source: %s\n", old_file);
    printf("  Fichier destination: %s\n", new_file);

    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <ancien_fichier.nn> <nouveau_fichier.nn>\n", argv[0]);
        fprintf(stderr, "\nExemple:\n");
        fprintf(stderr, "  %s model_old.nn model_new.nn\n", argv[0]);
        return 1;
    }

    const char *old_file = argv[1];
    const char *new_file = argv[2];

    // Vérifier que les fichiers sont différents
    if (strcmp(old_file, new_file) == 0) {
        fprintf(stderr, "Erreur: le fichier source et destination doivent être différents\n");
        return 1;
    }

    return convert_network(old_file, new_file);
}
