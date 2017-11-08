#include "./lib/libgraphique.h"
#include "./charge_labyrinthe.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define COTE 10
#define FEN_X 1190
#define FEN_Y 950
#define BUILD_AUDIO 1
#define NBR_BTN 16

/* ======== Définitions des structures ======== */

typedef enum Direction { /* Enumérations des différentes direction de déplacement possible */
	HAUT, BAS, GAUCHE, DROITE
} Direction;

typedef enum Type_Bouton { /* Les différents emplacements où pourraient se trouver les boutons */
	MENU_PRINCIPAL, JEU, VICTOIRE, OPTIONS, EDITEUR
} Type_Bouton;

typedef struct Tableau { /* Structure définissant un tableau avec ses lignes et ses colonnes */
	int Ligne;
	int Colonne;
} Tableau;

typedef struct Sortie {
	Point Pos;
	Tableau Pos_Tab;
} Sortie;

typedef struct Joueur { /* Structure définissant un joueur */
	
	int Points; /* Les points que possède le joueur */
	Tableau Pos_Tab; /* Sa position dans le tableau du labyrinthe */
	Point Pos; /* Sa position graphiquement dans la fenêtre */
	Sortie Sortie; /* La sortie que le joueur doit rejoindre */
	Couleur Couleur;
	int nbrRecompRamasse; /* Le nombre de récompenses qu'un joueur à ramassé sur 3 */
} Joueur;

typedef struct Bouton {
	
	Point Pos_HautG; /* Position haut gauche d'un bouton */
	Point Pos_BasD; /* Position bas droite d'un bouton */
	char* Texte; /* Le texte du bouton */
	Type_Bouton Type_Bouton; /* Le menu dans lequel sera affiché le bouton */
	char* Image; /* Le chemin d'accès à l'image du bouton */
	
} Bouton;

typedef struct Recompense {
	Point Pos;
	Tableau Pos_Tab;
	int Status;
} Recompense;

/* ======== Variables globales ======== */

#if BUILD_AUDIO
Mix_Music *Musique; //Création d'un pointeur de type Mix_Music
#endif

int Espacement = 13;
int nbr_Lignes = 1;
int nbr_Colonnes = 1;

/* ======== Définitions des prototypes de fonctions ======== */

//Fonctions d'affichage
Sortie Refresh_Maze(char tab[nbr_Lignes][nbr_Colonnes], Point Pos_J1, Point Pos_J2, int Espacement);
void Refresh_Maze_Editeur(char tab[][72], Point Pos_Start, int nbr_Lignes, int nbr_Colonnes, int Espacement);
void Clear_Screen();
void Quadrillage(Point Start, Point End);

//Fonctions des menus
int Main_Menu(char tab[][100], Bouton Liste_Bouton[NBR_BTN]);
int Options(Bouton Liste_Bouton[NBR_BTN]);
int Editeur(Bouton Liste_Bouton[NBR_BTN]);

//Fonctions de déplacement
void Deplacement(char tab[nbr_Lignes][nbr_Colonnes], Point Pos_Temp, Joueur *Joueur, int Espacement, Direction Direction, int isJ1);
void Check_And_Change_Letter(char tab[nbr_Lignes][nbr_Colonnes], int isJ1, int Phase2, Joueur Joueur);
Tableau Get_Tab_Pos_By_Pos(char tab[][60], Point Pos, int Espacement);

//Fonctions lors de la victoire
int Check_Win(Joueur J1, Joueur J2);
int Win(int isJ1, Bouton Liste_Bouton[NBR_BTN]);

//Fonctions gérants les boutons
int isButtonHit(Type_Bouton TypeBouton, Bouton Liste_Bouton[NBR_BTN], Point Pos_Clic);
void Afficher_Bouton(Type_Bouton TypeBouton, Bouton Liste_Bouton[NBR_BTN]);
void Add_Buttons(Bouton Liste_Bouton[NBR_BTN]);

//Autres fonctions utiles
void Set_Char_Tab_By_Pos(char tab[][72], Point Pos_Start, Point Pos, char Char, int nbr_Lignes, int nbr_Colonnes, int Espacement);
char Get_Char_Tab_By_Pos(char tab[][72], Point Pos_Start, Point Pos, int nbr_Lignes, int nbr_Colonnes, int Espacement);
void Save_Tab_To_File(char Tab[][72], char *FileName, int nbr_Lignes, int nbr_Colonnes, int nbr_Sorties);
int min(int a, int b);
int mon_abs(int a);
char* Convert_To_String(int i);

/* ======== Main ======== */

