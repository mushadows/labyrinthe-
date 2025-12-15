#include "utils.h"
#include "map.h"
#include <stdio.h>
#include <stdlib.h>

/*
** Charge une texture depuis un fichier PNG
** @param renderer: le renderer SDL
** @param path: chemin vers le fichier image
** @return pointeur vers la texture SDL, NULL en cas d'erreur
*/
static SDL_Texture	*load_texture(SDL_Renderer *renderer, const char *path)
{
	SDL_Surface		*surface;
	SDL_Texture		*texture;

	surface = IMG_Load(path);
	if (!surface)
	{
		printf("Erreur chargement image %s: %s\n", path, IMG_GetError());
		return (NULL);
	}
	texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);
	if (!texture)
	{
		printf("Erreur création texture %s: %s\n", path, SDL_GetError());
		return (NULL);
	}
	return (texture);
}

/*
** Charge toutes les textures necessaires pour le jeu
** @param renderer: le renderer SDL pour creer les textures
** @return pointeur vers la structure de textures, NULL en cas d'erreur
*/
t_textures	*load_textures(SDL_Renderer *renderer)
{
	t_textures	*textures;

	if (!renderer)
		return (NULL);
	textures = (t_textures *)malloc(sizeof(t_textures));
	if (!textures)
		return (NULL);
	textures->wall_left = load_texture(renderer,
			"0x72_16x16DungeonTileset.v5/items/Wall_inner_e.png");
	textures->wall_right = load_texture(renderer,
			"0x72_16x16DungeonTileset.v5/items/Wall_inner_w.png");
	textures->wall_top = load_texture(renderer,
			"0x72_16x16DungeonTileset.v5/items/wall_top_center.png");
	textures->wall_corner_ne = load_texture(renderer,
			"0x72_16x16DungeonTileset.v5/items/Wall_inner_ne.png");
	textures->wall_corner_nw = load_texture(renderer,
			"0x72_16x16DungeonTileset.v5/items/Wall_inner_nw.png");
	textures->wall_center = load_texture(renderer,
			"0x72_16x16DungeonTileset.v5/items/wall_center.png");
	textures->floor = load_texture(renderer,
			"0x72_16x16DungeonTileset.v5/items/floor_plain.png");
	textures->exit = load_texture(renderer,
			"0x72_16x16DungeonTileset.v5/items/Floor_ladder.png");
	textures->player = load_texture(renderer,
			"0x72_16x16DungeonTileset.v5/items/hero_basic.png");
	if (!textures->wall_left || !textures->wall_right || !textures->wall_top
		|| !textures->wall_corner_ne || !textures->wall_corner_nw
		|| !textures->wall_center || !textures->floor || !textures->exit
		|| !textures->player)
	{
		free_textures(textures);
		return (NULL);
	}
	return (textures);
}

/*
** Libere toutes les textures chargees
** @param textures: pointeur vers la structure de textures a liberer
*/
void	free_textures(t_textures *textures)
{
	if (!textures)
		return ;
	if (textures->wall_left)
		SDL_DestroyTexture(textures->wall_left);
	if (textures->wall_right)
		SDL_DestroyTexture(textures->wall_right);
	if (textures->wall_top)
		SDL_DestroyTexture(textures->wall_top);
	if (textures->wall_corner_ne)
		SDL_DestroyTexture(textures->wall_corner_ne);
	if (textures->wall_corner_nw)
		SDL_DestroyTexture(textures->wall_corner_nw);
	if (textures->wall_center)
		SDL_DestroyTexture(textures->wall_center);
	if (textures->floor)
		SDL_DestroyTexture(textures->floor);
	if (textures->exit)
		SDL_DestroyTexture(textures->exit);
	free(textures);
}

/*
** Obtient la texture de mur appropriée selon les cellules voisines
** @param textures: pointeur vers la structure de textures
** @param map: pointeur vers la carte
** @param x: coordonnée X du mur
** @param y: coordonnée Y du mur
** @return la texture appropriée pour ce mur
*/
SDL_Texture	*get_wall_texture(t_textures *textures, const void *map_ptr, int x, int y)
{
	const t_map	*map;
	int			has_empty_left;
	int			has_empty_right;
	int			has_empty_top;
	int			has_empty_bottom;

	if (!textures || !map_ptr)
		return (textures ? textures->wall_center : NULL);
	map = (const t_map *)map_ptr;
	has_empty_left = (x > 0 && map_get_cell(map, x - 1, y) != CELL_WALL);
	has_empty_right = (map_get_cell(map, x + 1, y) != CELL_WALL);
	has_empty_top = (y > 0 && map_get_cell(map, x, y - 1) != CELL_WALL);
	has_empty_bottom = (map_get_cell(map, x, y + 1) != CELL_WALL);
	if (has_empty_left && !has_empty_right)
		return (textures->wall_right);
	else if (has_empty_right && !has_empty_left)
		return (textures->wall_left);
	else if (has_empty_top && !has_empty_bottom)
		return (textures->wall_center);
	return (textures->wall_center);
}

/*
** Obtient la texture de rebord pour un sol au-dessus d'un mur
** @param textures: pointeur vers la structure de textures
** @param map_ptr: pointeur vers la carte
** @param x: coordonnée X du sol
** @param y: coordonnée Y du sol
** @return la texture de rebord appropriée, ou NULL si pas de rebord
*/
SDL_Texture	*get_floor_top_edge(t_textures *textures, const void *map_ptr, int x, int y)
{
	const t_map	*map;
	int			has_wall_below;
	int			has_wall_left;
	int			has_wall_right;

	if (!textures || !map_ptr)
		return (NULL);
	map = (const t_map *)map_ptr;
	has_wall_below = (map_get_cell(map, x, y + 1) == CELL_WALL);
	if (!has_wall_below)
		return (NULL);
	has_wall_left = (x > 0 && map_get_cell(map, x - 1, y) == CELL_WALL);
	has_wall_right = (map_get_cell(map, x + 1, y) == CELL_WALL);
	if (has_wall_left && !has_wall_right)
		return (textures->wall_corner_ne);
	else if (has_wall_right && !has_wall_left)
		return (textures->wall_corner_nw);
	return (textures->wall_top);
}
