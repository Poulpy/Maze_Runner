#include "./lib/libgraphique.h"
#include "./charge_labyrinthe.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//Dimensions de la fenêtre
#define FEN_X 1190
#define FEN_Y 950

#define DEBUG 1 //Activer/Désactiver le mode debug
#define BUILD_AUDIO 1 //Activer/Désactiver la compilation de la partie son
#define NBR_BTN 16 //Définition du nombre de boutons présents dans le programme

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

typedef struct Sorties { /* Structure définissant les 2 sorties */
	Point Pos1;
	Tableau Pos_Tab1;
	Point Pos2;
	Tableau Pos_Tab2;
} Sorties;

typedef struct Joueur { /* Structure définissant un joueur */
	char Nom[6]; /* Le nom d'un joueur */
	int Score; /* Les Score que possède le joueur */
	Tableau Pos_Tab; /* Sa position dans le tableau du labyrinthe */
	Point Pos; /* Sa position graphiquement dans la fenêtre */
	Sorties Sortie; /* La sortie que le joueur doit rejoindre */
	Couleur Couleur;
	int nbrRecompRamasse; /* Le nombre de récompenses qu'un joueur à ramassé sur 3 */
} Joueur;

typedef struct Bouton { /* Structure définissant un joueur */
	
	Point Pos_HautG; /* Position haut gauche d'un bouton */
	Point Pos_BasD; /* Position bas droite d'un bouton */
	char* Texte; /* Le texte du bouton */
	Type_Bouton Type_Bouton; /* Le menu dans lequel sera affiché le bouton */
	char* Image; /* Le chemin d'accès à l'image du bouton */
	
} Bouton;

typedef struct Recompense { /* Structure définissant une récompense (inutilisée)*/
	Point Pos;
	Tableau Pos_Tab;
	int Status;
} Recompense;

/* ======== Variables globales ======== */

#if BUILD_AUDIO
Mix_Music *Musique; //Création d'un pointeur de type Mix_Music
#endif

int Espacement = 13; // L'espacement entre chaque case
int nbr_Lignes = LIG; //Nombre de lignes du labyrinthe
int nbr_Colonnes = COL; //Nombre de colonnes du labyrinthe

char* Path_Lab = "./Data/Maze/maze"; //Emplacement du labyrinthe

/* ======== Définitions des prototypes de fonctions ======== */

//Fonctions d'affichage
Sorties Refresh_Maze(char tab[nbr_Lignes][nbr_Colonnes], Point Pos_J1, Point Pos_J2, int Espacement);
void Refresh_Maze_Editeur(char tab[][65], Point Pos_Start, int nbr_Lignes, int nbr_Colonnes, int Espacement);
void Clear_Screen();
void Quadrillage(Point Start, Point End);

//Fonctions des menus
int Main_Menu(char tab[nbr_Lignes][nbr_Colonnes], Bouton Liste_Bouton[NBR_BTN]);
int Options(Bouton Liste_Bouton[NBR_BTN]);
int Editeur(Bouton Liste_Bouton[NBR_BTN]);

//Fonctions de déplacement
void Deplacement(char tab[nbr_Lignes][nbr_Colonnes], Point Pos_Temp, Joueur *Joueur, int Espacement, Direction Direction, int isJ1);
void Check_And_Change_Letter(char tab[nbr_Lignes][nbr_Colonnes], int isJ1, int Phase2, Joueur Joueur);
Tableau Get_Tab_Pos_By_Pos(char tab[][60], Point Pos, int Espacement);
void Affiche_mouvement(Joueur *J, int Espacement, Direction direction);

//Fonctions lors de la victoire
int Check_Win(Joueur J1, Joueur J2, int Force_Check);
int Win(int isJ1, Bouton Liste_Bouton[NBR_BTN], Joueur *J1, Joueur *J2);

//Fonctions gérants les boutons
int isButtonHit(Type_Bouton TypeBouton, Bouton Liste_Bouton[NBR_BTN], Point Pos_Clic);
void Afficher_Bouton(Type_Bouton TypeBouton, Bouton Liste_Bouton[NBR_BTN]);
void Init_Buttons(Bouton Liste_Bouton[NBR_BTN]);

//Fonctions gérant les tableaux
void Set_Char_Tab_By_Pos(char tab[][65], Point Pos_Start, Point Pos, char Char, int nbr_Lignes, int nbr_Colonnes, int Espacement);
char Get_Char_Tab_By_Pos(char tab[][65], Point Pos_Start, Point Pos, int nbr_Lignes, int nbr_Colonnes, int Espacement);
void Save_Tab_To_File(char Tab[][65], char *FileName, int nbr_Lignes, int nbr_Colonnes, int nbr_Sorties);

//Fonctions servant à enrengistrer le score
void Score_temps(Joueur *J, time_t depart, time_t arrive) ;// modifie le score en fonction du temps
int Enregistrer_Scores_Fichier(Joueur tout_score[]);
int Recup_Scores_Fichier(Joueur tout_score[]);
int Enregistrer_Score_Joueur(Joueur J, Joueur tout_score[]);
void Entrez_Noms_Joueur(Joueur *J1, Joueur *J2);

//Autres fonctions utiles
int min(int a, int b);
int mon_abs(int a);
char* Convert_To_String(int i);

/* ======== Main ======== */

