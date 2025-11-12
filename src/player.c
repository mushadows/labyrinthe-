#include "../lab.h"
#include <assert.h>

/*
** Cree une nouvelle structure de joueur
** Alloue la memoire et initialise les valeurs par defaut
** @return pointeur vers la structure t_player creee
*/
t_player	*create_player(void)
{
	t_player	*player;

	player = (t_player *)malloc(sizeof(t_player));
	assert(player != NULL);
	player->life = 1;
	player->position.x = 0;
	player->position.y = 0;
	return (player);
}

/*
** Ajoute ou retire des points de vie au joueur
** @param player: pointeur vers la structure du joueur
** @param pv: nombre de points de vie a ajouter (negatif pour retirer)
** @param max_pv: limite maximale de points de vie (-1 pour pas de limite)
*/
void	add_life(t_player *player, int pv, int max_pv)
{
	player->life += pv;
	if (max_pv != -1 && player->life > max_pv)
		player->life = max_pv;
}

/*
** Teleporte le joueur a une position donnee
** @param player: pointeur vers la structure du joueur
** @param position: nouvelle position du joueur
*/
void	tp_player(t_player *player, t_position position)
{
	player->position.x = position.x;
	player->position.y = position.y;
}

/*
** Deplace le joueur vers le haut (decrement Y)
** @param player: pointeur vers la structure du joueur
*/
void	player_move_up(t_player *player)
{
	player->position.y--;
}

/*
** Deplace le joueur vers le bas (increment Y)
** @param player: pointeur vers la structure du joueur
*/
void	player_move_down(t_player *player)
{
	player->position.y++;
}

/*
** Deplace le joueur vers la droite (increment X)
** @param player: pointeur vers la structure du joueur
*/
void	player_move_right(t_player *player)
{
	player->position.x++;
}

/*
** Deplace le joueur vers la gauche (decrement X)
** @param player: pointeur vers la structure du joueur
*/
void	player_move_left(t_player *player)
{
	player->position.x--;
}
