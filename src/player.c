#include "../lab.h"

t_player	*create_player(void)
{
	t_player	*player;

	player = (t_player *)malloc(sizeof(t_player));
	player->life = 1;
	player->position.x = 0;
	player->position.y = 0;
	return (player);
}

void	add_life(t_player *player, int pv, int max_pv)
{
	player->life += pv;
	if (max_pv != -1 && player->life > max_pv)
		player->life = max_pv;
}

void	tp_player(t_player *player, t_position position)
{
	player->position.x = position.x;
	player->position.y = position.y;
}

void	player_move_up(t_player *player)
{
	player->position.y--;
}

void	player_move_down(t_player *player)
{
	player->position.y++;
}

void	player_move_right(t_player *player)
{
	player->position.x++;
}

void	player_move_left(t_player *player)
{
	player->position.x--;
}
