#ifndef MAP_H
# define MAP_H

/*
** Enumeration des types de cellules dans le labyrinthe
*/
typedef enum s_cellType
{
	CELL_EMPTY = ' ',
	CELL_WALL = '#',
	CELL_PLAYER = '@',
	CELL_EXIT = 'E',
	CELL_POTION = 'P',
	CELL_KEY = 'K',
	CELL_TRESOR_CLOSED = 'C',
	CELL_TRESOR_EMPTY = 'T',
	CELL_MONSTER = 'M'
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
int			map_save_to_file(const t_map *map, const char *filename);
t_cellType	map_get_cell(const t_map *map, int x, int y);
void		map_set_cell(t_map *map, int x, int y, t_cellType cell);
void		map_set_player_position(t_map *map, int x, int y);
int			map_is_valid_position(const t_map *map, int x, int y);

/* Fonctions d'acces aux proprietes de la carte */
int			map_get_width(const t_map *map);
int			map_get_height(const t_map *map);
int			map_get_player_x(const t_map *map);
int			map_get_player_y(const t_map *map);

/* Generation / sauvegarde de labyrinthe (outil) */
# define TAILLE 20
# define GEN_CELL_SIZE 4

typedef struct s_direction
{
	char	direction;
}			t_Direction;

/* Generateur de labyrinthe (produit un fichier texte dans maps/)
 * Ces fonctions permettent de generer et sauvegarder un labyrinthe.
 */
void		map_init(t_Direction grille[TAILLE][TAILLE]);
void		generer(t_Direction grille[TAILLE][TAILLE]);
void		sauver_labyrinthe(t_Direction grille[TAILLE][TAILLE]);
void		creer_labyrinthe(void);

#endif
