#include "map.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


/*Fonction qui initialide toutes les directions du double tableau vers la droites 
sauf la colone de droite qui elle est initialise vers le bas 
init le noeud d'origine en bas a droite*/

void mapInit(t_Direction grille[TAILLE][TAILLE])
{
    for (int i = 0; i < TAILLE; i++)
    {
        for (int j = 0; j < TAILLE - 1; j++)
        {
            grille[i][j].direction = 'D';
        }
    }

    for (int i = 0; i < TAILLE - 1; i++)
    {
        grille[i][TAILLE-1].direction = 'B';
    }

    grille[TAILLE - 1][TAILLE - 1].direction = 'N';
}


/*Fonction qui change le point d'origine et sews directions correspondantes 
afin de garder un labyrithe parfait
labyrithe parfait = 1- tous les noeuds mene a l'origine
                    2- pas plus d'une direction par noeuds 
On arrete la boucle au bout de nb_iterations 
( juge suffisant pour avoir un labyrinthe 100% original)*/

void generer(t_Direction grille[TAILLE][TAILLE]) 
{
    srand(time(NULL));

    int origine_i = TAILLE - 1;
    int origine_j = TAILLE - 1;

    int nb_iterations = TAILLE * TAILLE * 10;

    for (int iter = 0; iter < nb_iterations; iter++) 
    {
        char voisins[4];
        int nb_voisins = 0;

        if (origine_i > 0) voisins[nb_voisins++] = 'H';
        if (origine_i < TAILLE - 1) voisins[nb_voisins++] = 'B';
        if (origine_j > 0) voisins[nb_voisins++] = 'G';
        if (origine_j < TAILLE - 1) voisins[nb_voisins++] = 'D';

        int index = rand() % nb_voisins;
        char voisin_choisi = voisins[index];

        grille[origine_i][origine_j].direction = voisin_choisi;

        switch (voisin_choisi) 
        {
            case 'H': origine_i--; break;
            case 'B': origine_i++; break;
            case 'G': origine_j--; break;
            case 'D': origine_j++; break;
        }

        grille[origine_i][origine_j].direction = 'N';
    }
}


/*Fonction qui ecris dans un fichier text le labyrinthe sous forme de mur :

    -Chaque noeuds represente une case de 3x3
    -On remplit ce tableau de cases de murs
    -Chaque case a une direction 
    -On enleve le mur centrale de chaque case 
    -On enelve les murs en fonction de la direction*/

void sauverLabyrinthe(t_Direction grille[TAILLE][TAILLE]) 
{
    int h = TAILLE * CELL_SIZE;
    int w = TAILLE * CELL_SIZE;
    char tab_final[TAILLE*CELL_SIZE][TAILLE*CELL_SIZE];

    for (int i = 0; i < h; i++)
        for (int j = 0; j < w; j++)
            tab_final[i][j] = '#';

    for (int i = 0; i < TAILLE; i++) {
        for (int j = 0; j < TAILLE; j++) 
        {
            int start_i = i * CELL_SIZE;
            int start_j = j * CELL_SIZE;

            tab_final[start_i + 1][start_j + 1] = ' ';

            switch(grille[i][j].direction) 
            {
                case 'H':
                    tab_final[start_i][start_j + 1] = ' ';
                    tab_final[start_i - 1][start_j + 1] = ' ';
                    break;
                case 'B':
                    tab_final[start_i + 2][start_j + 1] = ' ';
                    tab_final[start_i + 3][start_j + 1] = ' ';
                    break;
                case 'G':
                    tab_final[start_i + 1][start_j] = ' ';
                    tab_final[start_i + 1][start_j - 1] = ' ';
                    break;
                case 'D':
                    tab_final[start_i + 1][start_j + 2] = ' ';
                    tab_final[start_i + 1][start_j + 3] = ' ';
                    break;
                case 'N': break;
            }
        }
    }

    FILE* f = fopen("../maps/labyrinthe.txt", "w");
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++)
            fputc(tab_final[i][j], f);
        fputc('\n', f);
    }
    fclose(f);
}


/*Appel Global*/

int creerLabyrinthe() 
{
    t_Direction grille[TAILLE][TAILLE];
    mapInit(grille);
    generer(grille);
    sauverLabyrinthe(grille);
}