# OCR Word Search Solver

Un logiciel OCR pour résoudre automatiquement des grilles de mots cachés
à partir d'images.

================================================================================
DESCRIPTION
================================================================================

Ce projet analyse une image contenant une grille de mots cachés, reconnaît
les caractères grâce à un réseau de neurones, et résout automatiquement la
grille en encerclant les mots trouvés.

================================================================================
INSTALLATION
================================================================================

1. Compilation du projet :

   make

2. Nettoyage des fichiers de compilation :

   make clean

================================================================================
UTILISATION
================================================================================

## Interface graphique

Lancer l'interface graphique :

    ./ui

Étapes d'utilisation :

1. Charger une image de grille de mots cachés
2. Prétraiter l'image (corrections automatiques)
3. Lancer la reconnaissance OCR
4. Visualiser la grille résolue
5. Sauvegarder le résultat

## Programme solver en ligne de commande

Format :

    ./solver <fichier_grille> <mot_à_chercher>

Exemples :

    ./solver grid.txt HORIZONTAL
    # Sortie : (0,0)(9,0)

    ./solver grid.txt VERTICAL
    # Sortie : (9,7)(9,0)

    ./solver grid.txt UNKNOWN
    # Sortie : Not found

## Format du fichier grille

Le fichier doit contenir les mots à chercher sur la première ligne,
suivis des lignes de la grille :

    HORIZONTAL VERTICAL
    DXRAHCLBGA
    DIKCILEOKC
    IGAJHYLYHI
    HGFGODTIOT
    GDLROWKBFR
    PLNRDNERGE
    JHAIDUAJGV
    UKGFFOLLEH
