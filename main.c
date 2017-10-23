#include "../../lib/libgraphique.h"
#include "./charge_labyrinthe.h"
#include <stdio.h>

#define COTE 10
#define FEN_X 1200
#define FEN_Y 850

/* Définitions des structures */

typedef struct Tableau { /* Structure définissant un tableau avec ses lignes et ses colonnes */
	int Ligne;
	int Colonne;
} Tableau;

typedef struct Joueur { /* Structure définissant un joueur */
	
	int Points; /* Les points que possède le joueur */
	Tableau Pos_Tab; /* Sa position dans le tableau du labyrinthe */
	Point Pos; /* Sa position graphiquement dans la fenêtre */
	Point Sortie; /* La sortie que le joueur doit rejoindre */
	
} Joueur;

/* Définitions des prototypes de fonctions */

Point Refresh_Maze(char tab[LIG][COL], Point Pos_J1, Point Pos_J2, int Espacement);

Tableau Get_Tab_Pos_By_Pos(char tab[LIG][COL], Point Pos, int Espacement);

int Main_Menu(char tab[LIG][COL]);
int Check_Win(Joueur J1, Joueur J2);
void Win(int isJ1);

void Check_And_Change_Letter(char tab[LIG][COL], int isJ1, int Phase2, Joueur Joueur);


/* ======== Main ======== */

