# Comment utiliser les données de `process_image_with_data`

## Vue d'ensemble

La fonction `process_image_with_data()` retourne maintenant une structure `process_result` qui contient toutes les données extraites de l'image.

Internement, elle utilise `process_image_aux()` qui prend un paramètre optionnel `struct process_result *result`. Si ce paramètre n'est pas NULL, la fonction remplit la structure avec les données extraites.

```c
struct process_result {
  struct img *img;                // Image traitée
  struct box ***words_and_grid;   // Tableau contenant les mots et la grille
  int *words_length;              // Longueur de chaque mot
  int width;                      // Largeur de la grille
  int length;                     // Longueur/hauteur de la grille
  int nbwords;                    // Nombre de mots détectés
};
```

## Architecture du code

- `process_image_aux(struct img *img, struct process_result *result)` : Fonction principale qui effectue le traitement
  - Si `result` est `NULL`, elle ne fait que le traitement sans stocker les données
  - Si `result` n'est pas `NULL`, elle remplit la structure avec les données extraites
- `process_image(struct img *img)` : Wrapper qui appelle `process_image_aux(img, NULL)` et libère l'image

- `process_image_with_data(struct img *img)` : Alloue une structure `process_result`, appelle `process_image_aux()` et retourne le résultat

```c
struct process_result {
  struct img *img;                // Image traitée
  struct box ***words_and_grid;   // Tableau contenant les mots et la grille
  int *words_length;              // Longueur de chaque mot
  int width;                      // Largeur de la grille
  int length;                     // Longueur/hauteur de la grille
  int nbwords;                    // Nombre de mots détectés
};
```

## Utilisation dans ui.c

### 1. Appeler la fonction

```c
struct process_result *result = process_image_with_data(current_img_data);
```

### 2. Accéder aux données

```c
if (result != NULL) {
    // Nombre de mots détectés
    int nb_mots = result->nbwords;

    // Dimensions de la grille
    int largeur = result->width;
    int hauteur = result->length;

    // Parcourir tous les mots
    for (int i = 0; i < result->nbwords; i++) {
        int longueur_mot = result->words_length[i];
        printf("Mot %d (longueur: %d): ", i, longueur_mot);

        // Parcourir toutes les lettres du mot
        for (int j = 0; j < longueur_mot; j++) {
            struct box lettre = result->words_and_grid[0][i][j];
            printf("Lettre à (%d, %d) ", lettre.min_x, lettre.min_y);
        }
        printf("\n");
    }

    // Parcourir la grille
    for (int i = 0; i < result->length; i++) {
        for (int j = 0; j < result->width; j++) {
            struct box case_grille = result->words_and_grid[1][i][j];
            // Traiter chaque case de la grille
        }
    }
}
```

### 3. Libérer la mémoire

**IMPORTANT** : Après utilisation, vous devez libérer la mémoire :

```c
if (result != NULL) {
    // Libérer l'image
    if (result->img != NULL) {
        free(result->img->img);
        free(result->img);
    }

    // Libérer words_length
    if (result->words_length != NULL) {
        free(result->words_length);
    }

    // Libérer words_and_grid (structure complexe)
    if (result->words_and_grid != NULL) {
        // Libérer les mots (première dimension)
        if (result->words_and_grid[0] != NULL) {
            for (int i = 0; i < result->nbwords; i++) {
                if (result->words_and_grid[0][i] != NULL) {
                    free(result->words_and_grid[0][i]);
                }
            }
            free(result->words_and_grid[0]);
        }

        // Libérer la grille (deuxième dimension)
        if (result->words_and_grid[1] != NULL) {
            for (int i = 0; i < result->length; i++) {
                if (result->words_and_grid[1][i] != NULL) {
                    free(result->words_and_grid[1][i]);
                }
            }
            free(result->words_and_grid[1]);
        }

        free(result->words_and_grid);
    }

    // Libérer la structure principale
    free(result);
}
```

## Exemple d'utilisation dans execute_solver()

```c
void execute_solver()
{
    printf("execute\n");

    current_process_result = process_image_with_data(current_img_data);
    current_img_data = NULL;

    if (current_process_result != NULL) {
        printf("=== Données extraites ===\n");
        printf("Nombre de mots: %d\n", current_process_result->nbwords);
        printf("Grille: %dx%d\n", current_process_result->width, current_process_result->length);

        // Utiliser les données pour le solver
        // Par exemple, passer les données à votre fonction de résolution
        solve_grid(
            current_process_result->words_and_grid,
            current_process_result->words_length,
            current_process_result->width,
            current_process_result->length,
            current_process_result->nbwords
        );
    }

    printf("process done\n");
}
```

## Structure de words_and_grid

`words_and_grid` est un tableau à 3 dimensions :

- `words_and_grid[0]` : contient les mots
  - `words_and_grid[0][i]` : le i-ème mot
  - `words_and_grid[0][i][j]` : la j-ème lettre du i-ème mot
- `words_and_grid[1]` : contient la grille
  - `words_and_grid[1][i]` : la i-ème ligne de la grille
  - `words_and_grid[1][i][j]` : la case (i,j) de la grille

Chaque élément est une `struct box` avec les coordonnées de la boîte englobante :

```c
struct box {
  int min_x;  // Coordonnée x minimale
  int max_x;  // Coordonnée x maximale
  int min_y;  // Coordonnée y minimale
  int max_y;  // Coordonnée y maximale
};
```

## Notes importantes

1. **Stockage global** : La variable `current_process_result` est maintenant disponible globalement dans `ui.c` et persiste entre les appels
2. **Libération** : Pensez toujours à libérer l'ancien résultat avant d'en créer un nouveau
3. **Null checks** : Vérifiez toujours que les pointeurs ne sont pas NULL avant de les utiliser
4. **Thread safety** : Cette implémentation n'est pas thread-safe, utilisez dans le thread principal GTK uniquement
