#ifndef GAME_H
# define GAME_H

# include <SDL2/SDL.h>
# include "map.h"
# include "player.h"
# include "utils.h"
# include "obj.h"

/* Dimensions de la fenetre */
# define WINDOW_WIDTH 800
# define WINDOW_HEIGHT 600
# define CELL_SIZE 20

/* Couleurs pour l'affichage des murs */
# define COLOR_WALL_R 0
# define COLOR_WALL_G 0
# define COLOR_WALL_B 0
# define COLOR_WALL_A 255

/* Couleurs pour l'affichage des cases vides */
# define COLOR_EMPTY_R 255
# define COLOR_EMPTY_G 255
# define COLOR_EMPTY_B 255
# define COLOR_EMPTY_A 255

/* Couleurs pour l'affichage du joueur */
# define COLOR_PLAYER_R 255
# define COLOR_PLAYER_G 0
# define COLOR_PLAYER_B 0
# define COLOR_PLAYER_A 255

/* Couleurs pour l'arriere-plan */
# define COLOR_BACKGROUND_R 128
# define COLOR_BACKGROUND_G 128
# define COLOR_BACKGROUND_B 128
# define COLOR_BACKGROUND_A 255

/*
** Structure principale du jeu
** Contient tous les elements necessaires au fonctionnement du jeu
*/
typedef struct s_game
{
	SDL_Window		*window;
	SDL_Renderer	*renderer;
	t_map			*map;
	t_player		*player;
	t_textures		*textures;
	int				running;
	int				camera_x;
	int				camera_y;
	int				use_wasd;
	int				movements;
}	t_game;

/* Fonctions de gestion du jeu */

t_game	*game_create(void);
void	game_destroy(t_game *game);
int		game_init(t_game *game);
void	game_cleanup(t_game *game);
void	game_handle_events(t_game *game);
void	game_update(t_game *game);
void	game_render(t_game *game);
void	game_run(t_game *game);
int		game_load_map(t_game *game, const char *filename);

void	game_move_player(t_game *game, int dx, int dy);
void	game_update_camera(t_game *game);

/* Fonctions de sauvegarde/chargement */
int		game_save(t_game *game, const char *filename);
int		game_load(t_game *game, const char *filename);

/* Menu pause en jeu */
void	game_pause_menu(t_game *game);

#endif
