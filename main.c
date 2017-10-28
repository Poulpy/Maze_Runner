#include "../../lib/libgraphique.h"
#include "./charge_labyrinthe.h"
#include <stdio.h>

#define COTE 10
#define FEN_X 1190
#define FEN_Y 950

#define NBR_BTN 10

/* ======== Définitions des structures ======== */

typedef enum Direction { /* Enumérations des différentes direction de déplacement possible */
	HAUT, BAS, GAUCHE, DROITE
} Direction;

typedef enum Type_Bouton {
	MENU_PRINCIPAL, JEU
} Type_Bouton;

typedef struct Tableau { /* Structure définissant un tableau avec ses lignes et ses colonnes */
	int Ligne;
	int Colonne;
} Tableau;

typedef struct Joueur { /* Structure définissant un joueur */
	
	int Points; /* Les points que possède le joueur */
	Tableau Pos_Tab; /* Sa position dans le tableau du labyrinthe */
	Point Pos; /* Sa position graphiquement dans la fenêtre */
	Point Sortie; /* La sortie que le joueur doit rejoindre */
	Couleur Couleur;
	
} Joueur;

typedef struct Bouton {
	
	Point Pos_HautG; /* Position haut gauche d'un bouton */
	Point Pos_BasD; /* Position bas droite d'un bouton */
	char* Texte; /* Le texte du bouton */
	Type_Bouton Type_Bouton; /* Le menu dans lequel sera affiché le bouton */
	char* Image; /* Le chemin d'accès à l'image du bouton */
	
} Bouton;



/* ======== Définitions des prototypes de fonctions ======== */

//Fonctions d'affichage
Point Refresh_Maze(char tab[LIG][COL], Point Pos_J1, Point Pos_J2, int Espacement);
void Clear_Screen();

//Fonctions des menus
int Main_Menu(char tab[LIG][COL], Bouton Liste_Bouton[NBR_BTN]);

//Fonctions de déplacement
void Deplacement(char tab[LIG][COL], Point Pos_Temp, Joueur *Joueur, int Espacement, Direction Direction, int isJ1);
void Check_And_Change_Letter(char tab[LIG][COL], int isJ1, int Phase2, Joueur Joueur);
Tableau Get_Tab_Pos_By_Pos(char tab[LIG][COL], Point Pos, int Espacement);

//Fonctions lors de la victoire
int Check_Win(Joueur J1, Joueur J2);
void Win(int isJ1, Bouton Liste_Bouton[NBR_BTN]);

//Fonctions gérants les boutons
int isButtonHit(Type_Bouton TypeBouton, Bouton Liste_Bouton[NBR_BTN], Point Pos_Clic);
void Afficher_Bouton(Type_Bouton TypeBouton, Bouton Liste_Bouton[NBR_BTN]);
void Add_Buttons(Bouton Liste_Bouton[NBR_BTN]);

//Autres fonctions utiles
int min(int a, int b);
int mon_abs(int a);


/* ======== Main ======== */

