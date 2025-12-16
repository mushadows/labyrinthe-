#ifndef UTILS_H
# define UTILS_H

# include <SDL2/SDL.h>
# include <SDL2/SDL_image.h>

/*
** Structure pour stocker les textures du jeu
*/
typedef struct s_textures
{
	SDL_Texture	*wall_left;
	SDL_Texture	*wall_right;
	SDL_Texture	*wall_top;
	SDL_Texture	*wall_corner_ne;
	SDL_Texture	*wall_corner_nw;
	SDL_Texture	*wall_center;
	SDL_Texture	*floor;
	SDL_Texture	*exit;
	SDL_Texture	*player;
	SDL_Texture	*potion;
	SDL_Texture	*tresor_closed;
	SDL_Texture	*tresor_empty;
	SDL_Texture	*monster;
}	t_textures;

/*
** Charge toutes les textures necessaires pour le jeu
** @param renderer: le renderer SDL pour creer les textures
** @return pointeur vers la structure de textures, NULL en cas d'erreur
*/
t_textures	*load_textures(SDL_Renderer *renderer);

/*
** Libere toutes les textures chargees
** @param textures: pointeur vers la structure de textures a liberer
*/
void		free_textures(t_textures *textures);

/*
** Obtient la texture de mur appropriée selon les cellules voisines
** @param textures: pointeur vers la structure de textures
** @param map: pointeur vers la carte
** @param x: coordonnée X du mur
** @param y: coordonnée Y du mur
** @return la texture appropriée pour ce mur
*/
SDL_Texture	*get_wall_texture(t_textures *textures, const void *map, int x, int y);

/*
** Obtient la texture de rebord pour un sol au-dessus d'un mur
** @param textures: pointeur vers la structure de textures
** @param map: pointeur vers la carte
** @param x: coordonnée X du sol
** @param y: coordonnée Y du sol
** @return la texture de rebord appropriée, ou NULL si pas de rebord
*/
SDL_Texture	*get_floor_top_edge(t_textures *textures, const void *map, int x, int y);

char	*itoa(int nb);

#endif
