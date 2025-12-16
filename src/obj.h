#include "position.h"

#ifndef OBJ_H
# define OBJ_H

/*
les objects sont definis par des noms (potion P, trap T, ...) a definir
le plus simple sera de faire ca via des constantes ex
#define POTION P
ils ont tous des effects, positif, neutre ou negatif
/!\ l'utilisation d'object n'entraine pas sa destruction /!\
*/

typedef struct s_obj
{
	char		name;
	int			effect;
	t_position	position;
}				t_obj;

t_obj	*create_obj(char name, int effect, t_position position);
// Ajoute des PV (retire avec un negatif)
// Si max_pv est egale a -1 on considere qu'il n'y a pas de limite (max)
void    use_obj(t_player *player, t_obj obj);

#endif