int main(int argc, char *argv[]) //Fonction principale
{

	int MainStatut = 0; //Le statut que l'on veut pour la boucle principale (Rejouer/Menu/Quitter)
	
	#if BUILD_AUDIO
	if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024) == -1) //Initialisation de SDL Mixer pour les musiques/son
		printf("%s", Mix_GetError()); //Si il y a une erreur d'initialisation
	#endif
	
	while (MainStatut != -1) //Tant que MainStatus != -1 on recommence le programme
	{
		/* ======== Initialisation des variables ======== */

		 //Tableau contentant le labyrinthe
		 
		char tab[nbr_Lignes][nbr_Colonnes]; //Tableau contentant le labyrinthe
		
		Bouton Liste_Boutons[NBR_BTN]; //Tableau contenant les différents boutons des différents menus
		int col, lig; 
		int isJ1Win = 0;
		
		//Variables de la boucle de mouvement
		
		int Touche_Press = 1;
		
		Point Pos_Temp = {0, 0}, Pos_Score = {175, 840}, Pos_Score_Texte = {348, 892};
		
		//Variables des deux joueurs
		
		Joueur J1 = {" ", 0, {0, 0}, {0, 0}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}, yellow, 0}, J2 = {" ", 0, {0, 0} ,{0, 0}, {{0, 0}, {0, 0}, {0, 0}, {0, 0}}, yellow, 0};
		
		//Variables gérant le chronomètre
		
		time_t Depart, Arrive;
		time_t Debut_Chrono = time(NULL);
		time(&Depart); //Début du jeu
		float Limite = 120.0f; //Limite de temps de jeu : 120 secondes / 2 minutes
		
		
		if (MainStatut == 0) //Si on ne veut pas rejouer et qu'on retourne au menu
		{
			ouvrir_fenetre(FEN_X, FEN_Y);
			
			Set_Window_Title("Ultimate Maze Runner"); //On change le titre de la fenêtre
			
			Init_Buttons(Liste_Boutons); //On initialise les boutons
			
			/* ======== Menu principal ======== */
			
			if (!Main_Menu(tab, Liste_Boutons)) //On lance le menu principal
				return 0; //Si il renvoie 0 cela veut dire que l'utilisateur veut quitter
		}

		Clear_Screen(); //On efface l'écran pour faire place au jeu
		
		actualiser();
		
		/* ======== Initialisation de la position des joueurs ======== */
		
		do
		{
		 	
			col = entier_aleatoire(nbr_Colonnes); //Emplacement verticale
			lig = entier_aleatoire(nbr_Lignes); //Emplacement horizontale
			
			while (tab[lig][col] == '*' || tab[lig][col] == 'S') //Tant que l'on est sur un mur où une sortie
			{
				col = entier_aleatoire(nbr_Colonnes);
				lig = entier_aleatoire(nbr_Lignes);
			}
			
			tab[lig][col]= 'T';
		
			J1.Pos.x = col * Espacement;
			J1.Pos.y = lig * Espacement;
			
			J2.Pos.x = J1.Pos.x + 600;
			J2.Pos.y = J1.Pos.y;
		 	
		} while ((J1.Pos.x > (nbr_Lignes * Espacement) + 2) || (J1.Pos.y > (nbr_Colonnes * Espacement) + 2)); //Tant que la position des joueurs dépasse celle maximum
		
		/* On configure la position des 2 joueurs ainsi que celle de leur sortie */
		
		J1.Pos_Tab.Colonne = col;
		J1.Pos_Tab.Ligne = lig;
		 
		J2.Pos_Tab.Colonne = col;
		J2.Pos_Tab.Ligne = lig;
		
		J2.Sortie = Refresh_Maze(tab, J1.Pos, J2.Pos, Espacement);
		
		J1.Sortie.Pos1 = J2.Sortie.Pos1;
		J1.Sortie.Pos2 = J2.Sortie.Pos2;
		
		J1.Sortie.Pos_Tab1 = J2.Sortie.Pos_Tab1;
		J1.Sortie.Pos_Tab2 = J2.Sortie.Pos_Tab2;
		
		J2.Sortie.Pos1.x += 600;
		J2.Sortie.Pos2.x += 600;
		
		/* ======== Création de l'interface de jeu ======== */
		
		afficher_image("./Data/Pictures/J1_Score.bmp", Pos_Score);
		
		Pos_Score.x += 600;
		
		afficher_image("./Data/Pictures/J2_Score.bmp", Pos_Score);

		actualiser();
		
		/* ======== Boucle principale de jeu tant qu'un joueur n'a pas atteint une sortie ======== */
		
		#if BUILD_AUDIO
		Mix_HaltMusic(); //On arrête les musiques précédentes
		
		Mix_PlayMusic(Musique, -1); //On joue la musique du jeu
		#endif
		
		while (!Check_Win(J1, J2, 0) && (time(NULL) - Debut_Chrono) < Limite) //Tant qu'un joueur n'a pas gagné où que le chronomètre n'est pas terminer
		{
			
			traiter_evenements(); //On enregistre les évènements
			
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
			
			if (touche_a_ete_pressee(SDLK_ESCAPE)) //On quitte le jeu
			{
				return 0;
			}
			
			#if DEBUG
			if (touche_a_ete_pressee(SDLK_F1)) //Touche de debug pour faire gagner le joueur 1
			{
				
				J1.Pos.x = J1.Sortie.Pos1.x;
				J1.Pos.y = J1.Sortie.Pos1.y;
				
				J1.Pos_Tab = J1.Sortie.Pos_Tab1;
			}
			
			if (touche_a_ete_pressee(SDLK_F2)) //Touche de debug pour faire gagner le joueur 2
			{
			
				J2.Pos.x = J2.Sortie.Pos1.x;
				J2.Pos.y = J2.Sortie.Pos1.y;
				
				J2.Pos_Tab = J2.Sortie.Pos_Tab1;
			}
			#endif
			
			reinitialiser_evenements(); //On réinitialise les évènements enregistrés
			
			if (Touche_Press) // Si une touche à été préssée on refresh l'interface afin de réduire le lag
			{			
				/* ======== Affichage du score ======== */
				
				if (J1.Score >= 1000) //On décale le texte en fonction du nbr de chiffres
					Pos_Score_Texte.x -= 5;
				
				dessiner_rectangle(Pos_Score_Texte, 51, 51, black); //On recouvre l'ancien texte
				
				afficher_texte(Convert_To_String(J1.Score), 20, Pos_Score_Texte, white); //On affiche le nouveau score
				
				if (J2.Score >= 1000) //Pareil que pour le joueur 1
				{
					if (J1.Score <= 1000)
						Pos_Score_Texte.x -= 5;
				}
				
				Pos_Score_Texte.x += 600; //On change de coord pour le J2
				
				dessiner_rectangle(Pos_Score_Texte, 51, 51, black); //On recouvre l'ancien texte du J2
				
				afficher_texte(Convert_To_String(J2.Score), 20, Pos_Score_Texte, white); //On affiche le nouveau score du J2
				
				Pos_Score_Texte.x -= 600; //On remet les coord de base
				
				if (J1.Score >= 1000 || J2.Score >= 1000)
					Pos_Score_Texte.x += 5;
				
				Refresh_Maze(tab, J1.Pos, J2.Pos, Espacement); //On met à jour le lab
			
				actualiser(); //On actualise le tout
			
			}
			
			Touche_Press = 0; //On réinitialise la variable
			
		}
		
		isJ1Win = Check_Win(J1, J2, 1); //On regarde si un joueur à atteint la sortie
		
		if (isJ1Win == 1) //On remet la sortie dans la tableau
		{
			tab[J1.Pos_Tab.Ligne][J1.Pos_Tab.Colonne] = 'S';
			time(&Arrive); //Fin du jeu, lorsqu'un est joueur est arrivé à la sortie
			Score_temps(&J1, Depart, Arrive); // modifie le score du joueur gagnant, en fonction du temps
		}
		else if(isJ1Win == 2)
		{
			tab[J2.Pos_Tab.Ligne][J2.Pos_Tab.Colonne] = 'S';
			time(&Arrive) ; //Fin du jeu, lorsqu'un est joueur est arrivé à la sortie
			Score_temps(&J2, Depart, Arrive); // modifie le score du joueur gagnant, en fonction du temps
		}
		
		switch (Win(isJ1Win, Liste_Boutons, &J1, &J2)) //On regarde quel bouton à été appuyé
		{
			case 3: //Rejouer
				MainStatut = 1;
				break;
			case 4: //Menu
				MainStatut = 0;
				break;
			case 5: //On quitte le jeu
				MainStatut = -1;
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

int Main_Menu(char tab[nbr_Lignes][nbr_Colonnes], Bouton Liste_Bouton[NBR_BTN]) //Affiche le menu principal
{

	system("clear"); //On efface la console
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
		
		afficher_image("./Data/Pictures/Menu.bmp", Pos_Menu); //On affiche le menu principal
		 
		actualiser();
		
		while (Result_Button_Hit == -1) //On regarde tant qu'un joueur n'a pas cliqué sur un bouton
		{
			Pos_Clic = attendre_clic();
			Result_Button_Hit = isButtonHit(MENU_PRINCIPAL, Liste_Bouton, Pos_Clic);
		}
		
		switch (Result_Button_Hit) //On regarde quel bouton à été cliqué
		{
			case 0: //Quitter
				return 0;
				break;
			case 1: //Jouer
			
				Taille_Tab = charge_labyrinthe(Path_Lab, tab);
			
				nbr_Colonnes = Taille_Tab.nbr_Colonnes;
				nbr_Lignes = Taille_Tab.nbr_Lignes;
				
				return 1;
				
				break;
				
			case 2: //Options
				Options(Liste_Bouton);
				break;
		}
		
		Result_Button_Hit = -1;
	}
}

