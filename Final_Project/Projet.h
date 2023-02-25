#ifndef PROJET_H
#define PROJET_H

#define NBR_SEMESTRE 6        // Valeur dépendante de la base de donnée
#define NBR_TOPDEGRE 5        // A modifier pour choisir un top 3,5,10... de degré dans le fichier mainResults.txt
#define MINREQUIREDDISTANCE 3 // A modifier pour choisir la distance minimale dans le fichier Perequis.txt
// Les fichiers d'input
#define FIC_NOEUD "nodes.csv"
#define FIC_EDGE "edges.csv"
// Les fichiers d'output, présents dans le dossier "résultats"
#define FIC_OUTPUT "resultats/mainResults.txt"
#define FIC_OUTPUT_RELATION "resultats/AllUPGP.txt"
#define FIC_OUTPUT_REQUIRED "resultats/Prerequis.txt"
#define FIC_OUTPUT_DEPENDANCE "resultats/Dependance.txt"
#define FIC_OUTPUT_OTHERS "resultats/Others.txt"
// include classiques
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

// Structure de coordonnées
typedef struct _coord
{
    int x;
    int y;
} coord;
// Structure pour chacun des noeuds
typedef struct _nodes
{
    int indice;           // Indice du noeud 0<i<nbr_noeuds
    int degre;            // Nombre de voisins
    int semestrePlusTot;  // Semestre le plus tôt possible pour réaliser l'UP/GP
    int semestrePlusTard; // Semestre le plus tard pour réaliser l'UP/GP
    bool estGP;           // Savoir si c'est un GP
    int numeroGP;         // Chaque GP a un numéro 0<i<nbr_GP
    char nom[100];        // Son nom (100 est une valeure fiable même si la base de donnée change)
    coord *coo;           // Coordonnées en x et y du noeud
    struct _nodes *suivant;
} nodes;

// Pour créer la file
typedef struct _file
{
    int nombre;
    struct _file *suivant;

} File;
typedef struct _bordfile
{
    File *tete;
    File *queue;
} bordFile;


#endif



