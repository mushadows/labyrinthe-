#include "position.h"

#ifndef PLAYER_H
# define PLAYER_H

/* Limite du joueur */
# define MAX_PV 100

typedef struct s_player
{
	int			life;
	int			max_life;
	t_position	position;
}				t_player;

t_player	*create_player(void);
// Ajoute des PV (retire avec un negatif)
// Si max_pv est egale a -1 on considere qu'il n'y a pas de limite (max)
void		add_life(t_player *player, int pv);
void		tp_player(t_player *player, t_position position);
// no doc, check le nom de fonction
void		player_move_up(t_player *player);
void		player_move_down(t_player *player);
void		player_move_right(t_player *player);
void		player_move_left(t_player *player);

#endif