int main(int argc, char *argv[])
{

	int MainStatus = 0; //Le status que l'on veut pour la boucle principale (Rejouer/Menu/Quitter)
	//int i;
	
	#if BUILD_AUDIO
	if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024) == -1) //Initialisation de SDL Mixer pour les musiques/son
		printf("%s", Mix_GetError());
	#endif
	
	//char **tab;

	//tab = (char **) malloc(nbr_Lignes * sizeof(char *));

	//for(i=0;i<nbr_Lignes;i++)
		//tab[i] = (char **) malloc(nbr_Colonnes * sizeof(char));
			
	while (MainStatus != -1) //Tant que MainStatus != -1 on recommence le programme
	{
		/* ======== Initialisation des variables ======== */

		char tab[100][100]; //Tableau contentant le labyrinthe
		
			
		Bouton Liste_Boutons[NBR_BTN]; //Tableau contenant les différents boutons des différents menus
		int col, lig; 
		int isJ1Win = 0;
		
		//Variables de la boucle de mouvement
		
		int Touche_Press = 1;
		
		Point Pos_Temp = {0, 0}, Pos_Score = {175, 840}, Pos_Score_Texte = {348, 892};
		
		//Variables des deux joueurs
		
		Joueur J1 = {0, {0, 0}, {0, 0}, {{0, 0}, {0, 0}}, yellow, 0}, J2 = {0, {0, 0} ,{0, 0}, {{0, 0}, {0, 0}}, yellow, 0};
		
		if (MainStatus == 0) //Si on ne veut pas rejouer
		{
			ouvrir_fenetre(FEN_X, FEN_Y);
			
			Set_Window_Title("Ultimate Maze Runner");
			
			Add_Buttons(Liste_Boutons);
			
			/* ======== Menu principal ======== */
			
			if (!Main_Menu(tab, Liste_Boutons)) //On lance le menu principal
				return 0; //Si il renvoie 0 cela veut dire que l'utilisateur veut quitter
		}

		Clear_Screen();
		
		actualiser();
		
		/* ======== Initialisation de la position des joueurs ======== */
		
		do
		{
		 	
			col = entier_aleatoire(nbr_Colonnes); //emplacement verticale
			lig = entier_aleatoire(nbr_Lignes); //emplacement horizontale
			
			while (tab[lig][col] == '*' || tab[lig][col] == 'S')
			{
				col = entier_aleatoire(nbr_Colonnes);
				lig = entier_aleatoire(nbr_Lignes);
			}
			
			tab[lig][col]= 'T';
		
			J1.Pos.x = col * Espacement;
			J1.Pos.y = lig * Espacement;
			
			J2.Pos.x = J1.Pos.x + 600;
			J2.Pos.y = J1.Pos.y;
		 	
		} while ((J1.Pos.x > (nbr_Lignes * Espacement) + 2) || (J1.Pos.y > (nbr_Colonnes * Espacement) + 2));
		
		/* On configure la position des 2 joueurs ainsi que celle de leur sortie */
		
		J1.Pos_Tab.Colonne = col;
		J1.Pos_Tab.Ligne = lig;
		 
		J2.Pos_Tab.Colonne = col;
		J2.Pos_Tab.Ligne = lig;

		J2.Sortie = Refresh_Maze(tab, J1.Pos, J2.Pos, Espacement);
		
		J1.Sortie.Pos.x = J2.Sortie.Pos.x - 600;
		J1.Sortie.Pos.y = J2.Sortie.Pos.y;
		
		J1.Sortie.Pos_Tab = J2.Sortie.Pos_Tab;
		
		
		/* ======== On positionne les récompenses ======== */
		
		
		
		/* ======== Création interface de jeu ======== */
		
		afficher_image("./Data/Pictures/J1_Score.bmp", Pos_Score);
		
		Pos_Score.x += 600;
		
		afficher_image("./Data/Pictures/J2_Score.bmp", Pos_Score);
		
		actualiser();
		
		/* ======== Boucle principale de jeu tant qu'un joueur n'a pas atteint une sortie ======== */
		
		#if BUILD_AUDIO
		Mix_HaltMusic();
		
		Mix_PlayMusic(Musique, -1);
		#endif
		
		while (!Check_Win(J1, J2))
		{
			
			//Point clic;
			//clic = attendre_clic();
			//printf("X: %d / Y: %d\n", clic.x, clic.y);
			
			traiter_evenements();
			
			Pos_Temp.x = 0;
			Pos_Temp.y = 0;
			
			if (touche_a_ete_pressee(SDLK_UP))
			{
				
				Pos_Temp.x = J1.Pos.x;
				Pos_Temp.y = J1.Pos.y - Espacement;
						
				Deplacement(tab, Pos_Temp, &J1, Espacement, HAUT, 1);	
				
				Touche_Press = 1;				
			}
			else if (touche_a_ete_pressee(SDLK_DOWN))
			{
				
				Pos_Temp.x = J1.Pos.x;
				Pos_Temp.y = J1.Pos.y + Espacement;
					
				Deplacement(tab, Pos_Temp, &J1, Espacement, BAS, 1);	
				
				Touche_Press = 1;										
			}
			else if (touche_a_ete_pressee(SDLK_LEFT))
			{
				
				Pos_Temp.x = J1.Pos.x - Espacement;
				Pos_Temp.y = J1.Pos.y;
						
				Deplacement(tab, Pos_Temp, &J1, Espacement, GAUCHE, 1);	
				
				Touche_Press = 1;						
			}
			else if (touche_a_ete_pressee(SDLK_RIGHT))
			{
				
				Pos_Temp.x = J1.Pos.x + Espacement;
				Pos_Temp.y = J1.Pos.y;
						
				Deplacement(tab, Pos_Temp, &J1, Espacement, DROITE, 1); 
				
				Touche_Press = 1;							
			}
				
			if (touche_a_ete_pressee(SDLK_z))
			{
				
				Pos_Temp.x = J2.Pos.x;
				Pos_Temp.y = J2.Pos.y - Espacement;
						
				Deplacement(tab, Pos_Temp, &J2, Espacement, HAUT, 0);
				
				Touche_Press = 1;				
			}
			else if (touche_a_ete_pressee(SDLK_s))
			{
				
				Pos_Temp.x = J2.Pos.x;
				Pos_Temp.y = J2.Pos.y + Espacement;
						
				Deplacement(tab, Pos_Temp, &J2, Espacement, BAS, 0);
				
				Touche_Press = 1;				
			}
			else if (touche_a_ete_pressee(SDLK_q))
			{
				
				Pos_Temp.x = J2.Pos.x - Espacement;
				Pos_Temp.y = J2.Pos.y;

				Deplacement(tab, Pos_Temp, &J2, Espacement, GAUCHE, 0);	
				
				Touche_Press = 1;
			}
			else if (touche_a_ete_pressee(SDLK_d))
			{
				
				Pos_Temp.x = J2.Pos.x + Espacement;
				Pos_Temp.y = J2.Pos.y;
						
				Deplacement(tab, Pos_Temp, &J2, Espacement, DROITE, 0);	
				
				Touche_Press = 1;
			}
			
			if (touche_a_ete_pressee(SDLK_ESCAPE))
			{
				return 0;
			}
					
			if (touche_a_ete_pressee(SDLK_F1))
			{
				J1.Pos.x = J1.Sortie.Pos.x;
				J1.Pos.y = J1.Sortie.Pos.y;
				
				J1.Pos_Tab = J1.Sortie.Pos_Tab;
			}
			
			if (touche_a_ete_pressee(SDLK_F2))
			{
				J2.Pos.x = J2.Sortie.Pos.x;
				J2.Pos.y = J2.Sortie.Pos.y;
				
				J2.Pos_Tab = J2.Sortie.Pos_Tab;
			}
			
			reinitialiser_evenements();
			
			if (Touche_Press) // Si une touche à été préssée on refresh l'interface afin de réduire le lag
			{			
				/* ======== Affichage du score ======== */
				
				if (J1.Points >= 1000) //On décale le texte en fonction du nbr de chiffres
					Pos_Score_Texte.x -= 5;
				
				dessiner_rectangle(Pos_Score_Texte, 51, 51, black); //On recouvre l'ancien texte
				
				afficher_texte(Convert_To_String(J1.Points), 20, Pos_Score_Texte, white); //On affiche le nouveau score
				
				if (J2.Points >= 1000)
				{
					if (J1.Points <= 1000)
						Pos_Score_Texte.x -= 5;
				}
				
				Pos_Score_Texte.x += 600; //On change de coord pour le J2
				
				dessiner_rectangle(Pos_Score_Texte, 51, 51, black); //On recouvre l'ancien texte du J2
				
				afficher_texte(Convert_To_String(J2.Points), 20, Pos_Score_Texte, white); //On affiche le nouveau score du J2
				
				Pos_Score_Texte.x -= 600; //On remet les coord de base
				
				if (J1.Points >= 1000 || J2.Points >= 1000)
					Pos_Score_Texte.x += 5;
				
				Refresh_Maze(tab, J1.Pos, J2.Pos, Espacement); //On met à jour le lab
			
				actualiser(); //On actualise le tout
			
			}
			
			Touche_Press = 0;
			
		}
		
		isJ1Win = Check_Win(J1, J2); /* On regarde si un joueur à atteint la sortie */
		
		if (isJ1Win == 1) //On remet la sortie dans la tableau
 			tab[J1.Pos_Tab.Ligne][J1.Pos_Tab.Colonne] = 'S';
		else if(isJ1Win == 2)
 			tab[J2.Pos_Tab.Ligne][J2.Pos_Tab.Colonne] = 'S';
		
		switch (Win(isJ1Win, Liste_Boutons))
		{
			case 3: //Rejouer
				MainStatus = 1;
				break;
			case 4: //Menu
				MainStatus = 0;
				break;
			case 5: //On quitte le jeu
				MainStatus = -1;
				break;
		}
	
	}
    
    // Fin de la session graphique
    
   #if BUILD_AUDIO
    Mix_FreeMusic(Musique);
    Mix_CloseAudio();
    #endif
    
    fermer_fenetre();
    return 0;
}

