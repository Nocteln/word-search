# 📚 Guide d'utilisation du réseau de neurones

## 🎯 Objectif

Le réseau de neurones détecte les lettres (a-z) à partir d'images 64x64 pixels en noir et blanc.

## 🔧 Fonctions principales à utiliser

### 1. **Charger le réseau de neurones entraîné**

```c
#include "neural_network/src/neural_network.h"
#include "neural_network/src/save_load.h"
#include "neural_network/src/train.h"

struct neural_network *n = load_network("./neural_network/network.bin");
```

Le fichier `network.bin` contient le réseau déjà entraîné. Il est prêt à l'emploi !

---

### 2. **Préparer une image pour la reconnaissance**

```c
// L'image doit être convertie en tableau de 64x64 doubles (0.0 ou 1.0)
double input[64 * 64];

// Convertir une image (PNG, JPG, etc.) en tableau 64x64
image_to_double64("chemin/vers/image.png", input);
```

**Important** : L'image doit être :

- En noir et blanc
- Les lettres noires sur fond blanc (ou inversé, selon l'entraînement)
- Idéalement déjà 64x64 pixels ou sera redimensionnée automatiquement

---

### 3. **Reconnaître une lettre - Méthode simple**

```c
// Retourne l'indice de la lettre détectée (0=a, 1=b, ..., 25=z)
int letter_index = classify(n, input);

// Convertir en caractère
char letter = 'a' + letter_index;

printf("Lettre détectée : %c\n", letter);
```

---

### 4. **Reconnaître une lettre - Méthode avancée (avec probabilités)**

```c
// Obtenir les probabilités pour chaque lettre (26 valeurs)
double *outputs = calculate_neural_network_outputs(n, input);

// La lettre la plus probable
int best_index = 0;
double best_score = outputs[0];

for (int i = 1; i < 26; i++) {
    if (outputs[i] > best_score) {
        best_score = outputs[i];
        best_index = i;
    }
}

printf("Lettre détectée : %c (score: %.2f%%)\n",
       'a' + best_index, best_score * 100);

// Afficher les 4 meilleures prédictions
printf("\nTop 4 prédictions :\n");
for (int i = 0; i < 26; i++) {
    if (outputs[i] > 0.1) { // Seuil de 10%
        printf("  %c : %.2f%%\n", 'a' + i, outputs[i] * 100);
    }
}

// IMPORTANT : Libérer la mémoire
free(outputs);
```

---

## 📋 Exemple complet d'utilisation

```c
#include "neural_network/src/neural_network.h"
#include "neural_network/src/save_load.h"
#include "neural_network/src/train.h"

int main() {
    // 1. Charger le réseau entraîné
    struct neural_network *n = load_network("./neural_network/network.bin");

    // 2. Préparer l'image
    double input[64 * 64];
    image_to_double64("letters/filtered/a_001.png", input);

    // 3. Reconnaître la lettre (méthode simple)
    int letter_index = classify(n, input);
    char letter = 'a' + letter_index;

    printf("Lettre détectée : %c\n", letter);

    // 4. Nettoyer (optionnel, le réseau reste en mémoire)
    drop_neural_network();

    return 0;
}
```

---

## 🗂️ Structure du réseau de neurones

Le réseau a cette architecture :

- **Entrée** : 64×64 = 4096 neurones (image aplatie)
- **Couche cachée 1** : 220 neurones
- **Couche cachée 2** : 60 neurones
- **Sortie** : 26 neurones (une pour chaque lettre a-z)

La sortie est un tableau de 26 valeurs entre 0 et 1, représentant la probabilité pour chaque lettre.

---

## 🔄 Format des données d'entrée

L'entrée est un tableau de **4096 doubles** (64×64 pixels) où :

- `0.0` = pixel blanc (fond)
- `1.0` = pixel noir (lettre)

Le tableau est organisé en **ligne par ligne** :

```
input[0...63]     = première ligne de pixels
input[64...127]   = deuxième ligne de pixels
...
input[4032...4095] = dernière ligne de pixels
```

---

## 📁 Fichiers importants

- **`neural_network/network.bin`** : Réseau pré-entraîné (à utiliser directement)
- **`neural_network/src/neural_network.h`** : Déclarations des fonctions principales
- **`neural_network/src/save_load.h`** : Pour charger/sauvegarder le réseau
- **`neural_network/src/train.h`** : Fonctions utilitaires (image_to_double64, etc.)

---

## ⚠️ Points d'attention

1. **Toujours charger le réseau avant de l'utiliser** :

   ```c
   struct neural_network *n = load_network("./neural_network/network.bin");
   ```

2. **Libérer la mémoire des outputs** :

   ```c
   double *outputs = calculate_neural_network_outputs(n, input);
   // ... utiliser outputs ...
   free(outputs); // ⚠️ NE PAS OUBLIER
   ```

3. **Format d'image** : Le réseau attend des images 64×64 en noir et blanc. Utiliser `image_to_double64()` pour convertir automatiquement.

4. **Chemin du fichier** : Selon où tu exécutes ton programme, le chemin vers `network.bin` peut changer. Ajuste-le si nécessaire.

---

## 🚀 Intégration dans ton projet

Pour utiliser le réseau de neurones dans ton code :

### Dans ton Makefile :

```makefile
# Ajouter les sources du réseau de neurones
NN_SRC = neural_network/src/neural_network.c \
         neural_network/src/save_load.c \
         neural_network/src/train.c \
         neural_network/src/activation_functions.c \
         neural_network/src/nodes_loss.c \
         neural_network/src/learn.c \
         neural_network/src/utils.c

# Inclure les headers
CFLAGS += -I./neural_network/src -I./stb

# Lier avec la librairie math
LDFLAGS += -lm
```

### Dans ton code source :

```c
#include "neural_network/src/neural_network.h"
#include "neural_network/src/save_load.h"
#include "neural_network/src/train.h"
```

---

## 🧪 Tester le réseau

Pour tester si le réseau fonctionne bien :

```c
// Tester avec plusieurs images
char *test_images[] = {
    "letters/filtered/a_001.png",
    "letters/filtered/b_001.png",
    "letters/filtered/z_001.png"
};

struct neural_network *n = load_network("./neural_network/network.bin");

for (int i = 0; i < 3; i++) {
    double input[64 * 64];
    image_to_double64(test_images[i], input);

    int result = classify(n, input);
    printf("Image %s -> Lettre détectée : %c\n",
           test_images[i], 'a' + result);
}
```

---

## 📞 Résumé ultra-rapide

```c
// 1. Charger
struct neural_network *n = load_network("./neural_network/network.bin");

// 2. Préparer
double input[64 * 64];
image_to_double64("mon_image.png", input);

// 3. Reconnaître
int lettre = classify(n, input);
printf("Lettre : %c\n", 'a' + lettre);
```

C'est tout ! 🎉
