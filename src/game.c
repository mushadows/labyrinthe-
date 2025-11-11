#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/*
** Cree une nouvelle structure de jeu
** Alloue la memoire et initialise les valeurs par defaut
** @return pointeur vers la structure t_game creee, NULL en cas d'erreur
*/
t_game	*game_create(void)
{
	t_game	*game;

	game = (t_game *)malloc(sizeof(t_game));
	assert(game != NULL);
	game->window = NULL;
	game->renderer = NULL;
	game->map = NULL;
	game->running = 0;
	game->camera_x = 0;
	game->camera_y = 0;
	return (game);
}

/*
** Detruit une structure de jeu et libere la memoire
** Nettoie SDL et libere toutes les ressources allouees
** @param game: pointeur vers la structure de jeu a detruire
*/
void	game_destroy(t_game *game)
{
	if (!game)
		return ;
	game_cleanup(game);
	if (game->map)
		map_destroy(game->map);
	free(game);
}

/*
** Initialise SDL et cree la fenetre et le renderer
** @param game: pointeur vers la structure de jeu a initialiser
** @return 1 en cas de succes, 0 en cas d'erreur
*/
int	game_init(t_game *game)
{
	if (!game)
		return (0);
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		printf("Erreur SDL_Init: %s\n", SDL_GetError());
		return (0);
	}
	game->window = SDL_CreateWindow("Labyrinthe SDL2", SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT,
			SDL_WINDOW_SHOWN);
	if (!game->window)
	{
		printf("Erreur création fenêtre: %s\n", SDL_GetError());
		SDL_Quit();
		return (0);
	}
	game->renderer = SDL_CreateRenderer(game->window, -1,
			SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!game->renderer)
	{
		printf("Erreur création renderer: %s\n", SDL_GetError());
		SDL_DestroyWindow(game->window);
		SDL_Quit();
		return (0);
	}
	game->running = 1;
	return (1);
}

/*
** Nettoie les ressources SDL (fenetre et renderer)
** @param game: pointeur vers la structure de jeu a nettoyer
*/
void	game_cleanup(t_game *game)
{
	if (!game)
		return ;
	if (game->renderer)
	{
		SDL_DestroyRenderer(game->renderer);
		game->renderer = NULL;
	}
	if (game->window)
	{
		SDL_DestroyWindow(game->window);
		game->window = NULL;
	}
	SDL_Quit();
}

/*
** Gere l'evenement de fermeture de la fenetre
** @param game: pointeur vers la structure de jeu
*/
static void	handle_quit_event(t_game *game)
{
	game->running = 0;
}

/*
** Gere les evenements clavier (deplacement et quitter)
** @param game: pointeur vers la structure de jeu
** @param key: code de la touche pressee
*/
static void	handle_key_event(t_game *game, SDL_Keycode key)
{
	if (key == SDLK_ESCAPE)
		game->running = 0;
	else if (key == SDLK_UP || key == SDLK_w)
		game_move_player(game, 0, -1);
	else if (key == SDLK_DOWN || key == SDLK_s)
		game_move_player(game, 0, 1);
	else if (key == SDLK_LEFT || key == SDLK_a)
		game_move_player(game, -1, 0);
	else if (key == SDLK_RIGHT || key == SDLK_d)
		game_move_player(game, 1, 0);
}

/*
** Traite tous les evenements SDL en attente
** Gere les evenements de fermeture et les touches du clavier
** @param game: pointeur vers la structure de jeu
*/
void	game_handle_events(t_game *game)
{
	SDL_Event	event;

	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
			handle_quit_event(game);
		else if (event.type == SDL_KEYDOWN)
			handle_key_event(game, event.key.keysym.sym);
	}
}

/*
** Met a jour l'etat du jeu (camera principalement)
** @param game: pointeur vers la structure de jeu
*/
void	game_update(t_game *game)
{
	if (!game || !game->map)
		return ;
	game_update_camera(game);
}

/*
** Affiche une cellule de la carte selon son type
** @param game: pointeur vers la structure de jeu
** @param cell_rect: rectangle de la cellule a afficher
** @param cell: type de cellule a afficher
*/
static void	render_cell(t_game *game, SDL_Rect *cell_rect, t_cellType cell)
{
	if (cell == CELL_WALL)
	{
		SDL_SetRenderDrawColor(game->renderer, COLOR_WALL_R, COLOR_WALL_G,
			COLOR_WALL_B, COLOR_WALL_A);
		SDL_RenderFillRect(game->renderer, cell_rect);
	}
	else if (cell == CELL_EMPTY)
	{
		SDL_SetRenderDrawColor(game->renderer, COLOR_EMPTY_R, COLOR_EMPTY_G,
			COLOR_EMPTY_B, COLOR_EMPTY_A);
		SDL_RenderFillRect(game->renderer, cell_rect);
	}
}

/*
** Affiche une zone de la carte comprise entre les coordonnees donnees
** @param game: pointeur vers la structure de jeu
** @param start_x: coordonnee X de debut
** @param start_y: coordonnee Y de debut
** @param end_x: coordonnee X de fin
** @param end_y: coordonnee Y de fin
*/
static void	render_map_cells(t_game *game, int start_x, int start_y, int end_x,
		int end_y)
{
	SDL_Rect	cell_rect;
	int			x;
	int			y;
	t_cellType	cell;

	cell_rect.w = CELL_SIZE;
	cell_rect.h = CELL_SIZE;
	y = start_y;
	while (y < end_y)
	{
		x = start_x;
		while (x < end_x)
		{
			cell_rect.x = (x * CELL_SIZE) - game->camera_x;
			cell_rect.y = (y * CELL_SIZE) - game->camera_y;
			cell = map_get_cell(game->map, x, y);
			render_cell(game, &cell_rect, cell);
			x++;
		}
		y++;
	}
}

