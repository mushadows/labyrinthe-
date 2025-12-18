#include "obj.h"
#include "map.h"

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

void spawn_all_objects(t_map *map)
{
    int x, y, i;
    int total_cases = map->width * map->height;
    
    int nb_potions = total_cases / 250;
    int nb_traps = total_cases / 20;
    int nb_chests = total_cases / 150;

    int placed = 0;
    while (placed == 0) {
        x = rand() % map->width;
        y = rand() % map->height;
        if (map_get_cell(map, x, y) == CELL_EMPTY) {
            map_set_cell(map, x, y, CELL_KEY);
            placed = 1;
        }
    }

    for (i = 0; i < nb_potions; i++) {
        x = rand() % map->width; y = rand() % map->height;
        if (map_get_cell(map, x, y) == CELL_EMPTY) map_set_cell(map, x, y, CELL_POTION);
    }

    for (i = 0; i < nb_traps; i++) {
        x = rand() % map->width; y = rand() % map->height;
        if (map_get_cell(map, x, y) == CELL_EMPTY) map_set_cell(map, x, y, CELL_TRAP);
    }

    for (i = 0; i < nb_chests; i++) {
        x = rand() % map->width; y = rand() % map->height;
        if (map_get_cell(map, x, y) == CELL_EMPTY) map_set_cell(map, x, y, CELL_TRESOR_CLOSED);
    }
}