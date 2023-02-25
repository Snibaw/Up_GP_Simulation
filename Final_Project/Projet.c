#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include "Projet.h"



// Fonction basique pour enfiler, utile pour calculer le plus long chemin
void enfiler(bordFile *f, int s)
{
    File *elt = (File *)malloc(sizeof(File));
    elt->nombre = s;
    elt->suivant = NULL;
    if (f->tete == NULL) // Si tête null alors queue aussi
    {
        f->tete = elt;
        f->queue = elt;
    }
    else // S'il y a déjà un élement
    {
        f->queue->suivant = elt; // Le dernier pointe vers "s"
        f->queue = elt;          // La queue pointe vers "s"
    }
}
// Fonction classique pour défiler, utilie pour calculer le plus long chemin
int defiler(bordFile *f)
{
    if (f->tete == NULL) // Si la file est vide
    {
        return -1;
    }
    else // S'il y a un élement
    {
        File *tempo = f->tete;      // On sauvegarde l'élément qu'on dépile
        f->tete = f->tete->suivant; // On dépile l'elt
        int nombre = tempo->nombre; // On sauvegarde le nombre;
        free(tempo);
        return nombre;
    }
}

// Fonction visiter analyse le graph et parcourt les sommets
void visiter(nodes **liste_noeud, bordFile *f, int *tabMarque, int s, int *nbr_isole, int *nbr_cluster)
{
    // On créer la file
    f = (bordFile *)malloc(sizeof(bordFile));
    f->tete = NULL;
    f->queue = NULL;
    // Enfillage du premier elt
    enfiler(f, s);
    tabMarque[s] = 1;
    bool cluster = false;   // Un cluster est un groupe d'UP/GP lié dont le nombre est > 1
    while (f->tete != NULL) // Tant que la file n'est pas vide
    {

        s = defiler(f);
        if (liste_noeud[s]->suivant != NULL) // Si l'elt a au moins un voison
        {
            cluster = true; // Il a au moins un voisin
            nodes *tempo = liste_noeud[s];
            while (tempo->suivant != NULL) // On parcourt les voisins
            {
                int valeur = tempo->suivant->indice;
                if (tabMarque[valeur] == 0) // S'ils sont pas marqués, on les enfile.
                {
                    enfiler(f, valeur);
                    tabMarque[valeur] = 1;
                }
                tempo = tempo->suivant; // On continue de parcourir
            }
        }
        else
        {
            *nbr_isole = *nbr_isole + 1; // S'il n'a pas de voisin, il est isolé
        }
    }
    if (cluster) // La boucle while précédente a parcouru un cluster
    {
        *nbr_cluster = *nbr_cluster + 1;
    }
    free(f);
}
// Fonction permettant de savoir si c'est un UP ou un GP
bool estGP(char nom[100])
{
    int nbr_maj = 0;
    int taille = strlen(nom);        // Besoin de la taille pour savoir la proportion de majuscule
    for (int i = 0; i < taille; i++) // Parcourt des caractères du string un à un
    {
        if ('A' <= nom[i] && nom[i] <= 'Z') // Si le caractère est une majuscule
            nbr_maj++;
    }
    if (nbr_maj > taille * 0.5) // 50 % de majuscule valide le fait que ce soit un GP
    {                           // Si on prend plus (ex:80) alors certains GP contenant des chiffres seront vu comme un UP
        return true;
    }
    return false;
}
nodes **LireNoeuds(FILE *fichier, int *nbr_noeuds, int *nbr_GP)
{
    int numeroGP = 0;
    // x,y -> coordonnées, j est utile pour parcourir la liste de noeuds et lui attribuer des valeurs
    int x, y, j = 0;
    char nom[100];
    fscanf(fichier, "%d", nbr_noeuds); // Convention d'écriture
    // Initialisation de la liste de noeuds
    nodes **liste_noeuds = (nodes **)malloc(*nbr_noeuds * sizeof(nodes *));
    for (int i = 0; i < *nbr_noeuds; i++)
        liste_noeuds[i] = (nodes *)malloc(sizeof(nodes));
    // Lecture du fichier ligne par ligne. %[^,] = lecture jusqu'à virgule
    while (fscanf(fichier, " %[^,],%d,%d", nom, &x, &y) == 3)
    {

        // On replit notre noeud avec les valeurs contenues dans le fichier
        // Les valeurs qui ne sont pas dans le fichier sont initialisées à des valeurs de base
        coord *coo_nodes = (coord *)malloc(sizeof(coord));
        strcpy(liste_noeuds[j]->nom, nom);
        liste_noeuds[j]->suivant = NULL;
        liste_noeuds[j]->indice = j;
        liste_noeuds[j]->coo = coo_nodes;
        liste_noeuds[j]->coo->x = x;
        liste_noeuds[j]->coo->y = y;
        liste_noeuds[j]->degre = 0;
        liste_noeuds[j]->semestrePlusTot = 5;
        liste_noeuds[j]->semestrePlusTard = 10;
        liste_noeuds[j]->estGP = false;
        liste_noeuds[j]->numeroGP = -1; // S'il n'appartient pas à un GP alors son numéro ne doit pas être pris en compte (-1)
        if (estGP(nom))                 // Si c'est un GP
        {
            *nbr_GP = *nbr_GP + 1;
            liste_noeuds[j]->estGP = true;        // On le précise dans la structure noeud
            liste_noeuds[j]->numeroGP = numeroGP; // On lui attribu un numéro
            numeroGP++;
        }
        j++;
    }
    return liste_noeuds; // On retourne la liste remplie
}
int AjouterNoeud(nodes *noeud1, nodes *noeud2, nodes **liste_noeuds, int *nbr_aretes)
{
    int deg = 0;
    int doublon = 0;                             // Vérifie que l'arête n'existe pas déjà
    nodes *tempo = liste_noeuds[noeud1->indice]; // On va parcourir les voisins du noeuds1
    while (tempo->suivant != NULL)               // Tant qu'on a pas atteind le dernier voisin
    {
        tempo = tempo->suivant;
        if (tempo->indice == noeud2->indice) // Si le noeud2 fait déjà parti des voisins du noeud1
        {
            doublon = 1; // Alors l'arête est un doublon
            break;
        }
    }
    if (doublon == 0) // Si l'arête n'est pas un doublon
    {
        // ON ajoute le noeud2 en tant que voisin du noeud1
        tempo->suivant = noeud2;
        // De même que pour noeud1
        tempo = liste_noeuds[noeud2->indice];
        while (tempo->suivant != NULL)
        {
            tempo = tempo->suivant;
        }
        tempo->suivant = noeud1;
        // Les deux noeuds ont un degré augmenté car on 1 voisin en plus
        liste_noeuds[noeud1->indice]->degre++;
        liste_noeuds[noeud2->indice]->degre++;
        // Si le noeud est un GP alors tous ses UP qui lui sont liés auront son numéro de GP
        // Si un GP est lié à un GP ( ce qui arrive 1 fois ) alors on ne fait rien
        if (liste_noeuds[noeud1->indice]->estGP && !liste_noeuds[noeud2->indice]->estGP)
            liste_noeuds[noeud2->indice]->numeroGP = liste_noeuds[noeud1->indice]->numeroGP;
        if (liste_noeuds[noeud2->indice]->estGP && !liste_noeuds[noeud1->indice]->estGP)
            liste_noeuds[noeud1->indice]->numeroGP = liste_noeuds[noeud2->indice]->numeroGP;
        // On regarde si un des deux noeuds n'a pas atteint le degré maximum
        if (liste_noeuds[noeud1->indice]->degre > deg)
            deg = liste_noeuds[noeud1->indice]->degre;
        if (liste_noeuds[noeud2->indice]->degre > deg)
            deg = liste_noeuds[noeud2->indice]->degre;
        // L'arête est valide (les deux noeuds existent, ce n'est pas un doublon) [INTEGRITE FICHIER]
        *nbr_aretes = *nbr_aretes + 1;
    }
    return deg;
}
int Lier(coord *n1, coord *n2, nodes **liste_noeuds, int nbr_noeuds, int *nbr_aretes)
{
    int deg = 0;
    int trouver = 0; // Condition pour savoir si les noeuds existent (sont dans la liste) [INTEGRITE FICHIER]
    nodes *noeud1 = (nodes *)malloc(sizeof(nodes));
    nodes *noeud2 = (nodes *)malloc(sizeof(nodes));
    for (int i = 0; i < nbr_noeuds; i++)
    {
        // On cherche selon les coordonnées de chaque noeuds
        if (liste_noeuds[i]->coo->x == n1->x && liste_noeuds[i]->coo->y == n1->y)
        {
            // Si trouvé alors on copie les informations dans noeud1
            noeud1->coo = liste_noeuds[i]->coo;
            noeud1->indice = liste_noeuds[i]->indice;
            strcpy(noeud1->nom, liste_noeuds[i]->nom);
            noeud1->suivant = NULL;
            trouver = trouver + 1;
        }
        // De même que pour noeud1
        else if (liste_noeuds[i]->coo->x == n2->x && liste_noeuds[i]->coo->y == n2->y)
        {
            noeud2->coo = liste_noeuds[i]->coo;
            noeud2->indice = liste_noeuds[i]->indice;
            strcpy(noeud2->nom, liste_noeuds[i]->nom);
            noeud2->suivant = NULL;
            trouver = trouver + 1;
        }
        if (trouver == 2) // Si on a déjà trouvé les deux noeuds, inutile de continuer la boucle
            break;
    }
    if (trouver == 2) // Tous les noeuds de l'arête existent
    {                 // On ajoute noeud1 en tant que voisin de noeud2 et noeud2 en tant que voisin de noeud1
        deg = AjouterNoeud(noeud1, noeud2, liste_noeuds, nbr_aretes);
    }
    return deg;
}
int LireAretes(FILE *fichier, nodes **liste_noeuds, int nbr_noeuds, int *nbr_aretes)
{
    int deg_Max = 0;
    char *tempo;                                                       // Utile pour manipuler strtok
    char n1Read[10], n2Read[10];                                       // Saut de la première ligne
    while (fscanf(fichier, " [%[^][]],[%[^][]]", n1Read, n2Read) == 2) // Lecture en évitant les caractères "[" "]"
    {
        coord *n1 = (coord *)malloc(sizeof(coord));
        coord *n2 = (coord *)malloc(sizeof(coord));
        n1->x = atoi(strtok(n1Read, ",")); // Stockage des coordonnées en x
        tempo = strtok(NULL, ",");         // Saut de l'espace empêchant le stockage des coordonnées en y
        n1->y = atoi(strtok(tempo, " "));  // Stockage coordonnées en y
        // De même que pour n1
        n2->x = atoi(strtok(n2Read, ","));
        tempo = strtok(NULL, ",");
        n2->y = atoi(strtok(tempo, " "));
        if (n1->x != n2->x || n1->y != n2->y) // Si l'arrête ne relie pas le noeud à lui même [INTEGRITE FICHIER]
        {
            int deg = Lier(n1, n2, liste_noeuds, nbr_noeuds, nbr_aretes);
            if (deg > deg_Max)
                deg_Max = deg;
        }
        free(n1);
        free(n2);
    }
    return deg_Max;
}
void ActualiserDistance(nodes **liste_noeuds, int *distance, int *indiceArrive, bool *visiter, int i, int debut, int fin)
{
    if (visiter[i - debut]) // Si noeud déjà visité
        return;
    visiter[i - debut] = true; // Sinon on le note comme visité
    nodes *tempo = liste_noeuds[i];
    while (tempo->suivant != NULL) // On parcours ses voisins
    {
        tempo = tempo->suivant;
        if (tempo->indice >= debut && tempo->indice < fin) // Si les indices de ses voisins sont dans la zone cherché
        {
            if (!visiter[tempo->indice - debut]) // S'il n'est pas visité
            {
                ActualiserDistance(liste_noeuds, distance, indiceArrive, visiter, tempo->indice, debut, fin); // On actualise la distance (récurssif)
            }
            if (distance[tempo->indice - debut] + 1 > distance[i - debut]) // Si on trouve un chemin plus long
            {
                distance[i - debut] = distance[tempo->indice - debut] + 1; // On lui donne la distance plus longue
                indiceArrive[i - debut] = tempo->indice;                   // On sauvegarde l'indice du dernier UP/GP parcouru pour l'écriture
            }
        }
    }
}
int *PlusLongCheminPrecedence(nodes **liste_noeuds, int nbr_noeuds, FILE *fichier)
{
    // On borne la recherche sur un semestre, une année ... d'où fin et début qui sont les indices max et min
    int *values = (int *)malloc(3 * sizeof(int));
    values[0] = 0; // Correspond à la distance max
    values[1] = 0; // Correspond au noeud de départ pour trouver le chemin le plus long
    values[2] = 0; // Correspond au noeud d'arrivé pour trouver le chemin le plus long
    // Initialisation à 0 des tableaux utiles
    int *distance = (int *)malloc(nbr_noeuds * sizeof(int)); // Correspond à la distance de chaque noeud
    // Contient pour chaque noeud de départ i, l'indice du noeud d'arrivé pour le chemin le plus long
    int *indiceArrive = (int *)malloc(nbr_noeuds * sizeof(int));
    bool *visiter = (bool *)malloc(nbr_noeuds * sizeof(bool)); // Liste des noeuds visités, 1 si oui, 0 si non
    for (int i = 0; i < nbr_noeuds; i++)
    {
        distance[i] = 0;
        indiceArrive[i] = 0;
        visiter[i] = false;
    }
    // On parcourt tous les indices correspondant aux critères de recherche
    for (int i = 0; i < nbr_noeuds; i++)
    {
        if (!visiter[i]) // S'ils n'ont pas déjà été visités
        {                // On actualise la distance
            ActualiserDistance(liste_noeuds, distance, indiceArrive, visiter, i, 0, nbr_noeuds);
        }
    }
    values[0] = 0; // Distance max
    for (int i = 0; i < nbr_noeuds; i++)
    {
        if (distance[i] > values[0]) // Si on trouve une distance supérieur à la distance max
        {
            values[0] = distance[i]; // Alors c'est la nouvelle distance max
            values[1] = i;           // On sauvegarde l'indice du noeud de départ
        }
    }
    // Ecriture des informations dans mainResults
    fprintf(fichier, "\nLe chemin le plus long pour tout le programme est : %d. Passant par :\n", values[0]);
    int indiceTempo = values[1];
    fprintf(fichier, "DEGRE  INDICE  X   Y    SMSTRE plus tôt   SMSTRE plus tard                  NOM\n%-2d      %-3d    %d   %-10d   %-15d  %-10d  %s\n", liste_noeuds[indiceTempo]->degre, liste_noeuds[indiceTempo]->indice, liste_noeuds[indiceTempo]->coo->x, liste_noeuds[indiceTempo]->coo->y, liste_noeuds[indiceTempo]->semestrePlusTot, liste_noeuds[indiceTempo]->semestrePlusTard, liste_noeuds[indiceTempo]->nom);
    while (indiceArrive[indiceTempo] != 0)
    {
        fprintf(fichier, "%-2d      %-3d    %d   %-10d   %-15d  %-10d  %s\n", liste_noeuds[indiceArrive[indiceTempo]]->degre, liste_noeuds[indiceArrive[indiceTempo]]->indice, liste_noeuds[indiceArrive[indiceTempo]]->coo->x, liste_noeuds[indiceArrive[indiceTempo]]->coo->y, liste_noeuds[indiceArrive[indiceTempo]]->semestrePlusTot, liste_noeuds[indiceArrive[indiceTempo]]->semestrePlusTard, liste_noeuds[indiceArrive[indiceTempo]]->nom);
        indiceTempo = indiceArrive[indiceTempo];
    }
    free(distance);
    free(indiceArrive);
    free(visiter);
    return values;
}

