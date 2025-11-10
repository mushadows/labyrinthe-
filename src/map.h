#ifndef LABY_H
#define LABY_H

#define TAILLE 20
#define CELL_SIZE 3


typedef struct s_enum {
    char direction;
} t_Direction;

void mapInit(t_Direction grille[TAILLE][TAILLE]);
void generer(t_Direction grille[TAILLE][TAILLE]);
void afficher(t_Direction grille[TAILLE][TAILLE]);

#endif
