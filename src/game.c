#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

static void	load_game_settings(t_game *game)
{
	FILE	*file;
	char	buffer[256];
	int		use_wasd;

	file = fopen("settings.txt", "r");
	if (!file)
	{
		game->use_wasd = 1;
		return ;
	}
	while (fgets(buffer, sizeof(buffer), file))
	{
		if (sscanf(buffer, "USE_WASD %d", &use_wasd) == 1)
			game->use_wasd = use_wasd;
	}
	fclose(file);
}

t_game	*game_create(void)
{
	t_game	*game;

	game = (t_game *)malloc(sizeof(t_game));
	assert(game != NULL);
	game->window = NULL;
	game->renderer = NULL;
	game->map = NULL;
	game->player = NULL;
	game->textures = NULL;
	game->running = 0;
	game->camera_x = 0;
	game->camera_y = 0;
	game->use_wasd = 1;
	game->movements = 0;
	game->monsters = NULL;
	game->monster_count = 0;
	game->player_moves_since_monster_move = 0;
	load_game_settings(game);
	srand(time(NULL));
	return (game);
}

void	game_destroy(t_game *game)
{
	int	i;

	if (!game)
		return ;
	game_cleanup(game);
	if (game->textures)
		free_textures(game->textures);
	if (game->map)
		map_destroy(game->map);
	if (game->player)
		free(game->player);
	if (game->monsters)
	{
		i = 0;
		while (i < game->monster_count)
		{
			if (game->monsters[i])
				destroy_monster(game->monsters[i]);
			i++;
		}
		free(game->monsters);
	}
	free(game);
}

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
		printf("Erreur crÃ©ation fenÃªtre: %s\n", SDL_GetError());
		SDL_Quit();
		return (0);
	}
	game->renderer = SDL_CreateRenderer(game->window, -1,
			SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!game->renderer)
	{
		printf("Erreur crÃ©ation renderer: %s\n", SDL_GetError());
		SDL_DestroyWindow(game->window);
		SDL_Quit();
		return (0);
	}
	game->textures = load_textures(game->renderer);
	if (!game->textures)
	{
		printf("Erreur chargement des textures\n");
		SDL_DestroyRenderer(game->renderer);
		SDL_DestroyWindow(game->window);
		SDL_Quit();
		return (0);
	}
	game->running = 1;
	return (1);
}

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

static void	handle_quit_event(t_game *game)
{
	game->running = 0;
}

static void	handle_key_event(t_game *game, SDL_Keycode key)
{
	if (key == SDLK_ESCAPE)
		game_pause_menu(game);
	else if (key == SDLK_UP || (game->use_wasd && key == SDLK_w) || (!game->use_wasd && key == SDLK_z))
		game_move_player(game, 0, -1);
	else if (key == SDLK_DOWN || (game->use_wasd && key == SDLK_s) || (!game->use_wasd && key == SDLK_s))
		game_move_player(game, 0, 1);
	else if (key == SDLK_LEFT || (game->use_wasd && key == SDLK_a) || (!game->use_wasd && key == SDLK_q))
		game_move_player(game, -1, 0);
	else if (key == SDLK_RIGHT || (game->use_wasd && key == SDLK_d) || (!game->use_wasd && key == SDLK_d))
		game_move_player(game, 1, 0);
	else if (key == SDLK_F1)
		game->use_wasd = !game->use_wasd;
	else if (key == SDLK_F5)
		game_save(game, "save.txt");
}

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

void	game_update(t_game *game)
{
	if (!game || !game->map)
		return ;
	game_update_camera(game);
}

void	draw_obj(t_game *game, SDL_Rect *cell_rect, t_cellType cell, int x, int y)
{
	SDL_Texture	*edge_texture;

	if (cell == CELL_POTION)
	{
		SDL_RenderCopy(game->renderer, game->textures->potion, NULL, cell_rect);
		edge_texture = get_floor_top_edge(game->textures, game->map, x, y);
		if (edge_texture)
			SDL_RenderCopy(game->renderer, edge_texture, NULL, cell_rect);
	}
	if (cell == CELL_TRESOR_CLOSED)
	{
		SDL_RenderCopy(game->renderer, game->textures->tresor_closed, NULL, cell_rect);
		edge_texture = get_floor_top_edge(game->textures, game->map, x, y);
		if (edge_texture)
			SDL_RenderCopy(game->renderer, edge_texture, NULL, cell_rect);
	}
	if (cell == CELL_TRESOR_EMPTY)
	{
		SDL_RenderCopy(game->renderer, game->textures->tresor_empty, NULL, cell_rect);
		edge_texture = get_floor_top_edge(game->textures, game->map, x, y);
		if (edge_texture)
			SDL_RenderCopy(game->renderer, edge_texture, NULL, cell_rect);
	}
}

