#include "../../lib/libgraphique.h"
#include "./charge_labyrinthe.h"
#include <stdio.h>

int main(int argc, char *argv[])
{

	ouvrir_fenetre(800, 600);
	    
	Point Pos_Texte = {0, 0};
	afficher_texte("T", 10, Pos_Texte, blanc);
	
	actualiser();
	
 	// fin de la session graphique
 	attendre_clic();
    fermer_fenetre();
    return 0;
}