void TrouverPrerequis(nodes **liste_noeuds, int nbr_noeuds, int deg_Max, FILE *fichier_resultats, FILE *fichier_relations, FILE *fichier)
{
    // Ecriture des UP dont les prérequis ont une distance >= MINREQUIREDDISTANCE
    fprintf(fichier, "Les arêtes dont la distance (en semestre) entre les 2 noeuds est supérieur à : %d (modifiable via MINREQUIREDDISTANCE)\n", MINREQUIREDDISTANCE);
    fprintf(fichier, "DIST (SMSTR)  i1                  nom1                         i2                 nom2\n");
    for (int i = 0; i < nbr_noeuds; i++) // Pour chaque noeuds
    {
        nodes *tempo = liste_noeuds[i]->suivant; // On parcourt les voisins
        while (tempo != NULL)
        {
            if (tempo->coo->x - liste_noeuds[i]->coo->x >= MINREQUIREDDISTANCE) // Si la distance entre le noeud et le voisin est supérieur à celle minimum on l'écrit
                fprintf(fichier, "     %-8d %-6d %-38s    %-3d    %s\n", tempo->coo->x - liste_noeuds[i]->coo->x, liste_noeuds[i]->indice, liste_noeuds[i]->nom, tempo->indice, tempo->nom);
            tempo = tempo->suivant;
        }
    }
    // Ecriture des UP / GP critiques
    fprintf(fichier, "\nLes UP/GP qui sont critiques (doivent être validé dans le semestre suivant pour pas modifier la durée du cycle ISMIN) sont: \n");
    fprintf(fichier, "DEGRE  INDICE  X   Y    SMSTRE plus tôt   SMSTRE plus tard                  NOM\n");
    for (int i = 0; i < nbr_noeuds; i++)
    {
        if (liste_noeuds[i]->semestrePlusTard - liste_noeuds[i]->semestrePlusTot <= 1) // la propriété "critique" dépend du semestre au plus tôt et celui au plus tard
            fprintf(fichier, "  %-2d      %-3d  %d   %-10d   %-15d  %-10d  %s\n", liste_noeuds[i]->degre, liste_noeuds[i]->indice, liste_noeuds[i]->coo->x, liste_noeuds[i]->coo->y, liste_noeuds[i]->semestrePlusTot, liste_noeuds[i]->semestrePlusTard, liste_noeuds[i]->nom);
    }
    fprintf(fichier, "\nLes chemins critiques sont: \n");
    // Les chemins critiques sont plus difficiles à identifier car ils dépendent de plusieurs UP/GP
    for (int i = 0; i < nbr_noeuds; i++)
    {
        nodes *precedent = liste_noeuds[i];
        nodes *tempo = precedent->suivant;
        int chemin = false;
        if (precedent->semestrePlusTard - precedent->semestrePlusTot <= 1) // On a trouvé un UP/GP critique
        {
            while (tempo != NULL)
            {
                if (liste_noeuds[tempo->indice]->semestrePlusTard - liste_noeuds[tempo->indice]->semestrePlusTot <= 1) // Il a un voisin critique
                {
                    fprintf(fichier, "%d %s -> ", precedent->coo->x + 4, precedent->nom); // On écrit le précedent à chaque fois
                    precedent = liste_noeuds[tempo->indice];
                    tempo = precedent;
                    chemin = true;
                }
                tempo = tempo->suivant;
            }
            if (chemin) // on écrit le dernier élément du chemin s'il n'est pas composé d'un seul élément
                fprintf(fichier, "%d %s\n", precedent->coo->x + 4, precedent->nom);
        }
        // Problème: si on a le chemin critique n1->n2->n3 alors il apparaîtra en plus du chemin n2->n3 et n1->n2
        // C'est une des limites du programme, difficile de l'en empêcher sachant que n1->n2->n4 peut être un chemin critique aussi etc
    }
    // Ecriture des informations de chaque UP/GP
    fprintf(fichier_relations, "---------------- Nombre de relations de chaque UP ----------------\n");
    fprintf(fichier_relations, "DEGRE  INDICE  X   Y    SMSTRE plus tôt   SMSTRE plus tard                  NOM\n");
    fprintf(fichier_resultats, "\n----------------TOP %d degré (modifiable via NBR_TOPDEGRE) ----------------\n", NBR_TOPDEGRE);
    fprintf(fichier_resultats, "DEGRE  INDICE  X   Y    SMSTRE plus tôt   SMSTRE plus tard                  NOM\n");
    int nbr_Top = NBR_TOPDEGRE;
    for (int j = deg_Max; j >= 0; j--) // degré = degré avant graph de précédence, on écrit dans l'ordre décroissant
    {
        for (int i = 0; i < nbr_noeuds; i++) // On parcourt la liste
            if (liste_noeuds[i]->degre == j) // Si on trouve un UP/GP de degré j, on l'écrit dans le fichier ALLUPGP et dans le TOP X si le top n'est pas rempli
            {
                fprintf(fichier_relations, "  %-2d      %-3d  %d   %-10d   %-15d  %-10d  %s\n", liste_noeuds[i]->degre, liste_noeuds[i]->indice, liste_noeuds[i]->coo->x, liste_noeuds[i]->coo->y, liste_noeuds[i]->semestrePlusTot, liste_noeuds[i]->semestrePlusTard, liste_noeuds[i]->nom);
                // Utilisation du TOP X, c'est un compteur inverse afin d'être sûr qu'il y a X noeuds dedans
                if (nbr_Top > 0)
                {
                    fprintf(fichier_resultats, "  %-2d      %-3d  %d   %-10d   %-15d  %-10d  %s\n", liste_noeuds[i]->degre, liste_noeuds[i]->indice, liste_noeuds[i]->coo->x, liste_noeuds[i]->coo->y, liste_noeuds[i]->semestrePlusTot, liste_noeuds[i]->semestrePlusTard, liste_noeuds[i]->nom);

                    nbr_Top--;
                }
            }
    }
}