/*
** Affiche une cellule de la carte selon son type
** @param game: pointeur vers la structure de jeu
** @param cell_rect: rectangle de la cellule a afficher
** @param cell: type de cellule a afficher
** @param x: coordonnee X de la cellule
** @param y: coordonnee Y de la cellule
*/
static void	render_cell(t_game *game, SDL_Rect *cell_rect, t_cellType cell, int x, int y)
{
	SDL_Texture	*wall_texture;
	SDL_Texture	*edge_texture;

	if (cell == CELL_WALL)
	{
		if (game->textures)
		{
			wall_texture = get_wall_texture(game->textures, game->map, x, y);
			if (wall_texture)
				SDL_RenderCopy(game->renderer, wall_texture, NULL, cell_rect);
		}
		else
		{
			SDL_SetRenderDrawColor(game->renderer, COLOR_WALL_R, COLOR_WALL_G,
				COLOR_WALL_B, COLOR_WALL_A);
			SDL_RenderFillRect(game->renderer, cell_rect);
		}
	}
	else if (cell == CELL_TRAP && (game->movements % 4 == 0 || (game->movements - 1) % 4 == 0))
	{
		if (game->textures && game->textures->trap)
		{
			SDL_Texture	*edge_texture;

			SDL_RenderCopy(game->renderer, game->textures->trap, NULL, cell_rect);
			edge_texture = get_floor_top_edge(game->textures, game->map, x, y);
			if (edge_texture)
				SDL_RenderCopy(game->renderer, edge_texture, NULL, cell_rect);
		}
	}
	else if (cell == CELL_EMPTY || cell == CELL_POTION || cell == CELL_TRESOR_EMPTY || cell == CELL_TRESOR_CLOSED || cell == CELL_MONSTER || !game->player->have_key || cell == CELL_TRAP)
	{
		if (game->textures && game->textures->floor)
		{
			SDL_RenderCopy(game->renderer, game->textures->floor, NULL, cell_rect);
			edge_texture = get_floor_top_edge(game->textures, game->map, x, y);
			if (edge_texture)
				SDL_RenderCopy(game->renderer, edge_texture, NULL, cell_rect);
		}
		else
		{
			SDL_SetRenderDrawColor(game->renderer, COLOR_EMPTY_R, COLOR_EMPTY_G,
				COLOR_EMPTY_B, COLOR_EMPTY_A);
			SDL_RenderFillRect(game->renderer, cell_rect);
		}
		if (cell == CELL_POTION || cell == CELL_TRESOR_EMPTY || cell == CELL_TRESOR_CLOSED)
			draw_obj(game, cell_rect, cell, x, y);
	}
	else if (cell == CELL_EXIT)
	{
		if (game->textures && game->textures->exit)
			SDL_RenderCopy(game->renderer, game->textures->exit, NULL, cell_rect);
		else
		{
			SDL_SetRenderDrawColor(game->renderer, 0, 255, 0, 255);
			SDL_RenderFillRect(game->renderer, cell_rect);
		}
	}
}

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
			render_cell(game, &cell_rect, cell, x, y);
			x++;
		}
		y++;
	}
}

static void	render_monsters(t_game *game)
{
	SDL_Rect	cell_rect;
	int			i;

	if (!game->monsters)
		return ;
	cell_rect.w = CELL_SIZE;
	cell_rect.h = CELL_SIZE;
	i = 0;
	while (i < game->monster_count)
	{
		if (game->monsters[i] && game->monsters[i]->active)
		{
			cell_rect.x = (game->monsters[i]->position.x * CELL_SIZE) - game->camera_x;
			cell_rect.y = (game->monsters[i]->position.y * CELL_SIZE) - game->camera_y;
			if (game->textures && game->textures->monster)
				SDL_RenderCopy(game->renderer, game->textures->monster, NULL, &cell_rect);
			else
			{
				SDL_SetRenderDrawColor(game->renderer, 255, 0, 255, 255);
				SDL_RenderFillRect(game->renderer, &cell_rect);
			}
		}
		i++;
	}
}

static void	render_player(t_game *game)
{
	SDL_Rect	cell_rect;

	if (!game->player)
		return ;
	if (game->player->position.x >= 0 && game->player->position.y >= 0)
	{
		cell_rect.x = (game->player->position.x * CELL_SIZE) - game->camera_x;
		cell_rect.y = (game->player->position.y * CELL_SIZE) - game->camera_y;
		cell_rect.w = CELL_SIZE;
		cell_rect.h = CELL_SIZE;
		if (game->textures && game->textures->player)
			SDL_RenderCopy(game->renderer, game->textures->player, NULL, &cell_rect);
		else
		{
			SDL_SetRenderDrawColor(game->renderer, COLOR_PLAYER_R, COLOR_PLAYER_G,
				COLOR_PLAYER_B, COLOR_PLAYER_A);
			SDL_RenderFillRect(game->renderer, &cell_rect);
		}
	}
}

static void	draw_number(SDL_Renderer *renderer, char c, int x, int y, int s);
static void	render_score_player(t_game *game)
{
	int		i;
	int		len;
	char	*nb;

	i = -1;
	nb = itoa(game->movements);
	len = strlen(nb);
	while (++i < len)
		draw_number(game->renderer, nb[i], WINDOW_WIDTH - 240 - 16 - 6 - 16 - 24 * (len - i), 16, 6);
	free(nb);
}

static void	render_pv_player(t_game *game)
{
	SDL_Rect	bg_rect;
	SDL_Rect	border_rect;
	SDL_Rect	life_rect;

	bg_rect.x = WINDOW_WIDTH - 240 - 16;
	bg_rect.y = 16;
	bg_rect.w = 240;
	bg_rect.h = 32;
	border_rect = bg_rect;
	border_rect.x -= 2;
	border_rect.y -= 2;
	border_rect.w += 5;
	border_rect.h += 5;
	life_rect = bg_rect;
	if (game->player->max_life)
		life_rect.w = life_rect.w * game->player->life / game->player->max_life;
	else
		life_rect.w = life_rect.w * game->player->life / MAX_PV;
	SDL_SetRenderDrawColor(game->renderer, 100, 100, 100, 255);
	SDL_RenderFillRect(game->renderer, &border_rect);
	SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 180);
	SDL_RenderFillRect(game->renderer, &bg_rect);
	SDL_SetRenderDrawColor(game->renderer, 255, 60, 26, 255);
	SDL_RenderFillRect(game->renderer, &life_rect);
}

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
	render_monsters(game);
	render_player(game);
	render_score_player(game);
	render_pv_player(game);
	SDL_RenderPresent(game->renderer);
}