int Options(Bouton Liste_Bouton[NBR_BTN]) //Menu des options
{
	int Result_Button_Hit = -1;
	Point Pos_Clic = {-1, -1};
	
	while (1)
	{
		Clear_Screen(); //On efface l'écran
		
		Afficher_Bouton(OPTIONS, Liste_Bouton); //On affiche les boutons des options
		
		actualiser();
		
		while (Result_Button_Hit == -1) //On regarde tant qu'un joueur n'a pas cliqué sur un bouton
		{
			Pos_Clic = attendre_clic();
			Result_Button_Hit = isButtonHit(OPTIONS, Liste_Bouton, Pos_Clic);
		}
		
		switch (Result_Button_Hit) //On regarde quel bouton à été cliqué
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

int Editeur(Bouton Liste_Bouton[NBR_BTN]) //Editeur graphique de labyrinthe
{
	int Result_Button_Hit = -1, Largeur = 32, Hauteur = 64, c, l;
	
	Point Pos_Clic = {-1, -1};
	
	Point Pos_Texte_HL = {59, 108}, Pos_Texte_Explication = {25, 300};
	Point LigneH_P1 = {0, 0}, LigneH_P2 = {0, 0}, LigneL = {LigneH_P2.x + (Largeur * Espacement), LigneH_P1.y};
	
	char Texte_Variable[10] = ""; //Contient la longueur ou la largeur
	sprintf(Texte_Variable, "%c", Largeur);
	
	int isRightClick = 0, nbr_Sorties = 0;
	
	char Tab[65][65] = {{' '}}; //Tableau qui contiendra le labyrinthe
	
	//On initialise le tableau 
	
	for (c=0; c<65;c++)
	{
		for (l=0; l<65; l++)
			Tab[c][l] = ' ';
		
	}
	
	while (1)
	{
		Clear_Screen();
		
		/* ======== Affichage de l'interface (Boutons) ======== */
		
		Afficher_Bouton(EDITEUR, Liste_Bouton); //On affiche les boutons
		
		//Texte d'explication fixe
		
		afficher_texte("Clic gauche : Mur", 20, Pos_Texte_Explication, white);
		
		Pos_Texte_Explication.y += 100;
		
		afficher_texte("Clic gauche sur un ", 20, Pos_Texte_Explication, white);
		
		Pos_Texte_Explication.y += 20;
		
		afficher_texte("mur : Sortie", 20, Pos_Texte_Explication, white);
		
		Pos_Texte_Explication.y += 100;
		
		afficher_texte("Clic droit : ", 20, Pos_Texte_Explication, white);
		
		Pos_Texte_Explication.y += 20;
		
		afficher_texte("Revenir en arrière", 20, Pos_Texte_Explication, white);
		
		Pos_Texte_Explication.y -= 240;
		
		//Texte Largeur et Hauteur
		 
		afficher_texte("Largeur : ",  20, Pos_Texte_HL, white); //On affiche : "Largeur : "
		
		Pos_Texte_HL.x += 100; //On se place au bon endroit pour le texte variable
		
		strcpy(Texte_Variable, Convert_To_String(Largeur * 2));
		
		afficher_texte(Texte_Variable,  20, Pos_Texte_HL, white); //On affiche la largeur actuelle
		
		Pos_Texte_HL.x -= 100; //On remet comme avant
		
		strcpy(Texte_Variable, Convert_To_String(Hauteur)); //On rechange le texte variable pour afficher la hauteur
		
		Pos_Texte_HL.y += 50; //On se place au bon endroit pour le second texte fixe
		
		afficher_texte("Hauteur : ", 20, Pos_Texte_HL, white); //On affiche "Hauteur : "
		
		Pos_Texte_HL.x += 100; //On se place au bon endroit pour le second texte variable
		
		afficher_texte(Texte_Variable,  20, Pos_Texte_HL, white); //On affiche la hauteur actuelle
		
		Pos_Texte_HL.x -= 100; //On se replace au bon endroit pour le premier texte fixe
		Pos_Texte_HL.y -= 50;
		
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
		
		while (Result_Button_Hit == -1) //On regarde tant qu'un joueur n'a pas cliqué sur un bouton
		{
			Pos_Clic = attendre_clic_gauche_droite();
			
			if (Pos_Clic.x <= 0) //On regarde si le clic est un clic droit
				isRightClick = 1;
			
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
				if (Largeur * 2 < 64)
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
				Save_Tab_To_File(Tab, "./Data/Maze/Custom_Maze", Hauteur, Largeur * 2, nbr_Sorties);
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
						Set_Char_Tab_By_Pos(Tab, Liste_Bouton[15].Pos_HautG, Pos_Clic, '*', Hauteur, Largeur * 2, Espacement);
						nbr_Sorties -= 1;
					}
					else //Sinon on met du vide
						Set_Char_Tab_By_Pos(Tab, Liste_Bouton[15].Pos_HautG, Pos_Clic, ' ', Hauteur, Largeur * 2, Espacement);
				}
				else //Si c'est un clic gauche
				{
					if (Char_Tab == '*') //Si le caractère est un mur alors on met une sortie
					{
						Set_Char_Tab_By_Pos(Tab, Liste_Bouton[15].Pos_HautG, Pos_Clic, 'S', Hauteur, Largeur * 2, Espacement);
						nbr_Sorties += 1;
					}
					else if (Char_Tab != 'S' && Char_Tab != '*') //Sinon on met un mur
						Set_Char_Tab_By_Pos(Tab, Liste_Bouton[15].Pos_HautG, Pos_Clic, '*', Hauteur, Largeur * 2, Espacement);
				
				}
				
				break;
		}
		
		//On remet les variables à leur état initial
		isRightClick = 0;
		Result_Button_Hit = -1;
		Pos_Clic.x = -1;
	}
	
}