/* ======== Fonctions ======== */

int Main_Menu(char tab[][100], Bouton Liste_Bouton[NBR_BTN]) //Affiche le menu principal
{

	system("clear");
	printf("======== Ultimate Maze Runner ========\n");
	
	int Result_Button_Hit = -1;
	Point Pos_Clic = {-1, -1}, Pos_Menu = {200, 50};
	SizeTab Taille_Tab;
	
	#if BUILD_AUDIO
	Musique = Mix_LoadMUS("./Data/Music/Main_Menu.mp3"); //On charge la musique du menu
	Mix_PlayMusic(Musique, -1); //On la joue indéfiniement
	
	Musique = Mix_LoadMUS("./Data/Music/Game.mp3"); //On précharge la musique du jeu
	#endif
	
	while (1)
	{
		Clear_Screen();
		
		afficher_image("./Data/Pictures/Menu.bmp", Pos_Menu);
		 
		actualiser();
		
		while (Result_Button_Hit == -1)
		{
			Pos_Clic = attendre_clic();
			//printf("X : %d / Y : %d\n", Pos_Clic.x, Pos_Clic.y);
			Result_Button_Hit = isButtonHit(MENU_PRINCIPAL, Liste_Bouton, Pos_Clic);
		}
		
		switch (Result_Button_Hit)
		{
			case 0:
				return 0;
				break;
			case 1:
			
				Taille_Tab = charge_labyrinthe("./Data/Maze/maze", tab);
			
				nbr_Colonnes = Taille_Tab.nbr_Colonnes;
				nbr_Lignes = Taille_Tab.nbr_Lignes;
				
				/* int add_rows = nbr_Lignes - 1;
				int **tmp = realloc( tab, sizeof *tab * (nbr_Lignes + add_rows) );
				
				if (tmp)
				{
						tab = tmp;
						
						for ( size_t i = 0; i < add_rows; i++ )
						{
							tab[nbr_Lignes + i] = malloc( sizeof *tab[nbr_Lignes + i] * nbr_Colonnes );
						}
				} */
				
				return 1;
				
				break;
				
			case 2:	
				Options(Liste_Bouton);
				break;
		}
		
		Result_Button_Hit = -1;
	}
}

