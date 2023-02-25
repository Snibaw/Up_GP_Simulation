Pour utiliser le projet :
    Avec makefile:
    make
    ./_Projet
	
    sans makefile:
    Compiler et lancer projet.c en utilisant -lm
    gcc ProjetSansMakefile.c -o ProjetSansMakefile -lm
    ./ProjetSansMakefile
	

Lire les résultats :
    5 fichiers d'output dans le dossiers resultats:
    mainResults.txt -> regroupe les infos les plus générales : nombre d'UP, de noeuds, d'arêtes, top X des degré sur le graph relationnel
    mais aussi des infos sur le graph de précédence comme le plus long chemin
    AllUPGP.txt -> contient les informations sur chaque noeud, le degré dépend du graph relationnel, pas du graph de précédence.
    Dependance.txt -> écriture des informations concernant les GP avec leur interdépendance mais aussi la dépendance entre un GP_i et un GP_j
    Prerequis.txt -> regroupe les chemins dont les prérequis sont les plus éloignes, les UP/GP et chemin critiques
    Others.txt -> contient des informations sur les plus longs chemins dans chaque semestre, année, totale du graph relationnel

Modifier le nombre d'éléments dans les TOP :
    Dans les defines, il y a deux valeurs modifiables par l'utilisateur:
    NBR_TOPDEGRE qui régit le nombre d'éléments dans le top des degré dans "mainResults.txt"
    MINREQUIREDDISTANCE qui permet de modifier la distance minimale permettant de considérer un prérequis comme éloigné dans "Prerequis.txt"