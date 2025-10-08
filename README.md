# OCR Word Search Solver

Un logiciel de reconnaissance optique de caractères (OCR) capable de résoudre automatiquement des grilles de mots cachés à partir d'images.

## 📋 Table des matières

- [Description](#description)
- [Fonctionnalités](#fonctionnalités)
- [Prérequis](#prérequis)
- [Installation](#installation)
- [Utilisation](#utilisation)
- [Structure du projet](#structure-du-projet)
- [Exemples](#exemples)
- [Développement](#développement)
- [Contributeurs](#contributeurs)

## 🎯 Description

Ce projet implémente un système OCR complet capable de :
1. **Analyser** une image contenant une grille de mots cachés et sa liste de mots
2. **Prétraiter** l'image (débruitage, rotation, amélioration du contraste)
3. **Détecter** et découper automatiquement la grille et la liste de mots
4. **Reconnaître** les caractères grâce à un réseau de neurones entraîné
5. **Résoudre** la grille de mots cachés
6. **Afficher** et sauvegarder le résultat avec les mots encerclés

## ✨ Fonctionnalités

### Traitement d'image
- ✅ Chargement d'images dans les formats standards
- ✅ Conversion en niveaux de gris et noir & blanc
- ✅ Redressement manuel et automatique des images
- ✅ Élimination du bruit parasite
- ✅ Renforcement des contrastes

### Reconnaissance optique
- ✅ Détection automatique de la position de la grille
- ✅ Détection de la liste de mots
- ✅ Segmentation des caractères individuels
- ✅ Réseau de neurones pour la reconnaissance de caractères
- ✅ Support des majuscules et minuscules

### Résolution
- ✅ Algorithme de recherche de mots dans toutes les directions
- ✅ Interface graphique intuitive
- ✅ Sauvegarde des résultats

### Niveaux de difficulté supportés
- **Niveau 1** : Images parfaites avec grilles bordurées
- **Niveau 2** : Images avec défauts légers (flou, bruit, rotation)
- **Niveau 3** : Images complexes avec éléments parasites

## 🔧 Prérequis

- **Compilateur C** : GCC avec support des options `-Wall -Wextra`
- **Système** : Linux (testé sur l'environnement EPITA)
- **Bibliothèques** : Toutes les bibliothèques standard installées sur les machines EPITA

## 🚀 Installation

```bash
# Cloner le projet
git clone [URL_DU_DEPOT]
cd ocr-word-search-solver

# Compiler le projet
make all

# Nettoyer les fichiers de compilation
make clean
```

## 💻 Utilisation

### Interface graphique principale
```bash
./ocr_solver
```

### Programme solver en ligne de commande
```bash
./solver <fichier_grille> <mot_à_chercher>
```

**Exemple :**
```bash
./solver examples/grid.txt HORIZONTAL
# Sortie : (0,0)(9,0)

./solver examples/grid.txt VERTICAL
# Sortie : (9,7)(9,0)

./solver examples/grid.txt unknown
# Sortie : Not found
```

### Format de fichier grille
```
HORIZONTAL
DXRAHCLBGA
DIKCILEOKC
IGAJHYLYHI
HGFGODTIOT
GDLROWKBFR
PLNRDNERGE
JHAIDUAJGV
UKGFFOLLEH
```

### Interface graphique
1. **Charger une image** : Sélectionnez votre image de grille de mots cachés
2. **Prétraitement** : Appliquez les corrections automatiques ou manuelles
3. **Traitement OCR** : Lancez la reconnaissance des caractères
4. **Résolution** : Visualisez la grille résolue avec les mots encerclés
5. **Sauvegarde** : Exportez le résultat dans un format image standard

## 📁 Structure du projet

```
ocr-word-search-solver/
├── src/                    # Code source principal
│   ├── main.c             # Point d'entrée principal
│   ├── ocr/               # Module OCR
│   ├── neural_network/    # Réseau de neurones
│   ├── image_processing/  # Traitement d'image
│   ├── solver/            # Algorithmes de résolution
│   └── gui/               # Interface graphique
├── include/               # Fichiers d'en-tête
├── examples/              # Images et grilles d'exemple
├── training_data/         # Données d'entraînement du réseau
├── tests/                 # Fichiers de tests
├── Makefile              # Fichier de compilation
├── README.md             # Ce fichier
├── AUTHORS               # Liste des contributeurs
└── rapport_*.pdf         # Rapports de projet
```

## 📸 Exemples

### Niveau 1 - Image parfaite
![Exemple niveau 1](examples/level1_example.png)

### Niveau 2 - Image avec défauts
![Exemple niveau 2](examples/level2_example.png)

### Niveau 3 - Image complexe
![Exemple niveau 3](examples/level3_example.png)

## 🛠️ Développement

### Compilation avec options de debug
```bash
make debug
```

### Tests mémoire
```bash
# Utilisation de valgrind
valgrind --leak-check=full ./ocr_solver

# Ou avec gcc sanitizer
gcc -fsanitize=address -g src/*.c -o ocr_solver_debug
```

## 📊 Algorithmes utilisés

- **Réseau de neurones** : Perceptron multicouche avec rétropropagation
- **Prétraitement** : Filtres de Sobel, seuillage adaptatif
- **Détection** : Transformée de Hough pour les lignes
- **Résolution** : Recherche en profondeur dans 8 directions

## 🤝 Contributeurs

Voir le fichier `AUTHORS` pour la liste complète des contributeurs.

## 📄 Licence

Ce projet est développé dans le cadre du cursus EPITA - S3 2024/2025.

## 🐛 Signalement de bugs

Pour signaler un bug ou proposer une amélioration, veuillez créer une issue sur ce dépôt.

## 📚 Documentation supplémentaire


---

**Note** : Ce projet respecte les standards de codage EPITA avec des lignes de maximum 80 caractères et tous les identifiants en anglais.