int Check_Win(Joueur J1, Joueur J2, int Force_Check) //Regarde quel joueur a gagné / Retourne 1 si le joueur 1 a gagné, 2 si le joueur 2 a gagné et 3 si les joueurs ont perdu au chronomètre
{
	if ( (((J1.Pos.x != J1.Sortie.Pos1.x) || (J1.Pos.y != J1.Sortie.Pos1.y)) && ((J2.Pos.x != J2.Sortie.Pos1.x) || (J2.Pos.y != J2.Sortie.Pos1.y))) && (((J1.Pos.x != J1.Sortie.Pos2.x) || (J1.Pos.y != J1.Sortie.Pos2.y)) && ((J2.Pos.x != J2.Sortie.Pos2.x) || (J2.Pos.y != J2.Sortie.Pos2.y))) && Force_Check == 0) //Si aucun des joueurs n'est à la sortie
		return 0;
	else	
	{
		if (((J1.Pos.x == J1.Sortie.Pos1.x) && (J1.Pos.y == J1.Sortie.Pos1.y)) || ((J1.Pos.x == J1.Sortie.Pos2.x) && (J1.Pos.y == J1.Sortie.Pos2.y))) //Si le joueur 1 est à la sortie
			return 1;
		else if (((J2.Pos.x == J2.Sortie.Pos1.x) && (J2.Pos.y == J2.Sortie.Pos1.y)) || ((J2.Pos.x == J2.Sortie.Pos2.x) && (J2.Pos.y == J2.Sortie.Pos2.y))) //Si le joueur 2 est à la sortie
			return 2;
		else //Sinon les joueurs ont perdu au chronomètre
			return 3;
	}
}