int	game_run(t_game *game)
{
	int	run;

	if (!game)
		return 0;
	run = game->running;
	while (game->running && game->running == run)
	{
		if (game->player->life <= 0)
		{
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Defaite !",
				"Vous avez perdu :(", game->window);
			game->running = 0;
		}
		game_handle_events(game);
		game_update(game);
		game_render(game);
		SDL_Delay(16);
	}
	return run;
}

void	game_spawn_monsters(t_game *game)
{
	int	i;
	int	x;
	int	y;
	int	attempts;

	if (!game || !game->map)
		return ;
	game->monster_count = map_get_width(game->map) / 3 + 4 * game->running;
	if (game->monster_count < 1)
		game->monster_count = 1;
	game->monsters = (t_monster **)malloc(sizeof(t_monster *) * game->monster_count);
	assert(game->monsters != NULL);
	i = 0;
	while (i < game->monster_count)
	{
		attempts = 0;
		while (attempts < 100)
		{
			x = rand() % map_get_width(game->map);
			y = rand() % map_get_height(game->map);
			if (map_is_valid_position(game->map, x, y)
				&& (x != game->player->position.x || y != game->player->position.y))
			{
				game->monsters[i] = create_monster(x, y);
				break ;
			}
			attempts++;
		}
		if (attempts >= 100)
			game->monsters[i] = create_monster(1, 1);
		i++;
	}
}

void	game_move_monsters(t_game *game)
{
	int	i;

	if (!game || !game->monsters)
		return ;
	i = 0;
	while (i < game->monster_count)
	{
		if (game->monsters[i] && game->monsters[i]->active)
			move_monster_random(game->monsters[i], game->map);
		i++;
	}
}

void	game_check_monster_collision(t_game *game)
{
	int	i;

	if (!game || !game->monsters || !game->player)
		return ;
	i = 0;
	while (i < game->monster_count)
	{
		if (game->monsters[i] && game->monsters[i]->active)
		{
			if (game->monsters[i]->position.x == game->player->position.x
				&& game->monsters[i]->position.y == game->player->position.y)
			{
				add_life(game->player, MONSTER);
				game->monsters[i]->active = 0;
				if (game->player->life <= 0)
				{
					game_render(game);
					SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Game Over",
						"Vous Ãªtes mort !", game->window);
					game->running = 0;
				}
			}
		}
		i++;
	}
}

int	game_load_map(t_game *game, const char *filename)
{
	t_position	start_pos;

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
	game->player = create_player();
	if (!game->player)
	{
		map_destroy(game->map);
		game->map = NULL;
		return (0);
	}
	start_pos.x = map_get_player_x(game->map);
	start_pos.y = map_get_player_y(game->map);
	tp_player(game->player, start_pos);
	game_spawn_monsters(game);
	game_update_camera(game);
	return (1);
}

void	game_apply_object(t_game *game, t_cellType cell)
{
	if (cell == CELL_POTION)
	{
		add_life(game->player, POTION);
		game->map->grid[game->player->position.y][game->player->position.x] = CELL_EMPTY;
	}
	if (cell == CELL_TRESOR_CLOSED)
	{
		add_life(game->player, CHEST);
		game->map->grid[game->player->position.y][game->player->position.x] = CELL_TRESOR_EMPTY;
	}
	if (cell == CELL_MONSTER)
	{
		add_life(game->player, MONSTER);
		// game->map->grid[game->player->position.y][game->player->position.x] = CELL_EMPTY;
	}
	if (cell == CELL_KEY)
	{
		game->player->have_key++;
		game->map->grid[game->player->position.y][game->player->position.x] = CELL_EMPTY;
	}
	if (cell == CELL_TRAP && (game->movements % 4 == 0 || (game->movements - 1) % 4 == 0))
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Defaite !",
			"Vous avez perdu :(", game->window);
		game->running = 0;
	}
}

/*
** Deplace le joueur selon le deplacement demande
** Verifie que la nouvelle position est valide avant le deplacement
** Utilise les fonctions de player.c pour le deplacement
** @param game: pointeur vers la structure de jeu
** @param dx: deplacement en X
** @param dy: deplacement en Y
*/
void	game_move_player(t_game *game, int dx, int dy)
{
	int			new_x;
	int			new_y;
	t_cellType	cell;

	if (!game || !game->map || !game->player)
		return ;
	if (dx == 1)
		player_move_right(game->player);
	else if (dx == -1)
		player_move_left(game->player);
	else if (dy == 1)
		player_move_down(game->player);
	else if (dy == -1)
		player_move_up(game->player);
	new_x = game->player->position.x;
	new_y = game->player->position.y;
	if (!map_is_valid_position(game->map, new_x, new_y))
	{
		if (dx == 1)
			player_move_left(game->player);
		else if (dx == -1)
			player_move_right(game->player);
		else if (dy == 1)
			player_move_up(game->player);
		else if (dy == -1)
			player_move_down(game->player);
	}
	else
	{
		game->movements++;
		game->player_moves_since_monster_move++;
		if (game->player_moves_since_monster_move >= 3)
		{
			game_move_monsters(game);
			game->player_moves_since_monster_move = 0;
		}
		game_check_monster_collision(game);
		cell = map_get_cell(game->map, new_x, new_y);
		if (cell == CELL_EXIT && game->player->have_key == 1)
		{
			map_set_player_position(game->map, new_x, new_y);
			game_render(game);
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Victoire !",
				"FÃ©licitations ! Vous avez atteint la sortie !", game->window);
			game->running++;
		}
		else if (cell == CELL_POTION || cell == CELL_TRESOR_CLOSED || cell == CELL_MONSTER || cell == CELL_KEY || (cell == CELL_TRAP && (game->movements % 4 == 0 || (game->movements - 1) % 4 == 0)))
		{
			map_set_player_position(game->map, new_x, new_y);
			game_render(game);
			game_apply_object(game, cell);
		}
		else
			map_set_player_position(game->map, new_x, new_y);
	}
}