int main(int argc, char *argv[])
{

	/* ======== Initialisation des variables ======== */
	
    char tab[LIG][COL];
    
    Bouton Liste_Boutons[NBR_BTN];
    
	int col, lig; 
	int maze_col = 63, maze_lig = 45, Espacement = 13, Tour = 1;
	
	//Variables de la boucle de mouvement
	
	int Touche_Press;
	
	Point Pos_Temp = {0, 0}, Coordonnes_Texte_Tour = {430, 845}, Taille_Texte = taille_texte("C'est au joueur 1 de jouer !", 30);
	
	//Variables des deux joueurs
	
	Joueur J1 = {0, {0, 0}, {0, 0}, {0, 0}, yellow}, J2 = {0, {0, 0} ,{0, 0}, {0, 0}, yellow};

	ouvrir_fenetre(FEN_X, FEN_Y);
	
	Add_Buttons(Liste_Boutons);
	
	/* ======== Menu principal ======== */
	
	if (!Main_Menu(tab, Liste_Boutons)) //On lance le menu principal
		return 0; //Si il renvoie 0 cela veut dire que l'utilisateur veut quitter

	Clear_Screen();
	actualiser();
	
	/* ======== Initialisation de la position des joueurs ======== */
    
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
    
    /* On configure la position des 2 joueurs ainsi que celle de leur sortie */
    
    J1.Pos_Tab.Colonne = col;
    J1.Pos_Tab.Ligne = lig;
     
    J2.Pos_Tab.Colonne = col;
    J2.Pos_Tab.Ligne = lig;

	J2.Sortie = Refresh_Maze(tab, J1.Pos, J2.Pos, Espacement);
	
	J1.Sortie.x = J2.Sortie.x - 600;
	J1.Sortie.y = J2.Sortie.y;
	
	actualiser();
	
	
	/* ======== Boucle principale de jeu tant qu'un joueur n'a pas atteint une sortie ======== */

    while (!Check_Win(J1, J2))
    {
		
		//Point clic;
		//clic = attendre_clic();
		//printf("X: %d / Y: %d\n", clic.x, clic.y);
		
		if (Tour == 1)
		{
			dessiner_rectangle(Coordonnes_Texte_Tour, Taille_Texte.x, Taille_Texte.y, black);
			afficher_texte("C'est au joueur 1 de jouer !", 30, Coordonnes_Texte_Tour, blanc);
		}
		else
		{
			dessiner_rectangle(Coordonnes_Texte_Tour, Taille_Texte.x, Taille_Texte.y, black);
			afficher_texte("C'est au joueur 2 de jouer !", 30, Coordonnes_Texte_Tour, blanc);
		}
			
		actualiser();	
			
		Touche_Press = attendre_touche_duree(1000); /* On attend une pression sur une touche */
		
		Pos_Temp.x = 0;
		Pos_Temp.y = 0;

			
		switch (Touche_Press)
    	{
    		case SDLK_UP:
    			
    			if (Tour == 1)
    			{
					Pos_Temp.x = J1.Pos.x;
					Pos_Temp.y = J1.Pos.y - Espacement;
					
					Deplacement(tab, Pos_Temp, &J1, Espacement, HAUT, 1);
					
    			}		
    			
    			break;
    			
    		case SDLK_DOWN:
    		
    			if (Tour == 1)
    			{
					
					Pos_Temp.x = J1.Pos.x;
					Pos_Temp.y = J1.Pos.y + Espacement;
					
					Deplacement(tab, Pos_Temp, &J1, Espacement, BAS, 1);			

    			}
    			
    			break;    
    						
    		case SDLK_LEFT:
    		
    			if (Tour == 1)
    			{
					Pos_Temp.x = J1.Pos.x - Espacement;
					Pos_Temp.y = J1.Pos.y;
					
					Deplacement(tab, Pos_Temp, &J1, Espacement, GAUCHE, 1);
    			
    			}

    			break;
    			
    		case SDLK_RIGHT:
    			
    			if (Tour == 1)
    			{
		 			Pos_Temp.x = J1.Pos.x + Espacement;
					Pos_Temp.y = J1.Pos.y;
					
					Deplacement(tab, Pos_Temp, &J1, Espacement, DROITE, 1); 			
    			}
    			
    			break;
    			
    		case SDLK_z:
    			
    			if (Tour == 2)
    			{
					Pos_Temp.x = J2.Pos.x;
					Pos_Temp.y = J2.Pos.y - Espacement;
					
					Deplacement(tab, Pos_Temp, &J2, Espacement, HAUT, 0);			
    			}
		
    			break;
    			
    		case SDLK_s:
				
				if (Tour == 2)
				{
					Pos_Temp.x = J2.Pos.x;
					Pos_Temp.y = J2.Pos.y + Espacement;
					
					Deplacement(tab, Pos_Temp, &J2, Espacement, BAS, 0);			
				}
    		
    			break;
    			
    		case SDLK_q:

				if (Tour == 2)
				{
					Pos_Temp.x = J2.Pos.x - Espacement;
					Pos_Temp.y = J2.Pos.y;
					
					Deplacement(tab, Pos_Temp, &J2, Espacement, GAUCHE, 0);			
    		
				}

    			break;
    			
    		case SDLK_d:
				
				if (Tour == 2)
				{
					Pos_Temp.x = J2.Pos.x + Espacement;
					Pos_Temp.y = J2.Pos.y;
					
					Deplacement(tab, Pos_Temp, &J2, Espacement, DROITE, 0);			
				}
	
    			break;
    			
    		case SDLK_ESCAPE:
    		
    			return 0;
    			
    			break;
    			
    		case SDLK_F1:
    			
    			J1.Pos.x = J1.Sortie.x;
    			J1.Pos.y = J1.Sortie.y;
    			
    			break;
    			
    		case SDLK_F2:
    		
    			J2.Pos.x = J2.Sortie.x;
    			J2.Pos.y = J2.Sortie.y;
    			
    			break;
    			
    	}
    	
    	
		Refresh_Maze(tab, J1.Pos, J2.Pos, Espacement);
		
    	actualiser();
    	
    	if (Tour == 1)
    		Tour = 2;
    	else
    		Tour = 1;
		
    }
    
    if (Check_Win(J1, J2) == 1) /* On regarde si un joueur à atteint la sortie */
    	Win(1, Liste_Boutons);
    else
    	Win(0, Liste_Boutons);
    
    // Fin de la session graphique
    
    fermer_fenetre();
    return 0;
}