int Options(Bouton Liste_Bouton[NBR_BTN])
{
	int Result_Button_Hit = -1;
	Point Pos_Clic = {-1, -1};
	
	while (1)
	{
		Clear_Screen();
		
		Afficher_Bouton(OPTIONS, Liste_Bouton);
		
		actualiser();
		
		while (Result_Button_Hit == -1)
		{
			Pos_Clic = attendre_clic();
			Result_Button_Hit = isButtonHit(OPTIONS, Liste_Bouton, Pos_Clic);
		}
		
		switch (Result_Button_Hit)
		{
			case 6: //Changement de labyrinthe
				printf("Changement de labyrinthe !\n");
				break;
			case 7: //Retour au menu principal
				return 0;
				break;
			case 8: //Editeur de labyrinthe
				Editeur(Liste_Bouton);
				break;
		}
		
		Result_Button_Hit = -1;
	}
}

int Editeur(Bouton Liste_Bouton[NBR_BTN])
{
	int Result_Button_Hit = -1, Largeur = 2, Hauteur = 1;
	
	Point Pos_Clic = {-1, -1};
	
	Point Pos_Texte_HL = {59, 108};
	Point LigneH_P1 = {0, 0}, LigneH_P2 = {0, 0}, LigneL = {LigneH_P2.x + (Largeur * Espacement), LigneH_P1.y};
	
	char Texte_Fixe[12] = "Largeur : "; //Contient "Longueur : " ou "Largeur : "
	char* Texte_Variable = Convert_To_String(Largeur); //Contient la longueur ou la largeur
	
	int isRightClick = 0, nbr_Sorties = 0;
	
	char Tab[64][72] = {{' '}}; //Tableau qui contiendra le lab
	
	while (1)
	{
		Clear_Screen();
		
		/* ======== Affichage de l'interface (Boutons) ======== */
		
		Afficher_Bouton(EDITEUR, Liste_Bouton); //On affiche les boutons
		
		//Texte d'explication fixe
		
		
		
		//Texte Largeur et Hauteur
		 
		afficher_texte(Texte_Fixe,  20, Pos_Texte_HL, white); //On affiche : "Largeur : "
		
		Pos_Texte_HL.x += 100; //On se place au bon endroit pour le texte variable
		
		strcpy(Texte_Variable, Convert_To_String(Largeur * 2));
		
		afficher_texte(Texte_Variable,  20, Pos_Texte_HL, white); //On affiche la largeur actuelle
		
		Pos_Texte_HL.x -= 100; //On remet comme avant
		
		strcpy(Texte_Fixe, "Hauteur : "); //On rechange le texte fixe
		strcpy(Texte_Variable, Convert_To_String(Hauteur)); //On rechange le texte variable pour afficher la hauteur
		
		Pos_Texte_HL.y += 50; //On se place au bon endroit pour le second texte fixe
		
		afficher_texte(Texte_Fixe, 20, Pos_Texte_HL, white); //On affiche "Hauteur : "
		
		Pos_Texte_HL.x += 100; //On se place au bon endroit pour le second texte variable
		
		afficher_texte(Texte_Variable,  20, Pos_Texte_HL, white); //On affiche la hauteur actuelle
		
		Pos_Texte_HL.x -= 100; //On se replace au bon endroit pour le premier texte fixe
		Pos_Texte_HL.y -= 50;
		
		strcpy(Texte_Fixe, "Largeur : "); //On rechange le texte fixe
		strcpy(Texte_Variable, Convert_To_String(Largeur * 2)); //On rechange le texte variable par la largeur
		
		/* ======== Affichage de l'interface (Editeur) ======== */
		
		//Affichage des lignes de hauteur
		
		LigneH_P1.x = 700 - ((Largeur * Espacement)); 
		
		LigneH_P2.x = LigneH_P1.x; 
		LigneH_P2.y = Hauteur * Espacement;
		
		dessiner_ligne(LigneH_P1, LigneH_P2, white); //Affichage de la première ligne de hauteur
		
		LigneL.x = 700 + ((Largeur * Espacement));
		LigneL.y = LigneH_P2.y;
		
		Liste_Bouton[15].Pos_HautG = LigneH_P1;
		Liste_Bouton[15].Pos_BasD = LigneL;
		
		dessiner_ligne(LigneH_P2, LigneL, white); //Affichage de la ligne de largeur

		LigneH_P1.x = 700 + ((Largeur * Espacement));
		LigneH_P2.x = LigneH_P1.x;
		
		//Affichage de la seconde ligne de hauteur
		//printf("LigneH_P1.x : %d / LigneH_P2.x : %d\n", LigneH_P1.x, LigneH_P2.x);
		dessiner_ligne(LigneH_P1, LigneH_P2, white);
		
		Liste_Bouton[15].Pos_HautG.x += 1;
		Liste_Bouton[15].Pos_HautG.y += 1;
		
		Refresh_Maze_Editeur(Tab, Liste_Bouton[15].Pos_HautG, Largeur * 2, Hauteur, Espacement);
		
		Liste_Bouton[15].Pos_HautG.x -= 1;
		Liste_Bouton[15].Pos_HautG.y -= 1;
		
		Quadrillage(Liste_Bouton[15].Pos_HautG, Liste_Bouton[15].Pos_BasD);
		
		actualiser();
		
		while (Result_Button_Hit == -1)
		{
			Pos_Clic = attendre_clic_gauche_droite();
			
			if (Pos_Clic.x <= 0) //On regarde si le clic est un clic droit
				isRightClick = 1;
			
			//printf("X : %d / Y : %d\n", Pos_Clic.x, Pos_Clic.y);
			
			//On récupére les coordonnées positives du clic
			Pos_Clic.x = mon_abs(Pos_Clic.x);
			Pos_Clic.y = mon_abs(Pos_Clic.y);
			
			//On regarde si un bouton est cliqué
			Result_Button_Hit = isButtonHit(EDITEUR, Liste_Bouton, Pos_Clic);
		}
		
		switch (Result_Button_Hit) //On regarde quel bouton est cliqué
		{
			case 9: //Retour au menu options
				return 0;
				break;
			case 10: // + Largeur
				if (Largeur * 2 < 72)
					Largeur += 2;
				break;
			case 11: // - Largeur
				if (Largeur >= 4)
					Largeur -= 2;
				break;
			case 12: // + Hauteur
				if (Hauteur < 64)
					Hauteur += 1;
				break;
			case 13: // - Hauteur
				if (Hauteur > 1)
					Hauteur -= 1;
				break;
			case 14: // Sauvegarder et quitter
				Save_Tab_To_File(Tab, "./Data/Maze/Custom_Maze", Largeur * 2, Hauteur, nbr_Sorties);
				return 0;
				break;
			case 15:
				
				//On ajuste les coordonnées pour avoir le coin supérieur gauche
				Pos_Clic.x = Pos_Clic.x - (Pos_Clic.x % Espacement);
				Pos_Clic.y = Pos_Clic.y - (Pos_Clic.y % Espacement);
				
				//On observe quel est le caractère actuellement présent où l'on a cliqué
				char Char_Tab = Get_Char_Tab_By_Pos(Tab, Liste_Bouton[15].Pos_HautG, Pos_Clic, Largeur * 2, Hauteur, Espacement);
				
				if (isRightClick) //Si c'est un clic droit
				{
					if (Char_Tab == 'S') //Si le caractère est une sortie alors un met un mur
					{
						Set_Char_Tab_By_Pos(Tab, Liste_Bouton[15].Pos_HautG, Pos_Clic, '*', Largeur * 2, Hauteur, Espacement);
						nbr_Sorties -= 1;
					}
					else //Sinon on met du vide
						Set_Char_Tab_By_Pos(Tab, Liste_Bouton[15].Pos_HautG, Pos_Clic, ' ', Largeur * 2, Hauteur, Espacement);
				}
				else //Si c'est un clic gauche
				{
					if (Char_Tab == '*') //Si le caractère est un mur alors on met une sortie
					{
						Set_Char_Tab_By_Pos(Tab, Liste_Bouton[15].Pos_HautG, Pos_Clic, 'S', Largeur * 2, Hauteur, Espacement);
						nbr_Sorties += 1;
					}
					else //Sinon on met un mur
						Set_Char_Tab_By_Pos(Tab, Liste_Bouton[15].Pos_HautG, Pos_Clic, '*', Largeur * 2, Hauteur, Espacement);

				}
				
				break;
		}
		
		//On remet les variables à leur état initial
		isRightClick = 0;
		Result_Button_Hit = -1;
		Pos_Clic.x = -1;
	}
}