int main(int argc, char *argv[])
{

	/* Initialisation des variables */
	
    char tab[LIG][COL];
    
	int col, lig; 
	int maze_col = 63, maze_lig = 45, Espacement = 13, Tour = 1;
	
	//Variables de la boucle de mouvement
	
	int Touche_Press, IsTouchPress;
	Point Pos_Temp = {0, 0};
	char Char_Temp = ' ';
	Tableau Tableau_Temp = {0, 0};
	
	//Variables des deux joueurs
	
	Joueur J1 = {0, {0, 0}, {0, 0}, {0, 0}}, J2 = {0, {0, 0} ,{0, 0}, {0, 0}};
   
	/* Menu principal */
	
	if (!Main_Menu(tab)) //On lance le menu principal
		return 0; //Si il renvoie 0 cela veut dire que l'utilisateur veut quitter
	 
	//Sinon on ouvre la fenêtre graphique 
	
    ouvrir_fenetre(FEN_X, FEN_Y);

	/* Initialisation de la position des joueurs */
    
	do
    {
     	
    	col = entier_aleatoire(maze_col); //emplacement verticale
    	lig = entier_aleatoire(maze_lig); //emplacement horizontale
    	
    	while (tab[lig][col] == '*')
    	{
    		col = entier_aleatoire(maze_col);
    		lig = entier_aleatoire(maze_lig);
    	}
    	
    	tab[lig][col]= 'T';
    
    	J1.Pos.x = col * Espacement;
    	J1.Pos.y = lig * Espacement;
    	
    	J2.Pos.x = J1.Pos.x + 600;
    	J2.Pos.y = J1.Pos.y;
     	
    } while ((J1.Pos.x > (maze_lig * Espacement) + 2) || (J1.Pos.y > (maze_col * Espacement) + 2));
    
    J1.Pos_Tab.Colonne = col;
    J1.Pos_Tab.Ligne = lig;
     
    J2.Pos_Tab.Colonne = col;
    J2.Pos_Tab.Ligne = lig;

	J2.Sortie = Refresh_Maze(tab, J1.Pos, J2.Pos, Espacement);
	
	J1.Sortie.x = J2.Sortie.x - 600;
	J1.Sortie.y = J2.Sortie.y;
	
	actualiser();
	
	
	/* Boucle principale de jeu tant qu'un joueur n'a pas atteint une sortie */

    while (!Check_Win(J1, J2))
    {
		
		Touche_Press = attendre_touche(); /* On attend une pression sur une touche */
		
		Pos_Temp.x = 0;
		Pos_Temp.y = 0;
		
		Char_Temp = ' ';
		
		Tableau_Temp.Ligne = 0;
		Tableau_Temp.Colonne = 0;
		
		switch (Touche_Press)
    	{
    		case SDLK_UP:
    			
    			Pos_Temp.x = J1.Pos.x;
    			Pos_Temp.y = J1.Pos.y - Espacement;
    			
    			Tableau_Temp = Get_Tab_Pos_By_Pos(tab, Pos_Temp, Espacement);
    			
    			Char_Temp = tab[Tableau_Temp.Ligne][Tableau_Temp.Colonne];
    			
    			if (Char_Temp != '*')
    			{
 					J1.Pos.y -= Espacement;
 					
					Check_And_Change_Letter(tab, 1, 0, J1);
 						 
 					J1.Pos_Tab = Tableau_Temp;
 					
					Check_And_Change_Letter(tab, 1, 1, J1);
 					
    			}
    				
    			break;
    			
    		case SDLK_DOWN:
    		
    			Pos_Temp.x = J1.Pos.x;
    			Pos_Temp.y = J1.Pos.y + Espacement;
    			
    			Tableau_Temp = Get_Tab_Pos_By_Pos(tab, Pos_Temp, Espacement);
    			
    			Char_Temp = tab[Tableau_Temp.Ligne][Tableau_Temp.Colonne];
    			
    			if (Char_Temp != '*')
    			{
    				J1.Pos.y += Espacement;
    				
    				Check_And_Change_Letter(tab, 1, 0, J1);
 						 
    				J1.Pos_Tab = Tableau_Temp;
    				
    				Check_And_Change_Letter(tab, 1, 1, J1);
    			}
    			
    			break;
    			
    		case SDLK_LEFT:
    		
    			Pos_Temp.x = J1.Pos.x - Espacement;
    			Pos_Temp.y = J1.Pos.y;
    			
    			Tableau_Temp = Get_Tab_Pos_By_Pos(tab, Pos_Temp, Espacement);
    			
    			Char_Temp = tab[Tableau_Temp.Ligne][Tableau_Temp.Colonne];
    			
    			if (Char_Temp != '*')
    			{
					J1.Pos.x -= Espacement;
					
					Check_And_Change_Letter(tab, 1, 0, J1);
 						 
					J1.Pos_Tab = Tableau_Temp;
					
					Check_And_Change_Letter(tab, 1, 1, J1);
    			}
    			
    			break;
    			
    		case SDLK_RIGHT:
    		
    			Pos_Temp.x = J1.Pos.x + Espacement;
    			Pos_Temp.y = J1.Pos.y;
    			
    			Tableau_Temp = Get_Tab_Pos_By_Pos(tab, Pos_Temp, Espacement);
    			
    			Char_Temp = tab[Tableau_Temp.Ligne][Tableau_Temp.Colonne];
    			
    			if (Char_Temp != '*')
    			{
    				J1.Pos.x += Espacement;
    				
    				Check_And_Change_Letter(tab, 1, 0, J1);
 						 
    				J1.Pos_Tab = Tableau_Temp;
    				
    				Check_And_Change_Letter(tab, 1, 1, J1);
    			}
    			
    			break;
    			
    		case SDLK_z:
    			
    			Pos_Temp.x = J2.Pos.x;
    			Pos_Temp.y = J2.Pos.y - Espacement;
    			
    			Tableau_Temp = Get_Tab_Pos_By_Pos(tab, Pos_Temp, Espacement);
    			
    			Char_Temp = tab[Tableau_Temp.Ligne][Tableau_Temp.Colonne];
    			
    			if (Char_Temp != '*')
    			{
 					J2.Pos.y -= Espacement;
 					
 					Check_And_Change_Letter(tab, 0, 0, J2);
 						 
 					J2.Pos_Tab = Tableau_Temp;
 					
 					Check_And_Change_Letter(tab, 0, 1, J2);
 					
    			}
    			
    			break;
    			
    		case SDLK_s:

    			Pos_Temp.x = J2.Pos.x;
    			Pos_Temp.y = J2.Pos.y + Espacement;
    			
    			Tableau_Temp = Get_Tab_Pos_By_Pos(tab, Pos_Temp, Espacement);
    			
    			Char_Temp = tab[Tableau_Temp.Ligne][Tableau_Temp.Colonne];
    			
    			if (Char_Temp != '*')
    			{
 					J2.Pos.y += Espacement;
 					
 					Check_And_Change_Letter(tab, 0, 0, J2);
 						 
 					J2.Pos_Tab = Tableau_Temp;
 					
 					Check_And_Change_Letter(tab, 0, 1, J2);
 					
    			}
    		
    			break;
    			
    		case SDLK_q:

    			Pos_Temp.x = J2.Pos.x - Espacement;
    			Pos_Temp.y = J2.Pos.y;
    			
    			Tableau_Temp = Get_Tab_Pos_By_Pos(tab, Pos_Temp, Espacement);
    			
    			Char_Temp = tab[Tableau_Temp.Ligne][Tableau_Temp.Colonne];
    			
    			if (Char_Temp != '*')
    			{
 					J2.Pos.x -= Espacement;
 					
 					Check_And_Change_Letter(tab, 0, 0, J2);

 					J2.Pos_Tab = Tableau_Temp;
 					
 					Check_And_Change_Letter(tab, 0, 1, J2);
 					
    			}
    		
    			break;
    			
    		case SDLK_d:

    			Pos_Temp.x = J2.Pos.x + Espacement;
    			Pos_Temp.y = J2.Pos.y;
    			
    			Tableau_Temp = Get_Tab_Pos_By_Pos(tab, Pos_Temp, Espacement);
    			
    			Char_Temp = tab[Tableau_Temp.Ligne][Tableau_Temp.Colonne];
    			
    			if (Char_Temp != '*')
    			{
 					J2.Pos.x += Espacement;
 					
 					Check_And_Change_Letter(tab, 0, 0, J2);
 						 
 					J2.Pos_Tab = Tableau_Temp;
 					
 					Check_And_Change_Letter(tab, 0, 1, J2);
 					
    			}
    		
    			break;
    			
    		case SDLK_ESCAPE:
    		
    			return 0;
    			
    			break;
    	}
    	
    	
		Refresh_Maze(tab, J1.Pos, J2.Pos, Espacement);
		
    	actualiser();
		
    }
    
    if (Check_Win(J1, J2) == 1)
    	Win(1);
    else
    	Win(0);
    
    // fin de la session graphique
    fermer_fenetre();
    return 0;
}


