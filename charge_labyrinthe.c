/******************************************************************************/
/* CHARGE_LABYRINTHE                                                          */
/******************************************************************************/
#include "./charge_labyrinthe.h"

// Charge dans tab le labyrinthe du fichier f

int lig, col;

SizeTab charge_labyrinthe(char *fic, char tab[LIG][COL])
{
    
    FILE *f = fopen(fic,"r"); // descripteur de fichier
    SizeTab Size;
    
    int l, c, nbrSorties;
    char car;

    //printf("charge_labyrinthe: démarrage\n");
    
    if(!f)
    {
    	printf("Fichier '%s': ouverture impossible\n",fic);
        exit(1);
    }

    fscanf(f,"%d %d %d\n",&lig,&col, &nbrSorties); // lecture des lignes et colonnes en en-tête
    
    if (nbrSorties > 2)
    	printf("Attention : Nombre de sorties supérieur à 2 ! Une ou plusieurs risques de ne pas fonctionner.");
    
    Size.nbr_Lignes = lig;
    Size.nbr_Colonnes = col;

    for(l=0;l!=lig;l++) // pour lig lignes
    {
        c=0; //position colonne
        while(1)
        {
            fscanf(f,"%c",&car); //lecture d'un caractère

            if(c>=col) // si on dépasse de la ligne
                break;  // fin du while, on change de ligne

            switch(car) // selon le caractère lu...
            {
                case ' ': case '*': case 'S': // si c'est un espace ou une étoile
                    tab[l][c]=car;  // je recopie tel quel dans le tableau
                case '\n': // si c'est un saut de ligne
                    break; // on quitte le while
                default:   // pour tous les autres caractères
                    printf("Fichier '%s': ligne %d caractère %d: caractère incorrect\n",fic,l+1,c+1);
                    exit(1); // fin du programme
             }

            c++; // colonne suivante
         }
    }
    
    fclose(f); // fermeture du descripteur de fichier
    //printf("charge_labyrinthe: terminé\n");
    
    return Size;
}

