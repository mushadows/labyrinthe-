#include "monster.h"
#include "map.h"
#include <stdlib.h>
#include <assert.h>

t_monster	*create_monster(int x, int y)
{
	t_monster	*monster;

	monster = (t_monster *)malloc(sizeof(t_monster));
	assert(monster != NULL);
	monster->position.x = x;
	monster->position.y = y;
	monster->active = 1;
	return (monster);
}

void	move_monster_random(t_monster *monster, const void *map_ptr)
{
	const t_map	*map;
	int			direction;
	int			new_x;
	int			new_y;

	if (!monster || !monster->active || !map_ptr)
		return ;
	map = (const t_map *)map_ptr;
	direction = rand() % 4;
	new_x = monster->position.x;
	new_y = monster->position.y;
	if (direction == 0)
		new_y--;
	else if (direction == 1)
		new_y++;
	else if (direction == 2)
		new_x--;
	else if (direction == 3)
		new_x++;
	if (map_is_valid_position(map, new_x, new_y))
	{
		monster->position.x = new_x;
		monster->position.y = new_y;
	}
}

void	destroy_monster(t_monster *monster)
{
	if (monster)
		free(monster);
}