int Win(int playerWin, Bouton Liste_Bouton[NBR_BTN], Joueur *J1, Joueur *J2) //Affiche la victoire d'un des deux joueurs
{

	int Result_Button_Hit = -1, i;
	
	Point Pos_Clic = {-1, -1}, Pos_Picture = {310, 100};

	if (playerWin == 1) //Si le joueur 1 a gagné
		afficher_image("./Data/Pictures/GagneJ1.bmp", Pos_Picture);
	else if (playerWin == 2) //Si le joueur 2 a gagné
		afficher_image("./Data/Pictures/GagneJ2.bmp", Pos_Picture);
	else if (playerWin == 3) //Si aucun des 2 n'a gagné à cause du chronomètre
		afficher_image("./Data/Pictures/Egalite.bmp", Pos_Picture);
	
	actualiser(); //On affiche les images
	
	Joueur tout_score[9]; //Tableau dans lequel sera stocké tous les scores (seulement 9)
	
	Entrez_Noms_Joueur(J1, J2); //On demande les noms des joueurs
	
	for (i = 0; i < 9; i++) //On initialise les scores
	{
		tout_score[i].Score = 0;
		tout_score[i].Nom[0] = '\0';
	}
	
	Recup_Scores_Fichier(tout_score); //On récupère les scores du fichier Scores.txt
	
	Enregistrer_Score_Joueur(*J1, tout_score); //On ajoute le score du joueur à la liste des scores, si le score est assez grand
	
	Enregistrer_Scores_Fichier(tout_score); //On enregistre tous les scores dans un fichier_scor
	
	while (Result_Button_Hit == -1) //Tant que l'utilisateur n'a pas cliqué sur un bouton
	{
		Pos_Clic = attendre_clic(); //On attend un clic
		Result_Button_Hit = isButtonHit(VICTOIRE, Liste_Bouton, Pos_Clic); //On regarde si l'utilisateur à cliqué sur un bouton
	}
	
	switch (Result_Button_Hit) //On prend les actions nécessaire en fonction du bouton cliqué
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


void Deplacement(char tab[nbr_Lignes][nbr_Colonnes], Point Pos_Temp, Joueur *Joueur, int Espacement, Direction Direction, int isJ1) //Déplace le joueur dans le tableau du labyrinthe si il n'y pas de collision avec un mur
{
	
	Tableau Tableau_Temp = {0, 0};
	char Char_Temp = ' ';
	
	Tableau_Temp = Get_Tab_Pos_By_Pos(tab, Pos_Temp, Espacement); //On regarde la où le joueur veut aller

	Char_Temp = tab[Tableau_Temp.Ligne][Tableau_Temp.Colonne]; //On regarde le caractère associé à la position où le joueur veut aller

	if (Char_Temp != '*') //S'il ne s'agit pas d'un mur alors on déplace le joueur
	{
		//On affiche l'animation de mouvement + on bouge la position graphique du joueur
		if (Direction == HAUT)
			Affiche_mouvement(Joueur, Espacement, HAUT);
		else if (Direction == BAS)
			Affiche_mouvement(Joueur, Espacement, BAS);
		else if (Direction == GAUCHE)
			Affiche_mouvement(Joueur, Espacement, GAUCHE);
		else if (Direction == DROITE)
			Affiche_mouvement(Joueur, Espacement, DROITE);
			
		//On enlève le joueur de son ancienne case
		if (isJ1) 
			Check_And_Change_Letter(tab, 1, 0, *Joueur);
		else
			Check_And_Change_Letter(tab, 0, 0, *Joueur);
			
		Joueur -> Pos_Tab = Tableau_Temp; //On change la postion dans le tableau du joueur
		
		//On ajoute le joueur dans sa nouvelle case
		if (isJ1)
			Check_And_Change_Letter(tab, 1, 1, *Joueur);
		else
			Check_And_Change_Letter(tab, 0, 1, *Joueur);
			
	}
}

void Check_And_Change_Letter(char tab[nbr_Lignes][nbr_Colonnes], int isJ1, int Phase2, Joueur Joueur) //On regarde s'il n'y pas collision entre les 2 joueurs dans le tableau du lab + changement dans le tableau dans l'ancienne et la nouvelle case
{
	/* J = Joueur 1 / K = Joueur 2 / T = Les 2 joueurs */
		
	if (isJ1 && !Phase2)
	{
		 if (tab[Joueur.Pos_Tab.Ligne][Joueur.Pos_Tab.Colonne] == 'T') //Si les joueurs sont 2 sur la même case et que le joueur 1 s'est déplacé
			tab[Joueur.Pos_Tab.Ligne][Joueur.Pos_Tab.Colonne] = 'K'; //Alors on laisse le joueur 2 à l'ancienne position du joueur 1
		 else //Sinon on met un espace à l'ancienne position pour signifié que le joueur 1 n'est plus à cette case
			tab[Joueur.Pos_Tab.Ligne][Joueur.Pos_Tab.Colonne] = ' ';
	}
	else if (isJ1 && Phase2) //Si il s'agit du joueur 1 on change dans le tableau la case d'arrivée
	{
		if (tab[Joueur.Pos_Tab.Ligne][Joueur.Pos_Tab.Colonne] == 'K') //Si le joueur 2 est déjà présent sur la même case
			tab[Joueur.Pos_Tab.Ligne][Joueur.Pos_Tab.Colonne] = 'T'; //On change en T (two) pour signifier qu'ils sont tous les 2 sur la même case
		else
			tab[Joueur.Pos_Tab.Ligne][Joueur.Pos_Tab.Colonne] = 'J'; //Sinon on met le joueur 1 seul
	}
	else if (!isJ1 && !Phase2) //On change l'ancienne case du joueur 2 dans le même modèle qu'avec le joueur 1
	{
		if (tab[Joueur.Pos_Tab.Ligne][Joueur.Pos_Tab.Colonne] == 'T')
			tab[Joueur.Pos_Tab.Ligne][Joueur.Pos_Tab.Colonne] = 'J';
		else
			tab[Joueur.Pos_Tab.Ligne][Joueur.Pos_Tab.Colonne] = ' ';
	}
	else //Sinon on change la nouvelle case du joueur 2 dans le même modèle qu'avec le joueur 1
	{
		if (tab[Joueur.Pos_Tab.Ligne][Joueur.Pos_Tab.Colonne] == 'J')
			tab[Joueur.Pos_Tab.Ligne][Joueur.Pos_Tab.Colonne] = 'T';
		else
			tab[Joueur.Pos_Tab.Ligne][Joueur.Pos_Tab.Colonne] = 'K';
	}
}

Sorties Refresh_Maze(char tab[nbr_Lignes][nbr_Colonnes], Point Pos_J1, Point Pos_J2, int Espacement) //Raffraichit le labyrinthe pour les 2 joueurs. Renvoie les sorties trouvées dans le labyrinthe
{

	int i, l, c, nbr_Sorties = 0;
	Point Coin = {0, 0};
	Sorties Sorties_R = {{0, 0}, {0, 0}, {0, 0}, {0, 0}};
	
	for(i=0; i < 2; i++) //Pour les 2 joueurs
	{
		for(l=0; l!=nbr_Lignes; l++)
		{
			for(c=0; c!=nbr_Colonnes; c++)
			{
		
				if (tab[l][c] == '*') //Si on trouve une * il s'agit d'un mur et on affiche l'image de brique
					afficher_image("./Data/Pictures/Brique.bmp", Coin);
				
				else if (tab[l][c] == 'J' || tab[l][c] == 'T' || tab[l][c] == 'K') //S'il s'agit des joueurs
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
				
				else if (tab[l][c] == 'S') //S'il s'agit d'une sortie
				{
					afficher_image("./Data/Pictures/Sortie.bmp", Coin);
					
					if (nbr_Sorties == 0) //On définie la sortie 1
					{
						Sorties_R.Pos1.x = Coin.x;
						Sorties_R.Pos1.y = Coin.y;
						Sorties_R.Pos_Tab1.Ligne = l;
						Sorties_R.Pos_Tab1.Colonne = c;
					}
					else if (nbr_Sorties == 1) //On définie la sortie 2
					{
						Sorties_R.Pos2.x = Coin.x;
						Sorties_R.Pos2.y = Coin.y;
						Sorties_R.Pos_Tab2.Ligne = l;
						Sorties_R.Pos_Tab2.Colonne = c;
					}
					
					nbr_Sorties += 1;
				}
				else if (tab[l][c] == ' ') //S'il s'agit d'un espace
					dessiner_rectangle(Coin, Espacement, Espacement, noir);
					
				Coin.x += Espacement;
					
			}
			
			if (i == 0) //On change de coordonnées en fonction du joueur
				Coin.x = 0;
			else
				Coin.x = 600;
				
			Coin.y += Espacement;
		}
		
		Coin.y = 0;
		Coin.x = 600;
	}
	
	return Sorties_R; //On retourne les différentes sorties
	
}

void Refresh_Maze_Editeur(char tab[65][65], Point Pos_Start, int nbr_Lignes, int nbr_Colonnes, int Espacement) //Raffraichit l'éditeur de labyrinthe
{
	int l, c;
	Point Coin = {Pos_Start.x, Pos_Start.y};

	for(l=0; l!=nbr_Lignes; l++)
	{
		for(c=0; c!=nbr_Colonnes; c++)
		{
			if (tab[l][c] == '*')
				afficher_image("./Data/Pictures/Brique.bmp", Coin);
			else if (tab[l][c] == 'S')
				afficher_image("./Data/Pictures/Sortie.bmp", Coin);
			else if (tab[l][c] == ' ')
				dessiner_rectangle(Coin, Espacement, Espacement, black);
			else //Debug
				dessiner_rectangle(Coin, Espacement, Espacement, red);
				
			Coin.y += Espacement;
				
		}
		
		Coin.y = Pos_Start.y;
		Coin.x += Espacement;
	}
}

void Save_Tab_To_File(char Tab[65][65], char *FileName, int nbr_Lignes, int nbr_Colonnes, int nbr_Sorties) //On sauvegarde un tableau dans un fichier
{
	int l, c;
	FILE* fichier = NULL; //Pointeur du fichier
	
	fichier = fopen(FileName, "w"); //On ouvre le fichier en écriture
	
	if (fichier != NULL)
	{
		fprintf(fichier, "%d %d %d\n", nbr_Lignes, nbr_Colonnes, nbr_Sorties); //On écrit le nombre de lignes et de colonnes en haut de fichier
		
		for(l=0; l!=nbr_Lignes; l++)
		{
			for(c=0; c!=nbr_Colonnes; c++)
			{
				if (Tab[l][c] == ' ' || Tab[l][c] == '*' || Tab[l][c] == 'S') //S'il s'agit d'un caractère valide on l'écrit directement
					fputc(Tab[l][c], fichier);
				else //Sinon si problème (ne devrait pas arriver) on écrit un espace
					fputc(' ', fichier);
			}
			
			fputc('\n', fichier); //On met un saut de ligne à chaque fin de colonnes
		}
		
		fclose(fichier); //On ferme le fichier
	}
}

Tableau Get_Tab_Pos_By_Pos(char tab[][60], Point Pos, int Espacement) //Renvoie une coordonnée sur le tableau en fonction d'un point graphique
{
	int i, l, c;
	Point Coin = {0, 0};
	Tableau Pos_J = {0, 0};
	
	for(i=0; i < 2; i++) //On regarde pour les 2 joueurs
	{
		for(l=0; l!=nbr_Lignes; l++)
		{
			for(c=0; c!=nbr_Colonnes; c++)
			{
				if (Coin.x == Pos.x && Coin.y == Pos.y) //Si la position correspond
				{
					Pos_J.Ligne = l;
					Pos_J.Colonne = c;
					return Pos_J; //On retourne les coordonnees trouvées
				}
				
				Coin.x += Espacement;
			}
			
			if (i == 0)
				Coin.x = 0;
			else
				Coin.x = 600;
				
			Coin.y += Espacement;
		}
		
		//On change de tableau de joueur
		Coin.y = 0;
		Coin.x = 600;
	
	}
	
	return Pos_J; //On retourne les coordonnees trouvées
}

char Get_Char_Tab_By_Pos(char tab[65][65], Point Pos_Start, Point Pos, int nbr_Lignes, int nbr_Colonnes, int Espacement) //On obtient un caractère d'un tableau en fonction de la position graphique. Retourne un espace si rien n'est trouvé sinon renvoie le caractère correpondant
{
	int l, c;
	Point Coin = {Pos_Start.x, Pos_Start.y};
	
	for(l=0; l!=nbr_Lignes; l++)
	{
		for(c=0; c!=nbr_Colonnes; c++)
		{
			
			if (Coin.x + 2 == Pos.x && Coin.y == Pos.y) //Si la position correspond à celle graphique
				return tab[l][c]; //On retourne le caractère correpondant à la position graphique
			
			Coin.y += Espacement;
		}
		
		Coin.y = Pos_Start.y;
		Coin.x += Espacement;
	}
	
	return ' '; //Retourne un espace si rien n'est trouvé
	
}

void Set_Char_Tab_By_Pos(char tab[65][65], Point Pos_Start, Point Pos, char Char, int nbr_Lignes, int nbr_Colonnes, int Espacement) //On change le caractère d'un tableau en fonction de la position graphique
{
	int l, c;
	Point Coin = {Pos_Start.x, Pos_Start.y};
	
	for(l=0; l!=nbr_Colonnes; l++)
	{
		for(c=0; c!=nbr_Lignes; c++)
		{
			
			if (Coin.x + 2 == Pos.x && Coin.y == Pos.y) //Si la position correspond à celle graphique
			{
				tab[l][c] = Char; //On change le caractère
				return; //On quitte la fonction pour arrêter de boucler inutilement
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
	
	for (i=0; i<NBR_BTN; i++) //On regarde tout les boutons
	{
		if (Liste_Bouton[i].Type_Bouton == TypeBouton) //On regarde s'il s'agit d'un bouton du bon menu
		{
			if (Liste_Bouton[i].Image[0] != '\0') //Si une image est définie
			{	
				afficher_image(Liste_Bouton[i].Image, Liste_Bouton[i].Pos_HautG); //Alors on l'affiche
			}
			else //Sinon
			{
				Taille_Texte = taille_texte(Liste_Bouton[i].Texte, 30);
				dessiner_rectangle(Liste_Bouton[i].Pos_HautG, Taille_Texte.x, Taille_Texte.y, white); //On dessine un rectangle de la taille du texte
				if (Liste_Bouton[i].Texte[0] != '\0') //Si un texte est défini
					afficher_texte(Liste_Bouton[i].Texte, 30, Liste_Bouton[i].Pos_HautG, black);
			}
		}
	}
	
}

int isButtonHit(Type_Bouton TypeBouton, Bouton Liste_Bouton[NBR_BTN], Point Pos_Clic) //On regarde si l'utilisateur à cliqué sur un bouton. Retourne -1 si aucun bouton n'a été touché sinon retourne l'indice du bouton cliqué
{
	int i;
	Point Taille_Texte = {0, 0};
	
	for (i=0; i<NBR_BTN; i++) //On regarde tout les boutons
	{
		if (Liste_Bouton[i].Type_Bouton == TypeBouton) //On regarde s'il s'agit d'un bouton du bon menu
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

void Quadrillage(Point Start, Point End) //Dessine un quadrillage sur l'écran en fonction d'une coordonnée de départ et de fin
{
	Point Pos_P1 = {Start.x, Start.y}, Pos_P2 = {Start.x, End.y};
	
	while(Pos_P1.x <= End.x) //On dessine les colonnes
	{
		dessiner_ligne(Pos_P1, Pos_P2, white);
		Pos_P1.x += Espacement;
		Pos_P2.x += Espacement;
	}
	
	Pos_P1.x = Start.x;
	Pos_P1.y = Start.y;
	
	Pos_P2.x = End.x;
	Pos_P2.y = Start.y;
	
	while (Pos_P1.y <= End.y) //On dessine les lignes
	{
		dessiner_ligne(Pos_P1, Pos_P2, white);
		Pos_P1.y += Espacement;
		Pos_P2.y += Espacement;
	}
}

void Init_Buttons(Bouton Liste_Bouton[NBR_BTN]) //Initialise les différents boutons du jeu
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
	Liste_Bouton[6].Pos_HautG.x = (FEN_X / 2) - 150;
	Liste_Bouton[6].Pos_HautG.y = 100;
	Liste_Bouton[6].Texte = "Labyrinthe : Défaut";
	Liste_Bouton[6].Image = "";
	
	Liste_Bouton[7].Type_Bouton = OPTIONS;
	Liste_Bouton[7].Pos_HautG.x = (FEN_X / 2) - 150;
	Liste_Bouton[7].Pos_HautG.y = 300;
	Liste_Bouton[7].Texte = "Sauvegarder et quitter";
	Liste_Bouton[7].Image = "";
	
	Liste_Bouton[8].Type_Bouton = OPTIONS;
	Liste_Bouton[8].Pos_HautG.x = (FEN_X / 2) - 150;
	Liste_Bouton[8].Pos_HautG.y = 200;
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
	Liste_Bouton[14].Pos_HautG.x = 655;
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

void Score_temps(Joueur *J, time_t depart, time_t arrive) //Modifie le score d'un joueur en fonction du temps passé pour sortir du labyrinthe
{
	if (difftime(arrive, depart) < 50)
		J->Score += 50;
	else if (difftime(arrive, depart) < 75)
		J->Score += 40;
	else if (difftime(arrive, depart) < 90)
		J->Score += 30;
	else if (difftime(arrive, depart) < 100)
		J->Score += 20;
}

void Affiche_mouvement(Joueur *J, int Espacement, Direction direction) //Exécute une animation de déplacement lorsque qu'un joueur se déplace
{
	int compteur = 0;
	Couleur degrade = fabrique_couleur(100, 100, 0);//Nuance de jaune
	Point traine={0, 0};
	
	for ( ; compteur < Espacement ; compteur++)//Boucle qui rend le mouvement plus fluide, donne aussi une impression de lenteur
	{
		dessiner_rectangle(J->Pos, Espacement, Espacement, noir);
		
		if (direction == BAS)
		{
			J -> Pos.y++ ;
			traine.x = J -> Pos.x;
			traine.y = J -> Pos.y - 3;
		}
		else if (direction == HAUT)
		{
			J -> Pos.y-- ;
			traine.x = J -> Pos.x;
			traine.y = J -> Pos.y + Espacement;
		}
		else if (direction == DROITE)
		{
			J -> Pos.x++ ;
			traine.x = J -> Pos.x - 3;
			traine.y = J -> Pos.y;
		}
		else if (direction == GAUCHE)
		{
			J -> Pos.x-- ;
			traine.x = J -> Pos.x + Espacement;
			traine.y = J -> Pos.y;
		}
		
		dessiner_rectangle(J->Pos, Espacement, Espacement, J->Couleur);
		
		if (direction == DROITE || direction == GAUCHE)//Une fois déplacé, un carré est créé derrière le joueur
			dessiner_rectangle(traine, 3, Espacement, degrade);
		else if (direction == HAUT || direction == BAS)
			dessiner_rectangle(traine, Espacement, 3, degrade);
			
		actualiser();
	}
	
}

int Enregistrer_Scores_Fichier(Joueur tout_score[]) //Enrengistre le tableau des scores dans un fichier et retourne 1 si une erreur à eu lieu et 0 sinon
{
	
	FILE* fichier_scor = NULL;//Il s'agit d'un pointeur, donc on l'initialise pour éviter des erreurs
	
	fichier_scor = fopen("./Data/Scores/Scores.txt", "w+");//On ouvre le fichier ; w+ : écraser le contenu et écrire
	
	int rang = 0;
	
	if (fichier_scor != NULL)
	{
		for ( ; rang <= 8 ; rang++)// pour chaque ligne, on écrit dans le fichier le score
			fprintf(fichier_scor, "%d %s\n", tout_score[rang].Score, tout_score[rang].Nom);
			
		fclose(fichier_scor);// on ferme le fichier_score
	}
	else
	{
		printf("Erreur : Le fichier score n'a pas pu être trouvé !\n");
		return 1;
	}
		
	return 0;
}

int Recup_Scores_Fichier(Joueur tout_score[]) //Récupère les scores du fichier contenant les scores. Retourne 1 en cas d'erreur et 0 sinon
{
	FILE* fichier_scor = NULL; //Il s'agit d'un pointeur, donc on l'initialise pour éviter des erreurs
	
	int compteur = 0;
	
	fichier_scor = fopen("./Data/Scores/Scores.txt", "r"); //On ouvre le fichier / r : pour lire
	
	if (fichier_scor != NULL)
	{
		
		for ( ; compteur <= 8 ; compteur++)//Récupère les valeurs pour chaque lignes / RANG == LIGNE
			fscanf(fichier_scor, "%d %s\n", &tout_score[compteur].Score, tout_score[compteur].Nom);
		fclose(fichier_scor); //On ferme le fichier_score
	}
	else
	{
		printf("Le fichier score n'a pas pu être trouvé");
		return 1;
	}
		
	return 0 ;
	
}


int Enregistrer_Score_Joueur(Joueur J, Joueur tout_score[]) //Enrengistre le score dans le tableau des scores. Retourne 0 en cas de succès
{
	int rang = 0, fin_fichier = 8;
	
	for ( ; rang <= fin_fichier ; rang++)//On parcourt tout les scores
	{
		if (J.Score > tout_score[rang].Score)//On regarde si le score du joueur est supérieur à celui du joueur de la ligne lue
		{
			for ( ; fin_fichier > rang ; fin_fichier--)//Boucle qui sert à décaler les valeurs
			{
				tout_score[fin_fichier].Score = tout_score[fin_fichier - 1].Score;
				snprintf(tout_score[fin_fichier].Nom, 6, "%s", tout_score[fin_fichier - 1].Nom);
			}
			
			tout_score[rang].Score = J.Score;//Une fois les lignes décalées, on peut écrire les valeurs du joueur
			
			snprintf(tout_score[rang].Nom, 6, "%s", J.Nom);
			
			break;
			
		}
		
	}
	
	return 0;
	
}

void Entrez_Noms_Joueur(Joueur *J1, Joueur *J2) //Demande le nom des joueurs dans la console
{
	do
	{
		printf("Entrez le nom du joueur 1 (5 lettres max) : ");
		scanf("%s", J1 -> Nom);
		printf("Entrez le nom du joueur 2 (5 lettres max) : ");
		scanf("%s", J2 -> Nom); 
		
	} while (strlen(J1 -> Nom) >= 6 && strlen(J2 -> Nom) >= 6); //On regarde si la longueur des noms entrés n'est pas trop grand
	
}


int mon_abs(int a) //Retourne l'abscisse d'un entier
{
	if (a > 0)
		return a;
	else
		return -a;
}

int min(int a, int b) //Retourne le minimum entre 2 entiers
{
	if (a > b)
		return b;
	else if (a == b)
		return b;
	else
		return a;
}

void Clear_Screen() //Supprime tous les éléments de l'écran en dessinant un rectangle de la taille de la fenêtre
{
	Point Origin = {0, 0};
	dessiner_rectangle(Origin, FEN_X, FEN_Y, black);
}

char* Convert_To_String(int i) //Convertit un entier en char*
{
	int Longueur = snprintf(NULL, 0, "%d", i);
	char* str = malloc(Longueur + 1);
	snprintf(str, Longueur + 1, "%d", i);
	return str;
}