int Check_Win(Joueur J1, Joueur J2) //Regarde quel joueur a gagné / Retourne 1 si le joueur 1 a gagné et 2 si le joueur 2 a gagné
{
	if (((J1.Pos.x != J1.Sortie.Pos.x) || (J1.Pos.y != J1.Sortie.Pos.y)) && ((J2.Pos.x != J2.Sortie.Pos.x) || (J2.Pos.y != J2.Sortie.Pos.y)))
		return 0;
	else	
	{
		if (((J1.Pos.x == J1.Sortie.Pos.x) && (J1.Pos.y == J1.Sortie.Pos.y)))
			return 1;
		else
			return 2;
	}
}


int Win(int playerWin, Bouton Liste_Bouton[NBR_BTN]) //Affiche la victoire d'un des deux joueurs
{

	int Result_Button_Hit = -1;
	
	Point Pos_Clic = {-1, -1}, Pos_Picture = {310, 100};

	if (playerWin == 1)
		afficher_image("./Data/Pictures/GagneJ1.bmp", Pos_Picture);
	else if (playerWin == 2)
		afficher_image("./Data/Pictures/GagneJ2.bmp", Pos_Picture);
	
	actualiser();
	
	while (Result_Button_Hit == -1)
	{
		Pos_Clic = attendre_clic();
		Result_Button_Hit = isButtonHit(VICTOIRE, Liste_Bouton, Pos_Clic);
	}
	
	switch (Result_Button_Hit)
	{
		case 3: //Rejouer
			return 3;	
			break;
		case 4: //Menu
			return 4;
			break;
		case 5: //Quitter
			return 5;
			break;
		default:
			return -1;
	}
	
}


void Deplacement(char tab[nbr_Lignes][nbr_Colonnes], Point Pos_Temp, Joueur *Joueur, int Espacement, Direction Direction, int isJ1) /* Fais déplacer le joueur dans le tableau du labyrinthe si il n'y pas de collision avec un mur */
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

