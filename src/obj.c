#include "../lab.h"

t_obj   *create_obj(char name, int effect, t_position position)
{
	t_obj   *obj;

	obj = (t_obj *)malloc(sizeof(t_obj));
	obj->name = name;
	obj->effect = effect;
	obj->position.x = position.x;
	obj->position.y = position.y;
	return obj;
}

void    use_obj(t_player *player, t_obj obj, int max_pv)
{
	add_life(player, obj.effect, max_pv);
}