/*
** Affiche le joueur a sa position actuelle
** @param game: pointeur vers la structure de jeu
*/
static void	render_player(t_game *game)
{
	SDL_Rect	cell_rect;
	int			player_x;
	int			player_y;

	player_x = map_get_player_x(game->map);
	player_y = map_get_player_y(game->map);
	if (player_x >= 0 && player_y >= 0)
	{
		cell_rect.x = (player_x * CELL_SIZE) - game->camera_x;
		cell_rect.y = (player_y * CELL_SIZE) - game->camera_y;
		cell_rect.w = CELL_SIZE;
		cell_rect.h = CELL_SIZE;
		SDL_SetRenderDrawColor(game->renderer, COLOR_PLAYER_R, COLOR_PLAYER_G,
			COLOR_PLAYER_B, COLOR_PLAYER_A);
		SDL_RenderFillRect(game->renderer, &cell_rect);
	}
}

/*
** Affiche l'ensemble du jeu (carte et joueur)
** Calcule la zone visible et optimise l'affichage
** @param game: pointeur vers la structure de jeu
*/
void	game_render(t_game *game)
{
	int	start_x;
	int	start_y;
	int	end_x;
	int	end_y;

	if (!game || !game->renderer || !game->map)
		return ;
	SDL_SetRenderDrawColor(game->renderer, COLOR_BACKGROUND_R,
		COLOR_BACKGROUND_G, COLOR_BACKGROUND_B, COLOR_BACKGROUND_A);
	SDL_RenderClear(game->renderer);
	start_x = game->camera_x / CELL_SIZE;
	start_y = game->camera_y / CELL_SIZE;
	end_x = start_x + (WINDOW_WIDTH / CELL_SIZE) + 2;
	end_y = start_y + (WINDOW_HEIGHT / CELL_SIZE) + 2;
	if (start_x < 0)
		start_x = 0;
	if (start_y < 0)
		start_y = 0;
	if (end_x > map_get_width(game->map))
		end_x = map_get_width(game->map);
	if (end_y > map_get_height(game->map))
		end_y = map_get_height(game->map);
	render_map_cells(game, start_x, start_y, end_x, end_y);
	render_player(game);
	SDL_RenderPresent(game->renderer);
}

/*
** Boucle principale du jeu
** Gere les evenements, met a jour et affiche le jeu en continu
** @param game: pointeur vers la structure de jeu
*/
void	game_run(t_game *game)
{
	if (!game)
		return ;
	while (game->running)
	{
		game_handle_events(game);
		game_update(game);
		game_render(game);
		SDL_Delay(16);
	}
}

/*
** Charge une carte depuis un fichier
** @param game: pointeur vers la structure de jeu
** @param filename: nom du fichier contenant la carte
** @return 1 en cas de succes, 0 en cas d'erreur
*/
int	game_load_map(t_game *game, const char *filename)
{
	if (!game)
		return (0);
	game->map = map_create();
	if (!game->map)
		return (0);
	if (!map_load_from_file(game->map, filename))
	{
		map_destroy(game->map);
		game->map = NULL;
		return (0);
	}
	game_update_camera(game);
	return (1);
}

/*
** Deplace le joueur selon le deplacement demande
** Verifie que la nouvelle position est valide avant le deplacement
** @param game: pointeur vers la structure de jeu
** @param dx: deplacement en X
** @param dy: deplacement en Y
*/
void	game_move_player(t_game *game, int dx, int dy)
{
	int	new_x;
	int	new_y;

	if (!game || !game->map)
		return ;
	new_x = map_get_player_x(game->map) + dx;
	new_y = map_get_player_y(game->map) + dy;
	if (map_is_valid_position(game->map, new_x, new_y))
		map_set_player_position(game->map, new_x, new_y);
}

/*
** Met a jour la position de la camera pour suivre le joueur
** Centre la camera sur le joueur tout en respectant les limites de la carte
** @param game: pointeur vers la structure de jeu
*/
void	game_update_camera(t_game *game)
{
	int	max_camera_x;
	int	max_camera_y;

	if (!game || !game->map)
		return ;
	game->camera_x = (map_get_player_x(game->map) * CELL_SIZE) - (WINDOW_WIDTH / 2);
	game->camera_y = (map_get_player_y(game->map) * CELL_SIZE) - (WINDOW_HEIGHT / 2);
	max_camera_x = (map_get_width(game->map) * CELL_SIZE) - WINDOW_WIDTH;
	max_camera_y = (map_get_height(game->map) * CELL_SIZE) - WINDOW_HEIGHT;
	if (game->camera_x < 0)
		game->camera_x = 0;
	if (game->camera_y < 0)
		game->camera_y = 0;
	if (game->camera_x > max_camera_x)
		game->camera_x = max_camera_x;
	if (game->camera_y > max_camera_y)
		game->camera_y = max_camera_y;
}