/* ======== Fonctions ======== */


int Main_Menu(char tab[LIG][COL], Bouton List_Bouton[NBR_BTN]) //Affiche le menu principal
{

	system("clear");
	printf("======== Ultimate Maze Runner ========\n");

	int Result_Button_Hit = -1;
	Point Pos_Clic = {-1, -1};
	
	Afficher_Bouton(MENU_PRINCIPAL, List_Bouton);
	actualiser();
	
	while (Result_Button_Hit == -1)
	{
		Pos_Clic = attendre_clic();
		Result_Button_Hit = isButtonHit(MENU_PRINCIPAL, List_Bouton, Pos_Clic);
	}
	
	switch (Result_Button_Hit)
	{
		case 0:
			return 0;
			break;
		case 1:
			charge_labyrinthe("./maze", tab);
			return 1;
			break;
		case 2:
			return 0;
			break;
	}
			
	return 1; 
}

int Check_Win(Joueur J1, Joueur J2) //Regarde quel joueur a gagné / Retourne 1 si le joueur 1 a gagné et 2 si le joueur 2 a gagné
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


void Win(int isJ1, Bouton Liste_Bouton[NBR_BTN]) //Affiche la victoire d'un des deux joueurs
{

	int Result_Button_Hit = -1;
	
	Point Pos_Texte = {100, 100};
	Point Pos_Clic = {-1, -1};
	
	if (isJ1)
		afficher_texte("Le joueur 1 a gagné !", 24, Pos_Texte, blanc);
	else
		afficher_texte("Le joueur 2 a gagné !", 24, Pos_Texte, blanc);
	
	Afficher_Bouton(JEU, Liste_Bouton);
	
	actualiser();
	
	while (Result_Button_Hit == -1)
	{
		Pos_Clic = attendre_clic();
		Result_Button_Hit = isButtonHit(MENU_PRINCIPAL, Liste_Bouton, Pos_Clic);
	}
	
	switch (Result_Button_Hit)
	{
		case 3:
			break;
	}
	
}


void Deplacement(char tab[LIG][COL], Point Pos_Temp, Joueur *Joueur, int Espacement, Direction Direction, int isJ1) /* Fais déplacer le joueur dans le tableau du labyrinthe si il n'y pas de collision avec un mur */
{
	
	Tableau Tableau_Temp = {0, 0};
	char Char_Temp = ' ';
	
	Tableau_Temp = Get_Tab_Pos_By_Pos(tab, Pos_Temp, Espacement);

	Char_Temp = tab[Tableau_Temp.Ligne][Tableau_Temp.Colonne];

	if (Char_Temp != '*')
	{
		if (Direction == HAUT)
	 		Joueur -> Pos.y -= Espacement;
		else if (Direction == BAS)
			 Joueur -> Pos.y += Espacement;
		else if (Direction == GAUCHE)
			 Joueur -> Pos.x -= Espacement;
		else if (Direction == DROITE)
			 Joueur -> Pos.x += Espacement;
			 
		if (isJ1)
			Check_And_Change_Letter(tab, 1, 0, *Joueur);
		else
			Check_And_Change_Letter(tab, 0, 0, *Joueur);
			
	 	Joueur -> Pos_Tab = Tableau_Temp;
		
		if (isJ1)
			Check_And_Change_Letter(tab, 1, 1, *Joueur);
		else
			Check_And_Change_Letter(tab, 0, 1, *Joueur);

	}

}


void Check_And_Change_Letter(char tab[LIG][COL], int isJ1, int Phase2, Joueur Joueur) /* On regarde s'il n'y pas collision entre les 2 joueurs dans le tableau du lab */
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

Point Refresh_Maze(char tab[LIG][COL], Point Pos_J1, Point Pos_J2, int Espacement) //Raffraichit le labyrinthe
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