/*             Fonctions principales                             */


int Main_Menu(char tab[LIG][COL])
{

	int Choix_User = 0;
	
	while (Choix_User != 1 && Choix_User != 2 && Choix_User != 3)
	{
		system("clear");
		printf("=== Ultimate Maze Runner ===\n");
		printf("1 - Jouer Labyrinthe 1\n");
		printf("2 - Jouer Labyrinthe 2\n");
		printf("3 - Quitter :(\n");
		
		scanf("%d", &Choix_User);
	}

	switch (Choix_User)
	{
		
		case 1:
			charge_labyrinthe("./maze", tab);
	   	    return 1;
			break;	
		case 2:
			charge_labyrinthe("./maze2", tab);
			return 1;
			break;
		case 3:
			return 0;
			break;
	}
	
	return 0;
		
}

int Check_Win(Joueur J1, Joueur J2)
{
	if (((J1.Pos.x != J1.Sortie.x) || (J1.Pos.y != J1.Sortie.y)) && ((J2.Pos.x != J2.Sortie.x) || (J2.Pos.y != J2.Sortie.y)))
		return 0;
	else	
	{
		if (((J1.Pos.x == J1.Sortie.x) && (J1.Pos.y == J1.Sortie.y)))
			return 1;
		else
			return 2;
	}
}


void Win(int isJ1)
{

	Point Pos_Texte = {100, 100};
	
	if (isJ1)
		//afficher_texte("Bonjour", 8, Pos_Texte, blanc);
	else
		afficher_texte("J", 2, Pos_Texte, blanc);

	actualiser();
	
	attendre_clic();
	
}