void	game_update_camera(t_game *game)
{
	int	max_camera_x;
	int	max_camera_y;

	if (!game || !game->map || !game->player)
		return ;
	game->camera_x = (game->player->position.x * CELL_SIZE) - (WINDOW_WIDTH / 2);
	game->camera_y = (game->player->position.y * CELL_SIZE) - (WINDOW_HEIGHT / 2);
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

int	game_save(t_game *game, const char *filename)
{
	FILE	*file;
	char	map_filename[256];
	int		i;

	if (!game || !game->player || !game->map)
		return (0);
	file = fopen(filename, "w");
	if (!file)
	{
		printf("Erreur: impossible de crÃ©er le fichier de sauvegarde %s\n", filename);
		return (0);
	}
	fprintf(file, "PLAYER_POS %d %d\n", game->player->position.x, game->player->position.y);
	fprintf(file, "PLAYER_LIFE %d\n", game->player->life);
	fprintf(file, "PLAYER_SCORE %d\n", game->movements);
	fprintf(file, "PLAYER_KEY %d\n", game->player->have_key);
	fprintf(file, "USE_WASD %d\n", game->use_wasd);
	fprintf(file, "MONSTER_COUNT %d\n", game->monster_count);
	fprintf(file, "PLAYER_MOVES_MONSTER %d\n", game->player_moves_since_monster_move);
	i = 0;
	while (i < game->monster_count)
	{
		if (game->monsters[i])
		{
			fprintf(file, "MONSTER %d %d %d\n", game->monsters[i]->position.x,
				game->monsters[i]->position.y, game->monsters[i]->active);
		}
		i++;
	}
	snprintf(map_filename, sizeof(map_filename), "maps/saved_map.txt");
	fprintf(file, "MAP_FILE %s\n", map_filename);
	fclose(file);
	if (!map_save_to_file(game->map, map_filename))
	{
		printf("Erreur: impossible de sauvegarder la carte\n");
		return (0);
	}
	printf("Partie sauvegardÃ©e dans %s\n", filename);
	return (1);
}

int	game_load(t_game *game, const char *filename)
{
	FILE		*file;
	char		buffer[256];
	t_position	pos;
	int			score;
	int			life;
	int			use_wasd;
	int			have_key;
	char		map_filename[256];
	int			monster_count;
	int			monster_x;
	int			monster_y;
	int			monster_active;
	int			i;
	int			player_moves_monster;

	if (!game)
		return (0);
	file = fopen(filename, "r");
	if (!file)
	{
		printf("Erreur: impossible d'ouvrir le fichier de sauvegarde %s\n", filename);
		return (0);
	}
	map_filename[0] = '\0';
	monster_count = 0;
	i = 0;
	while (fgets(buffer, sizeof(buffer), file))
	{
		if (sscanf(buffer, "PLAYER_POS %d %d", &pos.x, &pos.y) == 2)
		{
			if (game->player)
				tp_player(game->player, pos);
		}
		else if (sscanf(buffer, "PLAYER_SCORE %d", &score) == 1)
		{
			if (score)
				game->movements = score;
		}
		else if (sscanf(buffer, "PLAYER_LIFE %d", &life) == 1)
		{
			if (game->player)
				game->player->life = life;
		}
		else if (sscanf(buffer, "PLAYER_KEY %d", &have_key) == 1)
		{
			game->player->have_key = have_key;
		}
		else if (sscanf(buffer, "USE_WASD %d", &use_wasd) == 1)
			game->use_wasd = use_wasd;
		else if (sscanf(buffer, "MAP_FILE %s", map_filename) == 1)
		{
		}
		else if (sscanf(buffer, "MONSTER_COUNT %d", &monster_count) == 1)
		{
			if (game->monsters)
			{
				while (i < game->monster_count)
				{
					if (game->monsters[i])
						destroy_monster(game->monsters[i]);
					i++;
				}
				free(game->monsters);
			}
			game->monster_count = monster_count;
			game->monsters = (t_monster **)malloc(sizeof(t_monster *) * monster_count);
			i = 0;
		}
		else if (sscanf(buffer, "PLAYER_MOVES_MONSTER %d", &player_moves_monster) == 1)
		{
			game->player_moves_since_monster_move = player_moves_monster;
		}
		else if (sscanf(buffer, "MONSTER %d %d %d", &monster_x, &monster_y, &monster_active) == 3)
		{
			if (i < game->monster_count)
			{
				game->monsters[i] = create_monster(monster_x, monster_y);
				game->monsters[i]->active = monster_active;
				i++;
			}
		}
	}
	fclose(file);
	if (map_filename[0] != '\0')
	{
		if (game->map)
			map_destroy(game->map);
		game->map = map_create();
		if (!map_load_from_file(game->map, map_filename))
		{
			printf("Erreur: impossible de charger la carte sauvegardÃ©e %s\n", map_filename);
			return (0);
		}
	}
	game_update_camera(game);
	printf("Partie chargÃ©e depuis %s\n", filename);
	return (1);
}

static void	save_game_settings(int use_wasd)
{
	FILE	*file;

	file = fopen("settings.txt", "w");
	if (!file)
		return ;
	fprintf(file, "USE_WASD %d\n", use_wasd);
	fclose(file);
}

static void	draw_text_pixel(SDL_Renderer *renderer, int x, int y, int size)
{
	SDL_Rect	rect;

	rect.x = x;
	rect.y = y;
	rect.w = size;
	rect.h = size;
	SDL_RenderFillRect(renderer, &rect);
}

static void	draw_pause_char(SDL_Renderer *renderer, char c, int x, int y, int s)
{
	if (c == 'R')
	{
		draw_text_pixel(renderer, x, y, s);
		draw_text_pixel(renderer, x, y + s, s);
		draw_text_pixel(renderer, x, y + 2*s, s);
		draw_text_pixel(renderer, x, y + 3*s, s);
		draw_text_pixel(renderer, x + s, y, s);
		draw_text_pixel(renderer, x + 2*s, y, s);
		draw_text_pixel(renderer, x + 2*s, y + s, s);
		draw_text_pixel(renderer, x + s, y + 2*s, s);
		draw_text_pixel(renderer, x + 2*s, y + 3*s, s);
	}
	else if (c == 'W' || c == 'Z')
	{
		draw_text_pixel(renderer, x, y, s);
		draw_text_pixel(renderer, x, y + s, s);
		draw_text_pixel(renderer, x + s, y + 2*s, s);
		draw_text_pixel(renderer, x + 2*s, y, s);
		draw_text_pixel(renderer, x + 2*s, y + s, s);
	}
	else if (c == 'A')
	{
		draw_text_pixel(renderer, x + s, y, s);
		draw_text_pixel(renderer, x, y + s, s);
		draw_text_pixel(renderer, x, y + 2*s, s);
		draw_text_pixel(renderer, x, y + 3*s, s);
		draw_text_pixel(renderer, x + s, y + 2*s, s);
		draw_text_pixel(renderer, x + 2*s, y + s, s);
		draw_text_pixel(renderer, x + 2*s, y + 2*s, s);
		draw_text_pixel(renderer, x + 2*s, y + 3*s, s);
	}
	else if (c == 'S')
	{
		draw_text_pixel(renderer, x + s, y, s);
		draw_text_pixel(renderer, x + 2*s, y, s);
		draw_text_pixel(renderer, x, y + s, s);
		draw_text_pixel(renderer, x + s, y + 2*s, s);
		draw_text_pixel(renderer, x + 2*s, y + 3*s, s);
		draw_text_pixel(renderer, x, y + 4*s, s);
		draw_text_pixel(renderer, x + s, y + 4*s, s);
	}
	else if (c == 'D')
	{
		draw_text_pixel(renderer, x, y, s);
		draw_text_pixel(renderer, x, y + s, s);
		draw_text_pixel(renderer, x, y + 2*s, s);
		draw_text_pixel(renderer, x, y + 3*s, s);
		draw_text_pixel(renderer, x + s, y, s);
		draw_text_pixel(renderer, x + 2*s, y + s, s);
		draw_text_pixel(renderer, x + 2*s, y + 2*s, s);
		draw_text_pixel(renderer, x + s, y + 3*s, s);
	}
	else if (c == 'Q')
	{
		draw_text_pixel(renderer, x + s, y, s);
		draw_text_pixel(renderer, x, y + s, s);
		draw_text_pixel(renderer, x, y + 2*s, s);
		draw_text_pixel(renderer, x + s, y + 3*s, s);
		draw_text_pixel(renderer, x + 2*s, y, s);
		draw_text_pixel(renderer, x + 2*s, y + s, s);
		draw_text_pixel(renderer, x + 2*s, y + 2*s, s);
		draw_text_pixel(renderer, x + 3*s, y + 3*s, s);
	}
	else if (c == 'e')
	{
		draw_text_pixel(renderer, x, y, s);
		draw_text_pixel(renderer, x, y + s, s);
		draw_text_pixel(renderer, x, y + 2*s, s);
		draw_text_pixel(renderer, x, y + 3*s, s);
		draw_text_pixel(renderer, x + s, y, s);
		draw_text_pixel(renderer, x + s, y + s, s);
		draw_text_pixel(renderer, x + s, y + 3*s, s);
		draw_text_pixel(renderer, x + 2*s, y, s);
		draw_text_pixel(renderer, x + 2*s, y + 3*s, s);
	}
	else if (c == 'p')
	{
		draw_text_pixel(renderer, x, y, s);
		draw_text_pixel(renderer, x, y + s, s);
		draw_text_pixel(renderer, x, y + 2*s, s);
		draw_text_pixel(renderer, x, y + 3*s, s);
		draw_text_pixel(renderer, x, y + 4*s, s);
		draw_text_pixel(renderer, x + s, y, s);
		draw_text_pixel(renderer, x + s, y + 2*s, s);
		draw_text_pixel(renderer, x + 2*s, y, s);
		draw_text_pixel(renderer, x + 2*s, y + s, s);
	}
	else if (c == 'n')
	{
		draw_text_pixel(renderer, x, y + s, s);
		draw_text_pixel(renderer, x, y + 2*s, s);
		draw_text_pixel(renderer, x, y + 3*s, s);
		draw_text_pixel(renderer, x + s, y, s);
		draw_text_pixel(renderer, x + 2*s, y + s, s);
		draw_text_pixel(renderer, x + 2*s, y + 2*s, s);
		draw_text_pixel(renderer, x + 2*s, y + 3*s, s);
	}
	else if (c == 'd')
	{
		draw_text_pixel(renderer, x + 2*s, y, s);
		draw_text_pixel(renderer, x + 2*s, y + s, s);
		draw_text_pixel(renderer, x + 2*s, y + 2*s, s);
		draw_text_pixel(renderer, x + 2*s, y + 3*s, s);
		draw_text_pixel(renderer, x, y + s, s);
		draw_text_pixel(renderer, x, y + 2*s, s);
		draw_text_pixel(renderer, x + s, y, s);
		draw_text_pixel(renderer, x + s, y + 3*s, s);
	}
	else if (c == 'r')
	{
		draw_text_pixel(renderer, x, y, s);
		draw_text_pixel(renderer, x, y + s, s);
		draw_text_pixel(renderer, x, y + 2*s, s);
		draw_text_pixel(renderer, x, y + 3*s, s);
		draw_text_pixel(renderer, x + s, y, s);
		draw_text_pixel(renderer, x + 2*s, y, s);
	}
	else if (c == 'u')
	{
		draw_text_pixel(renderer, x, y, s);
		draw_text_pixel(renderer, x, y + s, s);
		draw_text_pixel(renderer, x, y + 2*s, s);
		draw_text_pixel(renderer, x + 2*s, y, s);
		draw_text_pixel(renderer, x + 2*s, y + s, s);
		draw_text_pixel(renderer, x + 2*s, y + 2*s, s);
		draw_text_pixel(renderer, x + s, y + 3*s, s);
	}
	else if (c == 'v')
	{
		draw_text_pixel(renderer, x, y, s);
		draw_text_pixel(renderer, x, y + s, s);
		draw_text_pixel(renderer, x + s, y + 2*s, s);
		draw_text_pixel(renderer, x + 2*s, y, s);
		draw_text_pixel(renderer, x + 2*s, y + s, s);
	}
	else if (c == 'a')
	{
		draw_text_pixel(renderer, x + s, y, s);
		draw_text_pixel(renderer, x + 2*s, y + s, s);
		draw_text_pixel(renderer, x + 2*s, y + 2*s, s);
		draw_text_pixel(renderer, x + 2*s, y + 3*s, s);
		draw_text_pixel(renderer, x, y + s, s);
		draw_text_pixel(renderer, x + s, y + 2*s, s);
	}
	else if (c == 'g')
	{
		draw_text_pixel(renderer, x + s, y, s);
		draw_text_pixel(renderer, x, y + s, s);
		draw_text_pixel(renderer, x + s, y + 2*s, s);
		draw_text_pixel(renderer, x + 2*s, y + s, s);
		draw_text_pixel(renderer, x + 2*s, y + 2*s, s);
		draw_text_pixel(renderer, x + 2*s, y + 3*s, s);
		draw_text_pixel(renderer, x + s, y + 4*s, s);
	}
	else if (c == 't')
	{
		draw_text_pixel(renderer, x + s, y, s);
		draw_text_pixel(renderer, x, y + s, s);
		draw_text_pixel(renderer, x + s, y + s, s);
		draw_text_pixel(renderer, x + 2*s, y + s, s);
		draw_text_pixel(renderer, x + s, y + 2*s, s);
		draw_text_pixel(renderer, x + s, y + 3*s, s);
		draw_text_pixel(renderer, x + 2*s, y + 4*s, s);
	}
	else if (c == 'i')
	{
		draw_text_pixel(renderer, x, y, s);
		draw_text_pixel(renderer, x, y + 2*s, s);
		draw_text_pixel(renderer, x, y + 3*s, s);
	}
	else if (c == 's')
	{
		draw_text_pixel(renderer, x + s, y, s);
		draw_text_pixel(renderer, x + 2*s, y, s);
		draw_text_pixel(renderer, x, y + s, s);
		draw_text_pixel(renderer, x + s, y + 2*s, s);
		draw_text_pixel(renderer, x + 2*s, y + 3*s, s);
		draw_text_pixel(renderer, x, y + 4*s, s);
		draw_text_pixel(renderer, x + s, y + 4*s, s);
	}
	else if (c == 'P')
	{
		draw_text_pixel(renderer, x, y, s);
		draw_text_pixel(renderer, x, y + s, s);
		draw_text_pixel(renderer, x, y + 2*s, s);
		draw_text_pixel(renderer, x, y + 3*s, s);
		draw_text_pixel(renderer, x + s, y, s);
		draw_text_pixel(renderer, x + s, y + 2*s, s);
		draw_text_pixel(renderer, x + 2*s, y, s);
		draw_text_pixel(renderer, x + 2*s, y + s, s);
	}
	else if (c == 'm')
	{
		draw_text_pixel(renderer, x, y + s, s);
		draw_text_pixel(renderer, x, y + 2*s, s);
		draw_text_pixel(renderer, x, y + 3*s, s);
		draw_text_pixel(renderer, x + s, y, s);
		draw_text_pixel(renderer, x + 2*s, y + s, s);
		draw_text_pixel(renderer, x + 3*s, y, s);
		draw_text_pixel(renderer, x + 4*s, y + s, s);
		draw_text_pixel(renderer, x + 4*s, y + 2*s, s);
		draw_text_pixel(renderer, x + 4*s, y + 3*s, s);
	}
}

static void	draw_number(SDL_Renderer *renderer, char c, int x, int y, int s)
{
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	if (c == '0')
	{
		draw_text_pixel(renderer, x, y, s);
		draw_text_pixel(renderer, x + s, y, s);
		draw_text_pixel(renderer, x + 2*s, y, s);
		draw_text_pixel(renderer, x, y + s, s);
		draw_text_pixel(renderer, x + 2*s, y + s, s);
		draw_text_pixel(renderer, x, y + 2*s, s);
		draw_text_pixel(renderer, x + 2*s, y + 2*s, s);
		draw_text_pixel(renderer, x, y + 3*s, s);
		draw_text_pixel(renderer, x + 2*s, y + 3*s, s);
		draw_text_pixel(renderer, x, y + 4*s, s);
		draw_text_pixel(renderer, x + s, y + 4*s, s);
		draw_text_pixel(renderer, x + 2*s, y + 4*s, s);
	}
	else if (c == '1')
	{
		draw_text_pixel(renderer, x + s, y, s);
		draw_text_pixel(renderer, x, y + s, s);
		draw_text_pixel(renderer, x + s, y + s, s);
		draw_text_pixel(renderer, x + s, y + 2*s, s);
		draw_text_pixel(renderer, x + s, y + 3*s, s);
		draw_text_pixel(renderer, x + s, y + 4*s, s);
	}
	else if (c == '2')
	{
		draw_text_pixel(renderer, x, y, s);
		draw_text_pixel(renderer, x + s, y, s);
		draw_text_pixel(renderer, x + 2*s, y, s);
		draw_text_pixel(renderer, x + 2*s, y + s, s);
		draw_text_pixel(renderer, x + s, y + 2*s, s);
		draw_text_pixel(renderer, x, y + 3*s, s);
		draw_text_pixel(renderer, x, y + 4*s, s);
		draw_text_pixel(renderer, x + s, y + 4*s, s);
		draw_text_pixel(renderer, x + 2*s, y + 4*s, s);
	}
	else if (c == '3')
	{
		draw_text_pixel(renderer, x, y, s);
		draw_text_pixel(renderer, x + s, y, s);
		draw_text_pixel(renderer, x + 2*s, y + s, s);
		draw_text_pixel(renderer, x, y + 2*s, s);
		draw_text_pixel(renderer, x + s, y + 2*s, s);
		draw_text_pixel(renderer, x + 2*s, y + 2*s, s);
		draw_text_pixel(renderer, x + 2*s, y + 3*s, s);
		draw_text_pixel(renderer, x, y + 4*s, s);
		draw_text_pixel(renderer, x + s, y + 4*s, s);
	}
	else if (c == '4')
	{
		draw_text_pixel(renderer, x, y, s);
		draw_text_pixel(renderer, x + 2*s, y, s);
		draw_text_pixel(renderer, x, y + s, s);
		draw_text_pixel(renderer, x + 2*s, y + s, s);
		draw_text_pixel(renderer, x, y + 2*s, s);
		draw_text_pixel(renderer, x + 2*s, y + 2*s, s);
		draw_text_pixel(renderer, x + s, y + 2*s, s);
		draw_text_pixel(renderer, x + 2*s, y + 3*s, s);
		draw_text_pixel(renderer, x + 2*s, y + 4*s, s);
	}
	else if (c == '5')
	{
		draw_text_pixel(renderer, x, y, s);
		draw_text_pixel(renderer, x + s, y, s);
		draw_text_pixel(renderer, x + 2*s, y, s);
		draw_text_pixel(renderer, x, y + s, s);
		draw_text_pixel(renderer, x, y + 2*s, s);
		draw_text_pixel(renderer, x + s, y + 2*s, s);
		draw_text_pixel(renderer, x + 2*s, y + 2*s, s);
		draw_text_pixel(renderer, x + 2*s, y + 3*s, s);
		draw_text_pixel(renderer, x, y + 4*s, s);
		draw_text_pixel(renderer, x + s, y + 4*s, s);
		draw_text_pixel(renderer, x + 2*s, y + 4*s, s);
	}
	else if (c == '6')
	{
		draw_text_pixel(renderer, x, y, s);
		draw_text_pixel(renderer, x + s, y, s);
		draw_text_pixel(renderer, x + 2*s, y, s);
		draw_text_pixel(renderer, x, y + s, s);
		draw_text_pixel(renderer, x, y + 2*s, s);
		draw_text_pixel(renderer, x + s, y + 2*s, s);
		draw_text_pixel(renderer, x + 2*s, y + 2*s, s);
		draw_text_pixel(renderer, x, y + 3*s, s);
		draw_text_pixel(renderer, x + 2*s, y + 3*s, s);
		draw_text_pixel(renderer, x, y + 4*s, s);
		draw_text_pixel(renderer, x + s, y + 4*s, s);
		draw_text_pixel(renderer, x + 2*s, y + 4*s, s);
	}
	else if (c == '7')
	{
		draw_text_pixel(renderer, x, y, s);
		draw_text_pixel(renderer, x + s, y, s);
		draw_text_pixel(renderer, x + 2*s, y, s);
		draw_text_pixel(renderer, x + 2*s, y + s, s);
		draw_text_pixel(renderer, x + 2*s, y + 2*s, s);
		draw_text_pixel(renderer, x + s, y + 3*s, s);
		draw_text_pixel(renderer, x + s, y + 4*s, s);
	}
	else if (c == '8')
	{
		draw_text_pixel(renderer, x, y, s);
		draw_text_pixel(renderer, x + s, y, s);
		draw_text_pixel(renderer, x + 2*s, y, s);
		draw_text_pixel(renderer, x, y + s, s);
		draw_text_pixel(renderer, x + 2*s, y + s, s);
		draw_text_pixel(renderer, x, y + 2*s, s);
		draw_text_pixel(renderer, x + s, y + 2*s, s);
		draw_text_pixel(renderer, x + 2*s, y + 2*s, s);
		draw_text_pixel(renderer, x, y + 3*s, s);
		draw_text_pixel(renderer, x + 2*s, y + 3*s, s);
		draw_text_pixel(renderer, x, y + 4*s, s);
		draw_text_pixel(renderer, x + s, y + 4*s, s);
		draw_text_pixel(renderer, x + 2*s, y + 4*s, s);
	}
	else if (c == '9')
	{
		draw_text_pixel(renderer, x, y, s);
		draw_text_pixel(renderer, x + s, y, s);
		draw_text_pixel(renderer, x + 2*s, y, s);
		draw_text_pixel(renderer, x, y + s, s);
		draw_text_pixel(renderer, x + 2*s, y + s, s);
		draw_text_pixel(renderer, x, y + 2*s, s);
		draw_text_pixel(renderer, x + s, y + 2*s, s);
		draw_text_pixel(renderer, x + 2*s, y + 2*s, s);
		draw_text_pixel(renderer, x + 2*s, y + 3*s, s);
		draw_text_pixel(renderer, x, y + 4*s, s);
		draw_text_pixel(renderer, x + s, y + 4*s, s);
		draw_text_pixel(renderer, x + 2*s, y + 4*s, s);
	}
}

static void	draw_pause_text(SDL_Renderer *renderer, const char *text, int x,
		int y, int size)
{
	int	i;
	int	offset_x;
	int	char_width;

	i = 0;
	offset_x = x;
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	while (text[i])
	{
		if (text[i] != ' ')
		{
			draw_pause_char(renderer, text[i], offset_x, y, size);
			if (text[i] == 'Q' || text[i] == 'm')
				char_width = size * 5;
			else
				char_width = size * 4;
		}
		else
			char_width = size * 4;
		offset_x += char_width;
		i++;
	}
}

static void	draw_pause_button(SDL_Renderer *renderer, const char *text, int y,
		int selected)
{
	SDL_Rect	bg_rect;
	SDL_Rect	border_rect;

	bg_rect.x = WINDOW_WIDTH / 2 - 140;
	bg_rect.y = y - 10;
	bg_rect.w = 280;
	bg_rect.h = 50;
	border_rect = bg_rect;
	border_rect.x -= 3;
	border_rect.y -= 3;
	border_rect.w += 6;
	border_rect.h += 6;
	if (selected)
		SDL_SetRenderDrawColor(renderer, 255, 255, 100, 255);
	else
		SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
	SDL_RenderFillRect(renderer, &border_rect);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200);
	SDL_RenderFillRect(renderer, &bg_rect);
	draw_pause_text(renderer, text, WINDOW_WIDTH / 2 - 70, y + 10, 3);
}