void Check_And_Change_Letter(char tab[nbr_Lignes][nbr_Colonnes], int isJ1, int Phase2, Joueur Joueur) /* On regarde s'il n'y pas collision entre les 2 joueurs dans le tableau du lab */
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

Sortie Refresh_Maze(char tab[nbr_Lignes][nbr_Colonnes], Point Pos_J1, Point Pos_J2, int Espacement) //Raffraichit le labyrinthe
{

	int i, l, c;
	Point Coin = {0, 0};
	Sortie Sortie = {{0, 0}, {0, 0}};
	
	for(i=0; i < 2; i++)
	{
		for(l=0; l!=nbr_Lignes; l++)
    	{
			for(c=0; c!=nbr_Colonnes; c++)
		   	{
		  
		   		if (tab[l][c] == '*')
		        	//dessiner_rectangle(Coin, Espacement, Espacement, marron);
		        	afficher_image("./Data/Pictures/Brique.bmp", Coin);
		        	
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
		   			afficher_image("./Data/Pictures/Sortie.bmp", Coin);
		   			
		   			Sortie.Pos.x = Coin.x;
		   			Sortie.Pos.y = Coin.y;
		   			Sortie.Pos_Tab.Ligne = l;
		   			Sortie.Pos_Tab.Colonne = c;
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

void Refresh_Maze_Editeur(char tab[][72], Point Pos_Start, int nbr_Lignes, int nbr_Colonnes, int Espacement) //Raffraichit le labyrinthe
{

	int l, c;
	Point Coin = {Pos_Start.x, Pos_Start.y};
	
	for(l=0; l!=nbr_Lignes; l++)
	{
		for(c=0; c!=nbr_Colonnes; c++)
		{
			//printf("Check tab[%d][%d] who have : %c !\n", l, c, tab[l][c]) ;
			if (tab[l][c] == '*')
				afficher_image("./Data/Pictures/Brique.bmp", Coin);
			else if (tab[l][c] == 'S')
				afficher_image("./Data/Pictures/Sortie.bmp", Coin);
			else if (tab[l][c] == ' ')
				dessiner_rectangle(Coin, Espacement, Espacement, black);
				
			Coin.y += Espacement;
				
			//printf("%c",tab[l][c]);
		}
		
		Coin.y = Pos_Start.y;
		Coin.x += Espacement;
		//printf("\n");
	}
}

void Save_Tab_To_File(char Tab[][72], char *FileName, int nbr_Lignes, int nbr_Colonnes, int nbr_Sorties)
{
	int l, c;
	FILE* fichier = NULL;
	
	fichier = fopen(FileName, "w");
	
	if (fichier != NULL)
	{
		fprintf(fichier, "%d %d %d\n", nbr_Lignes, nbr_Colonnes, nbr_Sorties); //On écrit le nombre de lignes et de colonnes
		
		for(l=0; l!=nbr_Lignes; l++)
		{
			for(c=0; c!=nbr_Colonnes; c++)
			{
				if (Tab[l][c] == ' ' || Tab[l][c] == '*' || Tab[l][c] == 'S')
					fputc(Tab[l][c], fichier);
				else
					fputc(' ', fichier);
			}
			
			fputc('\n', fichier);
		}
		
		fclose(fichier);
	}
}

Tableau Get_Tab_Pos_By_Pos(char tab[][60], Point Pos, int Espacement) //Renvoit une coordonnée sur le tableau en fonction d'un point graphique
{
	int i, l, c;
	Point Coin = {0, 0};
	Tableau Pos_J = {0, 0};
	
	for(i=0; i < 2; i++)
	{
		for(l=0; l!=nbr_Lignes; l++)
    	{ 
			for(c=0; c!=nbr_Colonnes; c++)
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

char Get_Char_Tab_By_Pos(char tab[][72], Point Pos_Start, Point Pos, int nbr_Lignes, int nbr_Colonnes, int Espacement)
{
	int l, c;
	Point Coin = {Pos_Start.x, Pos_Start.y};
	
	for(l=0; l!=nbr_Lignes; l++)
	{
		for(c=0; c!=nbr_Colonnes; c++)
		{
			//printf("Check : tab[%d][%d] with Coin.x = %d / Coin.y = %d and Pos.x = %d / Pos.y = %d\n", l, c, Coin.x, Coin.y, Pos.x, Pos.y);
			
			if (Coin.x + 2 == Pos.x && Coin.y == Pos.y)
			{
				return tab[l][c];
			}
			
			Coin.y += Espacement;
		}
		
		Coin.y = Pos_Start.y;
		Coin.x += Espacement;
	}
	
	return ' ';
	
}

void Set_Char_Tab_By_Pos(char tab[][72], Point Pos_Start, Point Pos, char Char, int nbr_Lignes, int nbr_Colonnes, int Espacement)
{
	int l, c;
	Point Coin = {Pos_Start.x, Pos_Start.y};
	
	for(l=0; l!=nbr_Lignes; l++)
	{
		for(c=0; c!=nbr_Colonnes; c++)
		{
			//printf("Check : tab[%d][%d] with Coin.x = %d / Coin.y = %d and Pos.x = %d / Pos.y = %d\n", l, c, Coin.x, Coin.y, Pos.x, Pos.y);
			
			if (Coin.x + 2 == Pos.x && Coin.y == Pos.y)
			{
				//printf("Char set !\n");
				//printf("Before : %c\n", tab[l][c]);
				tab[l][c] = Char;
				//printf("After : %c\n", tab[l][c]);
			}
			
			Coin.y += Espacement;
		}
		
		Coin.y = Pos_Start.y;
		Coin.x += Espacement;
	}
	
}

void Afficher_Bouton(Type_Bouton TypeBouton, Bouton Liste_Bouton[NBR_BTN]) //Affiche les boutons d'un type spécifié
{
	Point Taille_Texte = {0, 0};
	int i;
	
	for (i=0; i<NBR_BTN; i++)
	{
		if (Liste_Bouton[i].Type_Bouton == TypeBouton)
		{
			if (Liste_Bouton[i].Image[0] != '\0')
			{	
				afficher_image(Liste_Bouton[i].Image, Liste_Bouton[i].Pos_HautG);
			}
			else
			{
				Taille_Texte = taille_texte(Liste_Bouton[i].Texte, 30);
				dessiner_rectangle(Liste_Bouton[i].Pos_HautG, Taille_Texte.x, Taille_Texte.y, white);
				if (Liste_Bouton[i].Texte[0] != '\0')
					afficher_texte(Liste_Bouton[i].Texte, 30, Liste_Bouton[i].Pos_HautG, black);

			}

		}
	}
	
}

int isButtonHit(Type_Bouton TypeBouton, Bouton Liste_Bouton[NBR_BTN], Point Pos_Clic)
{
	int i;
	Point Taille_Texte = {0, 0};
	
	for (i=0; i<NBR_BTN; i++)
	{
		if (Liste_Bouton[i].Type_Bouton == TypeBouton)
		{
			
			Taille_Texte = taille_texte(Liste_Bouton[i].Texte, 30);
			
			if (Pos_Clic.x >= Liste_Bouton[i].Pos_HautG.x && Pos_Clic.x <= Liste_Bouton[i].Pos_BasD.x && Pos_Clic.y >= Liste_Bouton[i].Pos_HautG.y && Pos_Clic.y <= Liste_Bouton[i].Pos_BasD.y)
			{
				return i;
			}
			else if (Pos_Clic.x >= Liste_Bouton[i].Pos_HautG.x && Pos_Clic.x <= Liste_Bouton[i].Pos_HautG.x + Taille_Texte.x && Pos_Clic.y >= Liste_Bouton[i].Pos_HautG.y && Pos_Clic.y <= Liste_Bouton[i].Pos_HautG.y + Taille_Texte.y)
			{
				return i;
			}
		}
	}
	
	return -1;
		
}

void Quadrillage(Point Start, Point End)
{
	Point Pos_P1 = {Start.x, Start.y}, Pos_P2 = {Start.x, End.y};
	
	while(Pos_P1.x <= End.x)
	{
		dessiner_ligne(Pos_P1, Pos_P2, white);
		Pos_P1.x += Espacement;
		Pos_P2.x += Espacement;
	}
	
	Pos_P1.x = Start.x;
	Pos_P1.y = Start.y;
	
	Pos_P2.x = End.x;
	Pos_P2.y = Start.y;
	
	while (Pos_P1.y <= End.y)
	{
		dessiner_ligne(Pos_P1, Pos_P2, white);
		Pos_P1.y += Espacement;
		Pos_P2.y += Espacement;
	}
}

void Add_Buttons(Bouton Liste_Bouton[NBR_BTN])
{


	/* ======== Boutons du menu principal ======== */
	
	//Bouton Quitter
	Liste_Bouton[0].Type_Bouton = MENU_PRINCIPAL;
	Liste_Bouton[0].Pos_HautG.x = 512;
	Liste_Bouton[0].Pos_HautG.y = 526;
	Liste_Bouton[0].Pos_BasD.x = 676;
	Liste_Bouton[0].Pos_BasD.y = 561;
	Liste_Bouton[0].Texte = "Quitter";
	Liste_Bouton[0].Image = "";
	
	//Bouton Jouer
	Liste_Bouton[1].Type_Bouton = MENU_PRINCIPAL;
	Liste_Bouton[1].Pos_HautG.x = 525;
	Liste_Bouton[1].Pos_HautG.y = 313;
	Liste_Bouton[1].Pos_BasD.x = 663;
	Liste_Bouton[1].Pos_BasD.y = 353;
	Liste_Bouton[1].Texte = "Jouer";
	Liste_Bouton[1].Image = "";
	
	//Bouton Options
	Liste_Bouton[2].Type_Bouton = MENU_PRINCIPAL;
	Liste_Bouton[2].Pos_HautG.x = 524;
	Liste_Bouton[2].Pos_HautG.y = 426;
	Liste_Bouton[2].Pos_BasD.x = 667;
	Liste_Bouton[2].Pos_BasD.y = 458;
	Liste_Bouton[2].Texte = "Options";
	Liste_Bouton[2].Image = "";
	
	/* ======== Boutons sur l'écran de victoire ======== */
	
	Liste_Bouton[3].Type_Bouton = VICTOIRE;
	Liste_Bouton[3].Pos_HautG.x = 429;
	Liste_Bouton[3].Pos_HautG.y = 324;
	Liste_Bouton[3].Pos_BasD.x = 800;
	Liste_Bouton[3].Pos_BasD.y = 441;
	Liste_Bouton[3].Texte = "Rejouer";
	Liste_Bouton[3].Image = "";
	
	Liste_Bouton[4].Type_Bouton = VICTOIRE;
	Liste_Bouton[4].Pos_HautG.x = 537;
	Liste_Bouton[4].Pos_HautG.y = 540;
	Liste_Bouton[4].Pos_BasD.x = 704;
	Liste_Bouton[4].Pos_BasD.y = 579;
	Liste_Bouton[4].Texte = "Menu";
	Liste_Bouton[4].Image = "";
	
	Liste_Bouton[5].Type_Bouton = VICTOIRE;
	Liste_Bouton[5].Pos_HautG.x = 501;
	Liste_Bouton[5].Pos_HautG.y = 674;
	Liste_Bouton[5].Pos_BasD.x = 743;
	Liste_Bouton[5].Pos_BasD.y = 716;
	Liste_Bouton[5].Texte = "Quitter";
	Liste_Bouton[5].Image = "";
	
	/* ======== Boutons du menu options ======== */
	
	Liste_Bouton[6].Type_Bouton = OPTIONS;
	Liste_Bouton[6].Pos_HautG.x = 100;
	Liste_Bouton[6].Pos_HautG.y = 100;
	Liste_Bouton[6].Pos_BasD.x = 150;
	Liste_Bouton[6].Pos_BasD.y = 150;
	Liste_Bouton[6].Texte = "Labyrinthe : Défaut";
	Liste_Bouton[6].Image = "";
	
	Liste_Bouton[7].Type_Bouton = OPTIONS;
	Liste_Bouton[7].Pos_HautG.x = 100;
	Liste_Bouton[7].Pos_HautG.y = 400;
	Liste_Bouton[7].Pos_BasD.x = 150;
	Liste_Bouton[7].Pos_BasD.y = 550;
	Liste_Bouton[7].Texte = "Sauvegarder et quitter";
	Liste_Bouton[7].Image = "";
	
	Liste_Bouton[8].Type_Bouton = OPTIONS;
	Liste_Bouton[8].Pos_HautG.x = 100;
	Liste_Bouton[8].Pos_HautG.y = 300;
	Liste_Bouton[8].Pos_BasD.x = 150;
	Liste_Bouton[8].Pos_BasD.y = 350;
	Liste_Bouton[8].Texte = "Editeur de labyrinthe";
	Liste_Bouton[8].Image = "";
	
	/* ======== Boutons de l'éditeur de labyrinthe ======== */
	
	Liste_Bouton[9].Type_Bouton = EDITEUR;
	Liste_Bouton[9].Pos_HautG.x = 65;
	Liste_Bouton[9].Pos_HautG.y = 845;
	Liste_Bouton[9].Pos_BasD.x = 452;
	Liste_Bouton[9].Pos_BasD.y = 882;
	Liste_Bouton[9].Texte = "Quitter sans sauvegarder";
	Liste_Bouton[9].Image = "";
	
	Liste_Bouton[10].Type_Bouton = EDITEUR; // + Largeur
	Liste_Bouton[10].Pos_HautG.x = 25;
	Liste_Bouton[10].Pos_HautG.y = 105;
	Liste_Bouton[10].Pos_BasD.x = 50;
	Liste_Bouton[10].Pos_BasD.y = 144;
	Liste_Bouton[10].Texte = "+";
	Liste_Bouton[10].Image = "";
	
	Liste_Bouton[11].Type_Bouton = EDITEUR; // - Largeur
	Liste_Bouton[11].Pos_HautG.x = 195;
	Liste_Bouton[11].Pos_HautG.y = 105;
	Liste_Bouton[11].Pos_BasD.x = 210;
	Liste_Bouton[11].Pos_BasD.y = 145;
	Liste_Bouton[11].Texte = "-";
	Liste_Bouton[11].Image = "";
	
	Liste_Bouton[12].Type_Bouton = EDITEUR; // + Hauteur
	Liste_Bouton[12].Pos_HautG.x = 25;
	Liste_Bouton[12].Pos_HautG.y = 155;
	Liste_Bouton[12].Pos_BasD.x = 50;
	Liste_Bouton[12].Pos_BasD.y = 195;
	Liste_Bouton[12].Texte = "+";
	Liste_Bouton[12].Image = "";
	
	Liste_Bouton[13].Type_Bouton = EDITEUR; // - Hauteur
	Liste_Bouton[13].Pos_HautG.x = 195;
	Liste_Bouton[13].Pos_HautG.y = 155;
	Liste_Bouton[13].Pos_BasD.x = 210;
	Liste_Bouton[13].Pos_BasD.y = 195;
	Liste_Bouton[13].Texte = "-";
	Liste_Bouton[13].Image = "";
	
	Liste_Bouton[14].Type_Bouton = EDITEUR;
	Liste_Bouton[14].Pos_HautG.x = 725;
	Liste_Bouton[14].Pos_HautG.y = 845;
	Liste_Bouton[14].Pos_BasD.x = 1071;
	Liste_Bouton[14].Pos_BasD.y = 883;
	Liste_Bouton[14].Texte = "Sauvegarder et quitter";
	Liste_Bouton[14].Image = "";
	
	Liste_Bouton[15].Type_Bouton = EDITEUR;
	Liste_Bouton[15].Pos_HautG.x = 0;
	Liste_Bouton[15].Pos_HautG.y = 0;
	Liste_Bouton[15].Pos_BasD.x = 0;
	Liste_Bouton[15].Pos_BasD.y = 0;
	Liste_Bouton[15].Texte = "";
	Liste_Bouton[15].Image = "";
	
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

char* Convert_To_String(int i)
{
	int Longueur = snprintf( NULL, 0, "%d", i);
	char* str = malloc(Longueur + 1);
	snprintf(str, Longueur + 1, "%d", i);
	return str;
	
}