Tableau Get_Tab_Pos_By_Pos(char tab[LIG][COL], Point Pos, int Espacement) //Renvoit une coordonnée sur le tableau en fonction d'un point graphique
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

void Afficher_Bouton(Type_Bouton TypeBouton, Bouton Liste_Bouton[NBR_BTN]) //Affiche les boutons d'un type spécifié
{
	int i;
	
	for (i=0; i<NBR_BTN; i++)
	{
		if (Liste_Bouton[i].Type_Bouton == TypeBouton)
		{
			if (Liste_Bouton[i].Image[0] != '\0')
			{	
				//afficher_image(Liste_Bouton[i].Image, Liste_Bouton[i].Pos_HautG);
			}
			else
			{
			
				dessiner_rectangle(Liste_Bouton[i].Pos_HautG, mon_abs(Liste_Bouton[i].Pos_HautG.x - Liste_Bouton[i].Pos_BasD.x), mon_abs(Liste_Bouton[i].Pos_HautG.y - Liste_Bouton[i].Pos_BasD.y), white);
				if (Liste_Bouton[i].Texte[0] != '\0')
					afficher_texte(Liste_Bouton[i].Texte, 30, Liste_Bouton[i].Pos_HautG, black);

			}

		}
	}
	
}

int isButtonHit(Type_Bouton TypeBouton, Bouton Liste_Bouton[NBR_BTN], Point Pos_Clic)
{
	int i;
	
	for (i=0; i<NBR_BTN; i++)
	{
		if (Liste_Bouton[i].Type_Bouton == TypeBouton)
		{
			if (Pos_Clic.x >= Liste_Bouton[i].Pos_HautG.x && Pos_Clic.x <= Liste_Bouton[i].Pos_BasD.x && Pos_Clic.y >= Liste_Bouton[i].Pos_HautG.y && Pos_Clic.y <= Liste_Bouton[i].Pos_BasD.y)
			{
				return i;
			}
		}
	}
	
	return -1;
		
}

void Add_Buttons(Bouton Liste_Bouton[NBR_BTN])
{


	/* ======== Boutons du menu principal ======== */
	
	//Bouton Quitter
	Liste_Bouton[0].Type_Bouton = MENU_PRINCIPAL;
	Liste_Bouton[0].Pos_HautG.x = 100;
	Liste_Bouton[0].Pos_HautG.y = 100;
	Liste_Bouton[0].Pos_BasD.x = 300;
	Liste_Bouton[0].Pos_BasD.y = 150;
	Liste_Bouton[0].Texte = "Quitter";
	Liste_Bouton[0].Image = "";
	
	//Bouton Jouer
	Liste_Bouton[1].Type_Bouton = MENU_PRINCIPAL;
	Liste_Bouton[1].Pos_HautG.x = 350;
	Liste_Bouton[1].Pos_HautG.y = 100;
	Liste_Bouton[1].Pos_BasD.x = 550;
	Liste_Bouton[1].Pos_BasD.y = 150;
	Liste_Bouton[1].Texte = "Jouer";
	Liste_Bouton[1].Image = "";
	
	//Bouton Options
	Liste_Bouton[2].Type_Bouton = MENU_PRINCIPAL;
	Liste_Bouton[2].Pos_HautG.x = 600;
	Liste_Bouton[2].Pos_HautG.y = 100;
	Liste_Bouton[2].Pos_BasD.x = 800;
	Liste_Bouton[2].Pos_BasD.y = 150;
	Liste_Bouton[2].Texte = "Options";
	Liste_Bouton[2].Image = "";
	
	/* ======== Boutons en jeu ======== */
	
	Liste_Bouton[3].Type_Bouton = JEU;
	Liste_Bouton[3].Pos_HautG.x = 600;
	Liste_Bouton[3].Pos_HautG.y = 100;
	Liste_Bouton[3].Pos_BasD.x = 800;
	Liste_Bouton[3].Pos_BasD.y = 150;
	Liste_Bouton[3].Texte = "Quitter";
	Liste_Bouton[3].Image = "";
	
}

int mon_abs(int a)
{
	if (a > 0)
		return a;
	else
		return -a;
}

int min(int a, int b)
{
	if (a > b)
		return b;
	else if (a == b)
		return b;
	else
		return a;
}

void Clear_Screen() 
{
	Point Origin = {0, 0};
	dessiner_rectangle(Origin, FEN_X, FEN_Y, black);
}