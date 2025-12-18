#ifndef MONSTER_H
# define MONSTER_H

# include "position.h"

typedef struct s_monster
{
	t_position	position;
	int			active;
}	t_monster;

t_monster	*create_monster(int x, int y);
void		move_monster_random(t_monster *monster, const void *map);
void		destroy_monster(t_monster *monster);

#endif