static void	render_pause_menu(t_game *game, int selected)
{
	SDL_Rect	overlay;

	game_render(game);
	SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 180);
	overlay.x = 0;
	overlay.y = 0;
	overlay.w = WINDOW_WIDTH;
	overlay.h = WINDOW_HEIGHT;
	SDL_SetRenderDrawBlendMode(game->renderer, SDL_BLENDMODE_BLEND);
	SDL_RenderFillRect(game->renderer, &overlay);
	draw_pause_button(game->renderer, "Reprendre", WINDOW_HEIGHT / 2 - 80,
		selected == 0);
	if (game->use_wasd)
		draw_pause_button(game->renderer, "WASD", WINDOW_HEIGHT / 2 - 10,
			selected == 1);
	else
		draw_pause_button(game->renderer, "ZQSD", WINDOW_HEIGHT / 2 - 10,
			selected == 1);
	draw_pause_button(game->renderer, "Sauvegarder", WINDOW_HEIGHT / 2 + 60,
		selected == 2);
	draw_pause_button(game->renderer, "Quitter", WINDOW_HEIGHT / 2 + 130,
		selected == 3);
	SDL_RenderPresent(game->renderer);
}

void	game_pause_menu(t_game *game)
{
	SDL_Event	event;
	int			selected;
	int			paused;

	if (!game)
		return ;
	selected = 0;
	paused = 1;
	while (paused)
	{
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				game->running = 0;
				paused = 0;
			}
			else if (event.type == SDL_KEYDOWN)
			{
				if (event.key.keysym.sym == SDLK_ESCAPE)
					paused = 0;
				else if (event.key.keysym.sym == SDLK_UP)
				{
					selected--;
					if (selected < 0)
						selected = 3;
				}
				else if (event.key.keysym.sym == SDLK_DOWN)
				{
					selected++;
					if (selected > 3)
						selected = 0;
				}
				else if (event.key.keysym.sym == SDLK_RETURN)
				{
					if (selected == 0)
						paused = 0;
					else if (selected == 1)
					{
						game->use_wasd = !game->use_wasd;
						save_game_settings(game->use_wasd);
					}
					else if (selected == 2)
						game_save(game, "save.txt");
					else if (selected == 3)
					{
						game->running = 0;
						paused = 0;
					}
				}
			}
		}
		render_pause_menu(game, selected);
		SDL_Delay(16);
	}
}