void Check_And_Change_Letter(char tab[LIG][COL], int isJ1, int Phase2, Joueur Joueur)
{
	if (isJ1 && !Phase2)
	{
		 if (tab[Joueur.Pos_Tab.Ligne][Joueur.Pos_Tab.Colonne] == 'T')
 			tab[Joueur.Pos_Tab.Ligne][Joueur.Pos_Tab.Colonne] = 'K';
 		 else
 			tab[Joueur.Pos_Tab.Ligne][Joueur.Pos_Tab.Colonne] = ' ';	
	}
	else if (isJ1 && Phase2)
	{
		if (tab[Joueur.Pos_Tab.Ligne][Joueur.Pos_Tab.Colonne] == 'K')
 			tab[Joueur.Pos_Tab.Ligne][Joueur.Pos_Tab.Colonne] = 'T';
 		else
 			tab[Joueur.Pos_Tab.Ligne][Joueur.Pos_Tab.Colonne] = 'J';
	}
	else if (!isJ1 && !Phase2)
	{
		if (tab[Joueur.Pos_Tab.Ligne][Joueur.Pos_Tab.Colonne] == 'T')
 			tab[Joueur.Pos_Tab.Ligne][Joueur.Pos_Tab.Colonne] = 'J';
 		else
 			tab[Joueur.Pos_Tab.Ligne][Joueur.Pos_Tab.Colonne] = ' ';
	}
	else
	{
		if (tab[Joueur.Pos_Tab.Ligne][Joueur.Pos_Tab.Colonne] == 'J')
 			tab[Joueur.Pos_Tab.Ligne][Joueur.Pos_Tab.Colonne] = 'T';
 		else
 			tab[Joueur.Pos_Tab.Ligne][Joueur.Pos_Tab.Colonne] = 'K';
	}
}

Point Refresh_Maze(char tab[LIG][COL], Point Pos_J1, Point Pos_J2, int Espacement)
{

	int i, l, c;
	Point Coin = {0, 0}, Sortie = {0, 0};
	
	for(i=0; i < 2; i++)
	{
		for(l=0; l!=LIG; l++)
    	{ 
			for(c=0; c!=COL; c++)
		   	{
		  
		   		if (tab[l][c] == '*')
		        	dessiner_rectangle(Coin, Espacement, Espacement, marron);
		        	
		   		else if (tab[l][c] == 'J' || tab[l][c] == 'T' || tab[l][c] == 'K')
		   		{
		   			if (i == 0)
		   			{
		   				if (tab[l][c] == 'J' || tab[l][c] == 'T')
		   					dessiner_rectangle(Pos_J1, Espacement, Espacement, yellow);
		   				else
		   					dessiner_rectangle(Coin, Espacement, Espacement, noir);

		   			}
		   			else if (i == 1)
		   			{
		   				if (tab[l][c] == 'K' || tab[l][c] == 'T')
		   					dessiner_rectangle(Pos_J2, Espacement, Espacement, yellow);
		   				else
		   					dessiner_rectangle(Coin, Espacement, Espacement, noir);
		   			}
		   		}	
		   		
		   		else if (tab[l][c] == 'S')
		   		{
		   			dessiner_rectangle(Coin, Espacement, Espacement, green);
		   			Sortie.x = Coin.x;
		   			Sortie.y = Coin.y;
		   		}
		   		else if (tab[l][c] == ' ')
		   			dessiner_rectangle(Coin, Espacement, Espacement, noir);
		   			
		   		Coin.x += Espacement;
		   			
		   		//printf("%c",tab[l][c]);
		   	}
            
            if (i == 0)
		 		Coin.x = 0;
		 	else
		 		Coin.x = 600;
		 		
		 	Coin.y += Espacement;
			//printf("\n");
    	}
    	
    	Coin.y = 0;
    	Coin.x = 600;
	}
	
	return Sortie;
	
}

Tableau Get_Tab_Pos_By_Pos(char tab[LIG][COL], Point Pos, int Espacement)
{
	int i, l, c;
	Point Coin = {0, 0};
	Tableau Pos_J = {0, 0};
	
	for(i=0; i < 2; i++)
	{
		for(l=0; l!=LIG; l++)
    	{ 
			for(c=0; c!=COL; c++)
		   	{
		   		if (Coin.x == Pos.x && Coin.y == Pos.y)
		   		{
		   			Pos_J.Ligne = l;
		   			Pos_J.Colonne = c;
		   			return Pos_J;
		   		}
		   		
		   		Coin.x += Espacement;
		   	}
		   	
		   	if (i == 0)
		 		Coin.x = 0;
		 	else
		 		Coin.x = 600;
		 		
		 	Coin.y += Espacement;
		}
		
		Coin.y = 0;
    	Coin.x = 600;
    	
	}
	
	return Pos_J;
}