void TrouverCluster(nodes **liste_noeuds, int nbr_noeuds, FILE *resultats)
{
    int nbr_isole = 0, nbr_cluster = 0;                       // Calcul des deux nombres pour les écrirent
    int *tabMarque = (int *)malloc(nbr_noeuds * sizeof(int)); // Permet de savoir par quel indice le programme est passé
    for (int i = 0; i < nbr_noeuds; i++)
        tabMarque[i] = 0;
    bordFile *f; // Initialisation file
    for (int s = 0; s < nbr_noeuds; s++)
    {
        if (tabMarque[s] == 0) // Si le noeud n'a pas déjà été visité
        {
            visiter(liste_noeuds, f, tabMarque, s, &nbr_isole, &nbr_cluster); // On le visite
        }
    }
    // écriture finale dans le fichier texte
    fprintf(resultats, "\nNombre de UP/GP isolés : %d\nNombre de clusters : %d\n", nbr_isole, nbr_cluster);
    free(tabMarque);
}
void InterdependanceGP(nodes **liste_noeuds, int nbr_noeuds, FILE *fichier)
{
    fprintf(fichier, "Nombre d'interdépendance de chaque GP (degré GP !=0)\n  IGP                     NOM                      Interdépendance\n");
    for (int i = 0; i < nbr_noeuds; i++) // Parcourt de la liste
    {
        if (liste_noeuds[i]->estGP && liste_noeuds[i]->degre != 0) // Si on trouve un GP avec des voisins
        {
            int interdependance = 0;
            int degre = liste_noeuds[i]->degre;
            nodes *tempo = liste_noeuds[i]->suivant;
            while (tempo != NULL) // On parcourt les voisins
            {
                nodes *tempoVoisin = liste_noeuds[tempo->indice]->suivant;
                while (tempoVoisin != NULL) // Pour chaque voisin de ce GP on regarde s'il n'a pas un voisin qui appartient à ce GP
                {                           // Rappel : tous les voisins d'un GP ontw le même numéroGP que ce GP
                    if (liste_noeuds[tempoVoisin->indice]->numeroGP == liste_noeuds[i]->numeroGP)
                        interdependance++;
                    tempoVoisin = tempoVoisin->suivant;
                }
                tempo = tempo->suivant;
            }
            fprintf(fichier, "  %-5d %-50s %d\n", liste_noeuds[i]->numeroGP, liste_noeuds[i]->nom, interdependance);
        }
    }
}
void DependanceEntreGP(nodes **liste_noeuds, int nbr_noeuds, int nbr_GP, FILE *fichier)
{ // Initialisation de la matrice d'adjacence avec sur chaque axe des GP permettant de savoir le nombre de laisons en commun
    fprintf(fichier, "Nombre de dépendance entre les GP d'indice 1 et d'indice 2 (degré GP !=0)\nINDICE1  INDICE2      Dépendance\n");
    int **matrice = (int **)malloc(nbr_GP * sizeof(int *));
    for (int i = 0; i < nbr_GP; i++)
        matrice[i] = (int *)malloc(nbr_GP * sizeof(int));
    for (int i = 0; i < nbr_GP; i++)
        for (int j = 0; j < nbr_GP; j++)
            matrice[i][j] = 0;
    for (int i = 0; i < nbr_noeuds; i++) // On parcours la liste de noeuds
    {
        if (liste_noeuds[i]->estGP && liste_noeuds[i]->degre != 0) // De même, si le noeud est un GP et qu'il a au moins un voisin
        {
            int degre = liste_noeuds[i]->degre;
            nodes *tempo = liste_noeuds[i]->suivant;
            while (tempo != NULL) // On parcours ses voisins
            {
                nodes *tempoVoisin = liste_noeuds[tempo->indice]->suivant;
                while (tempoVoisin != NULL) // on parcours les voisins de cet UP
                {
                    if (liste_noeuds[i]->numeroGP != liste_noeuds[tempoVoisin->indice]->numeroGP && liste_noeuds[tempoVoisin->indice]->numeroGP != -1) // On ne s'interesse pas à l'interdépendance
                    {
                        if (liste_noeuds[i]->numeroGP > liste_noeuds[tempoVoisin->indice]->numeroGP) // On utilise que la moitié de la matrice
                                                                                                     // Pour pouvoir répertorier et écrire plus facilement
                            matrice[liste_noeuds[i]->numeroGP][liste_noeuds[tempoVoisin->indice]->numeroGP]++;
                        else
                            matrice[liste_noeuds[tempoVoisin->indice]->numeroGP][liste_noeuds[i]->numeroGP]++;
                    }
                    tempoVoisin = tempoVoisin->suivant;
                }
                tempo = tempo->suivant;
            }
        }
    }
    for (int i = 0; i < nbr_GP; i++) // On lit la matrice d'adjacence
    {
        for (int j = 0; j < nbr_GP; j++)
        {
            if (matrice[i][j] != 0) // S'il y a un dépendance entre le GP_i et le GP_j
            {
                fprintf(fichier, "   %-8d %-10d %d\n", i, j, matrice[i][j]);
            }
        }
    }
    free(matrice);
}
int *PlusLongChemin(nodes **liste_noeuds, int debut, int fin)
{
    // On borne la recherche sur un semestre, une année ... d'où fin et début qui sont les indices max et min
    int nbr_noeuds = fin - debut;
    int *values = (int *)malloc(3 * sizeof(int));
    values[0] = 0; // Correspond à la distance max
    values[1] = 0; // Correspond au noeud de départ pour trouver le chemin le plus long
    values[2] = 0; // Correspond au noeud d'arrivé pour trouver le chemin le plus long
    // Initialisation à 0 des tableaux utiles
    int *distance = (int *)malloc(nbr_noeuds * sizeof(int)); // Correspond à la distance de chaque noeud
    // Contient pour chaque noeud de départ i, l'indice du noeud d'arrivé pour le chemin le plus long
    int *indiceArrive = (int *)malloc(nbr_noeuds * sizeof(int));
    bool *visiter = (bool *)malloc(nbr_noeuds * sizeof(bool)); // Liste des noeuds visités, 1 si oui, 0 si non
    for (int i = 0; i < nbr_noeuds; i++)
    {
        distance[i] = 0;
        indiceArrive[i] = 0;
        visiter[i] = false;
    }
    // On parcourt tous les indices correspondant aux critères de recherche
    for (int i = debut; i < fin; i++)
    {
        if (!visiter[i - debut]) // S'ils n'ont pas déjà été visité
        {                        // On actualise la distance
            ActualiserDistance(liste_noeuds, distance, indiceArrive, visiter, i, debut, fin);
        }
    }
    values[0] = 0; // Distance max
    for (int i = 0; i < nbr_noeuds; i++)
    {
        if (distance[i] > values[0]) // Si on trouve une distance supérieur à la distance max
        {
            values[0] = distance[i];     // Alors c'est la nouvelle distance max
            values[1] = i + debut;       // On sauvegarde l'indice du noeud de départ
            values[2] = indiceArrive[i]; // On sauvegarde l'indice du noeud d'arrivé
        }
    }
    free(distance);
    free(indiceArrive);
    free(visiter);
    return values;
}
void EcrirePlusLongChemin(nodes **liste_noeuds, int *indicdeDelimitation, FILE *resultats)
{
    // Ecriture dans le fichier des plus long chemin avec l'appel aux différentes fonctions
    int *values;
    fprintf(resultats, "---------------- Distance pLus long chemin ----------------");
    // Ecriture semestre
    for (int i = 0; i < 6; i++)
    {
        values = PlusLongChemin(liste_noeuds, indicdeDelimitation[i], indicdeDelimitation[i + 1]);
        if (values[0] != 0) // Si la distance max n'est pas nulle alors on peut écrire le nom du noeud de départ et celui d'arrivé
            fprintf(resultats, "\nSemestre %02d : %02d. Le chemin va de : %-35s à %s", i + 5, values[0], liste_noeuds[values[1]]->nom, liste_noeuds[values[2]]->nom);
        else // Sinon, ses noeuds de départ et d'arrivé n'existe pas car tous les noeuds sont isolés
            fprintf(resultats, "\nSemestre %02d : %02d.\n", i + 5, values[0]);
    }
    // Ecriture année
    for (int i = 0; i < 3; i++)
    { // On pourrait refaire le même if que pour les semestres mais la condition else n'est jamais utilisé dans notre base de donnée
        values = PlusLongChemin(liste_noeuds, indicdeDelimitation[2 * i], indicdeDelimitation[2 * (i + 1)]);
        fprintf(resultats, "\nAnnée %02d : %02d. Le chemin va de : %-38s à %s", i, values[0], liste_noeuds[values[1]]->nom, liste_noeuds[values[2]]->nom);
    }
    // Ecriture programme entier
    // De même, on pourrait réecrire la condition if mais inutile avec nos données.
    values = PlusLongChemin(liste_noeuds, indicdeDelimitation[0], indicdeDelimitation[NBR_SEMESTRE]);
    fprintf(resultats, "\n\nTout le programme : %d. Le chemin va de : %-35s à %s", values[0], liste_noeuds[values[1]]->nom, liste_noeuds[values[2]]->nom);
}
int *DelimiterListe(nodes **liste_noeuds, int nbr_noeuds)
{ // Permet de délimiter les semestres dans les noeuds donnés
    // 1er indice : 0, dernier indice : nbr_noeuds
    int *indiceDelimitation = (int *)malloc((NBR_SEMESTRE + 1) * sizeof(int));
    indiceDelimitation[0] = 0;
    indiceDelimitation[NBR_SEMESTRE] = nbr_noeuds;
    int j = 1;
    for (int i = 1; i < nbr_noeuds; i++)
    {
        if (liste_noeuds[i]->coo->x > liste_noeuds[i - 1]->coo->x)
        { // Quand le x change on a changé de semestre donc on sauvegarde l'indice
            indiceDelimitation[j] = i;
            j++;
        }
    }
    return indiceDelimitation;
}
void creerGraphPrecedence(nodes **liste_noeuds, int nbr_noeuds)
{                                        // A partir du graphique contenant toutes les arêtes, on crée le graph de précédence
    for (int i = 0; i < nbr_noeuds; i++) // Parcourt de chaque noeud
    {
        nodes *precedent = liste_noeuds[i];
        nodes *voisin = precedent->suivant;
        while (voisin != NULL) // On regarde la différence de coo_x entre le noeud et chacun de ses voisins
        {
            if (voisin->coo->x <= liste_noeuds[i]->coo->x) // Si le voisin a un plus petit semestre ou le même semestre que le noeud
            {                                              // On enlève ce voisin
                precedent->suivant = voisin->suivant;
                nodes *tempo = voisin;
                voisin = precedent->suivant;
                free(tempo);
            }
            else // Sinon on garde le voisin
            {    // On actualise les valeurs de semestrePlusTard et semestrePlusTot
                if (liste_noeuds[i]->semestrePlusTard > voisin->coo->x + 4)
                    liste_noeuds[i]->semestrePlusTard = voisin->coo->x + 4;
                if (liste_noeuds[voisin->indice]->semestrePlusTot < liste_noeuds[i]->coo->x + 5)
                    liste_noeuds[voisin->indice]->semestrePlusTot = liste_noeuds[i]->coo->x + 5;
                // On actualise les variables pour la suite
                precedent = voisin;
                voisin = voisin->suivant;
            }
        }
    }
}
int main()
{
    // Initialisation des fichiers, des variables globales
    FILE *fichier_noeuds, *fichier_aretes, *resultats, *relationUP, *requiredUP, *dependanceGP, *othersResults;
    int nbr_GP = 0, nbr_noeuds, nbr_aretes = 0;
    resultats = fopen(FIC_OUTPUT, "w+");
    relationUP = fopen(FIC_OUTPUT_RELATION, "w+");
    requiredUP = fopen(FIC_OUTPUT_REQUIRED, "w+");
    dependanceGP = fopen(FIC_OUTPUT_DEPENDANCE, "w+");
    othersResults = fopen(FIC_OUTPUT_OTHERS, "w+");
    fichier_noeuds = fopen(FIC_NOEUD, "r");
    fichier_aretes = fopen(FIC_EDGE, "r");
    if (resultats != NULL && relationUP != NULL && requiredUP != NULL && dependanceGP != NULL && othersResults != NULL && fichier_noeuds != NULL && fichier_aretes != NULL)
    {
        nodes **liste_noeuds = LireNoeuds(fichier_noeuds, &nbr_noeuds, &nbr_GP);         // Mise en place de la liste des noeuds sans voisin
        int deg_Max = LireAretes(fichier_aretes, liste_noeuds, nbr_noeuds, &nbr_aretes); // Ajout des voisins à la liste des noeuds
        // Ecriture des premières valeurs dans "mainResults"
        fprintf(resultats, "Nombre de noeuds = %d\nNombre d'UP = %d\nNombre de GP = %d\nNombre d'arêtes = %d\nDegré maximum = %d", nbr_noeuds, nbr_noeuds - nbr_GP, nbr_GP, nbr_aretes, deg_Max);
        TrouverCluster(liste_noeuds, nbr_noeuds, resultats);                                         // Ecriture des informations concernant les clusters
        InterdependanceGP(liste_noeuds, nbr_noeuds, dependanceGP);                                   // Ecriture des informations concernant l'interdependance de chaque GP
        DependanceEntreGP(liste_noeuds, nbr_noeuds, nbr_GP, dependanceGP);                           // Puis de la dépendance de chaque GP_i avec un GP_j
        EcrirePlusLongChemin(liste_noeuds, DelimiterListe(liste_noeuds, nbr_noeuds), othersResults); // Pour les autres indicateurs de performances
        creerGraphPrecedence(liste_noeuds, nbr_noeuds);                                              // Passage du graph relationnel au graph de précédence
        PlusLongCheminPrecedence(liste_noeuds, nbr_noeuds, resultats);                               // Cette fois ci calcule d'un seul plus long chemin
        TrouverPrerequis(liste_noeuds, nbr_noeuds, deg_Max, resultats, relationUP, requiredUP);      // Ecriture des informations concernant les prérequis
        // Fermeture de tous les fichiers
        fclose(resultats);
        fclose(relationUP);
        fclose(requiredUP);
        fclose(dependanceGP);
        fclose(othersResults);
        fclose(fichier_noeuds);
        fclose(fichier_aretes);
    }
    else
        printf("Erreur dans les fichiers, avez-vous bien un dossier 'resultats' vide et les fichiers nodes.csv et edges.csv?");
    return 0;
}
