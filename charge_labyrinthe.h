/******************************************************************************
* charge_labyrinthe
*
*****************************************************************************/
 
#include <stdio.h>
#include <stdlib.h>

#define LIG 63
#define COL 45


typedef struct SizeTab {
	int nbr_Lignes;
	int nbr_Colonnes;
} SizeTab;

// PROTOTYPES

// Charge dans le tableau tab le labyrinthe du fichier f.
// Ce fichier doit contenir les dimensions du labyrinthe sur la 1ère ligne:
// NB_LIGNES puis NB COLONNES
SizeTab charge_labyrinthe(char *f, char tab[LIG][COL]);
int Get_Lab_Ligne();
int Get_Lab_Colonne();
