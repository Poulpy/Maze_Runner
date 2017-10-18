/******************************************************************************
* charge_labyrinthe
*
*****************************************************************************/
 
#include <stdio.h>
#include <stdlib.h>

#define LIG 63
#define COL 45

// PROTOTYPES

// Charge dans le tableau tab le labyrinthe du fichier f.
// Ce fichier doit contenir les dimensions du labyrinthe sur la 1ère ligne:
// NB_LIGNES puis NB COLONNES
void charge_labyrinthe(char *f, char tab[LIG][COL]);
