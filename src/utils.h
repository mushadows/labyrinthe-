#ifndef MAP_H
# define MAP_H

/*
** Enumeration des types de cellules dans le labyrinthe
*/
typedef enum s_cellType
{
	CELL_EMPTY = ' ',
	CELL_WALL = '#',
	CELL_PLAYER = '@'
}			t_cellType;

/*
** Structure pour representer la carte du labyrinthe
** Contient la grille 2D et les informations de position
*/
typedef struct s_map
{
	char	**grid;
	int		width;
	int		height;
	int		player_x;
	int		player_y;
}			t_map;

/* Fonctions de gestion de la carte */
t_map		*map_create(void);
void		map_destroy(t_map *map);
int			map_load_from_file(t_map *map, const char *filename);
t_cellType	map_get_cell(const t_map *map, int x, int y);
void		map_set_cell(t_map *map, int x, int y, t_cellType cell);
void		map_set_player_position(t_map *map, int x, int y);
int			map_is_valid_position(const t_map *map, int x, int y);

/* Fonctions d'acces aux proprietes de la carte */
int			map_get_width(const t_map *map);
int			map_get_height(const t_map *map);
int			map_get_player_x(const t_map *map);
int			map_get_player_y(const t_map *map);

#endif
