#include "menu.h"
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL_ttf.h>

# define WINDOW_WIDTH 800
# define WINDOW_HEIGHT 600

/*
** Cree une nouvelle structure de menu
** @return pointeur vers la structure t_menu creee, NULL en cas d'erreur
*/
t_menu	*menu_create(void)
{
	t_menu	*menu;

	menu = (t_menu *)malloc(sizeof(t_menu));
	if (!menu)
		return (NULL);
	menu->window = NULL;
	menu->renderer = NULL;
	menu->selected_option = 0;
	menu->running = 1;
	menu->background_map = NULL;
	menu->textures = NULL;
	return (menu);
}

/*
** Detruit une structure de menu et libere la memoire
** @param menu: pointeur vers la structure de menu a detruire
*/
void	menu_destroy(t_menu *menu)
{
	if (!menu)
		return ;
	menu_cleanup(menu);
	if (menu->textures)
		free_textures(menu->textures);
	if (menu->background_map)
		map_destroy(menu->background_map);
	free(menu);
}

/*
** Initialise SDL et cree la fenetre et le renderer pour le menu
** @param menu: pointeur vers la structure de menu a initialiser
** @return 1 en cas de succes, 0 en cas d'erreur
*/
int	menu_init(t_menu *menu)
{
	if (!menu)
		return (0);
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		printf("Erreur SDL_Init: %s\n", SDL_GetError());
		return (0);
	}
	menu->window = SDL_CreateWindow("Labyrinthe - Menu Principal",
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
	if (!menu->window)
	{
		printf("Erreur création fenêtre: %s\n", SDL_GetError());
		SDL_Quit();
		return (0);
	}
	menu->renderer = SDL_CreateRenderer(menu->window, -1,
			SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!menu->renderer)
	{
		printf("Erreur création renderer: %s\n", SDL_GetError());
		SDL_DestroyWindow(menu->window);
		SDL_Quit();
		return (0);
	}
	menu->textures = load_textures(menu->renderer);
	menu->background_map = map_create();
	if (menu->background_map)
		map_load_from_file(menu->background_map, "maps/labyrinthe.txt");
	return (1);
}

/*
** Nettoie les ressources SDL du menu
** @param menu: pointeur vers la structure de menu a nettoyer
*/
void	menu_cleanup(t_menu *menu)
{
	if (!menu)
		return ;
	if (menu->renderer)
	{
		SDL_DestroyRenderer(menu->renderer);
		menu->renderer = NULL;
	}
	if (menu->window)
	{
		SDL_DestroyWindow(menu->window);
		menu->window = NULL;
	}
	SDL_Quit();
}

/*
** Gere les evenements du menu
** @param menu: pointeur vers la structure de menu
** @param choice: pointeur vers le choix selectionne
*/
void	menu_handle_events(t_menu *menu, t_menu_choice *choice)
{
	SDL_Event	event;

	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
		{
			*choice = MENU_QUIT;
			menu->running = 0;
		}
		else if (event.type == SDL_KEYDOWN)
		{
			if (event.key.keysym.sym == SDLK_ESCAPE)
			{
				*choice = MENU_QUIT;
				menu->running = 0;
			}
			else if (event.key.keysym.sym == SDLK_UP)
			{
				menu->selected_option--;
				if (menu->selected_option < 0)
					menu->selected_option = 3;
			}
			else if (event.key.keysym.sym == SDLK_DOWN)
			{
				menu->selected_option++;
				if (menu->selected_option > 3)
					menu->selected_option = 0;
			}
			else if (event.key.keysym.sym == SDLK_RETURN)
			{
				if (menu->selected_option == 0)
					*choice = MENU_PLAY;
				else if (menu->selected_option == 1)
					*choice = MENU_LOAD;
				else if (menu->selected_option == 2)
					*choice = MENU_SETTINGS;
				else if (menu->selected_option == 3)
					*choice = MENU_QUIT;
				menu->running = 0;
			}
		}
	}
}

/*
** Dessine un pixel pour creer du texte bitmap
** @param renderer: le renderer SDL
** @param x: position X
** @param y: position Y
** @param size: taille du pixel
*/
static void	draw_pixel(SDL_Renderer *renderer, int x, int y, int size)
{
	SDL_Rect	rect;

	rect.x = x;
	rect.y = y;
	rect.w = size;
	rect.h = size;
	SDL_RenderFillRect(renderer, &rect);
}

/*
** Dessine une lettre simple en bitmap
** @param renderer: le renderer SDL
** @param c: caractere a dessiner
** @param x: position X
** @param y: position Y
** @param size: taille du pixel
*/
static void	draw_char(SDL_Renderer *renderer, char c, int x, int y, int size)
{
	if (c == 'J')
	{
		draw_pixel(renderer, x, y, size);
		draw_pixel(renderer, x + size, y, size);
		draw_pixel(renderer, x + 2*size, y, size);
		draw_pixel(renderer, x + size, y + size, size);
		draw_pixel(renderer, x + size, y + 2*size, size);
		draw_pixel(renderer, x, y + 3*size, size);
		draw_pixel(renderer, x + size, y + 3*size, size);
	}
	else if (c == 'o')
	{
		draw_pixel(renderer, x, y + size, size);
		draw_pixel(renderer, x, y + 2*size, size);
		draw_pixel(renderer, x + size, y, size);
		draw_pixel(renderer, x + 2*size, y + size, size);
		draw_pixel(renderer, x + 2*size, y + 2*size, size);
		draw_pixel(renderer, x + size, y + 3*size, size);
	}
	else if (c == 'u')
	{
		draw_pixel(renderer, x, y, size);
		draw_pixel(renderer, x, y + size, size);
		draw_pixel(renderer, x, y + 2*size, size);
		draw_pixel(renderer, x + 2*size, y, size);
		draw_pixel(renderer, x + 2*size, y + size, size);
		draw_pixel(renderer, x + 2*size, y + 2*size, size);
		draw_pixel(renderer, x + size, y + 3*size, size);
	}
	else if (c == 'e')
	{
		draw_pixel(renderer, x, y, size);
		draw_pixel(renderer, x, y + size, size);
		draw_pixel(renderer, x, y + 2*size, size);
		draw_pixel(renderer, x, y + 3*size, size);
		draw_pixel(renderer, x + size, y, size);
		draw_pixel(renderer, x + size, y + size, size);
		draw_pixel(renderer, x + size, y + 3*size, size);
		draw_pixel(renderer, x + 2*size, y, size);
		draw_pixel(renderer, x + 2*size, y + 3*size, size);
	}
	else if (c == 'r')
	{
		draw_pixel(renderer, x, y, size);
		draw_pixel(renderer, x, y + size, size);
		draw_pixel(renderer, x, y + 2*size, size);
		draw_pixel(renderer, x, y + 3*size, size);
		draw_pixel(renderer, x + size, y, size);
		draw_pixel(renderer, x + 2*size, y, size);
	}
	else if (c == 'C')
	{
		draw_pixel(renderer, x + size, y, size);
		draw_pixel(renderer, x + 2*size, y, size);
		draw_pixel(renderer, x, y + size, size);
		draw_pixel(renderer, x, y + 2*size, size);
		draw_pixel(renderer, x + size, y + 3*size, size);
		draw_pixel(renderer, x + 2*size, y + 3*size, size);
	}
	else if (c == 'h')
	{
		draw_pixel(renderer, x, y, size);
		draw_pixel(renderer, x, y + size, size);
		draw_pixel(renderer, x, y + 2*size, size);
		draw_pixel(renderer, x, y + 3*size, size);
		draw_pixel(renderer, x + size, y + size, size);
		draw_pixel(renderer, x + 2*size, y + 2*size, size);
		draw_pixel(renderer, x + 2*size, y + 3*size, size);
	}
	else if (c == 'a')
	{
		draw_pixel(renderer, x + size, y, size);
		draw_pixel(renderer, x + 2*size, y + size, size);
		draw_pixel(renderer, x + 2*size, y + 2*size, size);
		draw_pixel(renderer, x + 2*size, y + 3*size, size);
		draw_pixel(renderer, x, y + size, size);
		draw_pixel(renderer, x + size, y + 2*size, size);
	}
	else if (c == 'g')
	{
		draw_pixel(renderer, x + size, y, size);
		draw_pixel(renderer, x, y + size, size);
		draw_pixel(renderer, x + size, y + 2*size, size);
		draw_pixel(renderer, x + 2*size, y + size, size);
		draw_pixel(renderer, x + 2*size, y + 2*size, size);
		draw_pixel(renderer, x + 2*size, y + 3*size, size);
		draw_pixel(renderer, x + size, y + 4*size, size);
	}
	else if (c == 'Q')
	{
		draw_pixel(renderer, x + size, y, size);
		draw_pixel(renderer, x, y + size, size);
		draw_pixel(renderer, x, y + 2*size, size);
		draw_pixel(renderer, x + size, y + 3*size, size);
		draw_pixel(renderer, x + 2*size, y, size);
		draw_pixel(renderer, x + 2*size, y + size, size);
		draw_pixel(renderer, x + 2*size, y + 2*size, size);
		draw_pixel(renderer, x + 3*size, y + 3*size, size);
	}
	else if (c == 'i')
	{
		draw_pixel(renderer, x, y, size);
		draw_pixel(renderer, x, y + 2*size, size);
		draw_pixel(renderer, x, y + 3*size, size);
	}
	else if (c == 't')
	{
		draw_pixel(renderer, x + size, y, size);
		draw_pixel(renderer, x, y + size, size);
		draw_pixel(renderer, x + size, y + size, size);
		draw_pixel(renderer, x + 2*size, y + size, size);
		draw_pixel(renderer, x + size, y + 2*size, size);
		draw_pixel(renderer, x + size, y + 3*size, size);
		draw_pixel(renderer, x + 2*size, y + 4*size, size);
	}
	else if (c == 'P')
	{
		draw_pixel(renderer, x, y, size);
		draw_pixel(renderer, x, y + size, size);
		draw_pixel(renderer, x, y + 2*size, size);
		draw_pixel(renderer, x, y + 3*size, size);
		draw_pixel(renderer, x + size, y, size);
		draw_pixel(renderer, x + size, y + 2*size, size);
		draw_pixel(renderer, x + 2*size, y, size);
		draw_pixel(renderer, x + 2*size, y + size, size);
	}
	else if (c == 'm')
	{
		draw_pixel(renderer, x, y + size, size);
		draw_pixel(renderer, x, y + 2*size, size);
		draw_pixel(renderer, x, y + 3*size, size);
		draw_pixel(renderer, x + size, y, size);
		draw_pixel(renderer, x + 2*size, y + size, size);
		draw_pixel(renderer, x + 3*size, y, size);
		draw_pixel(renderer, x + 4*size, y + size, size);
		draw_pixel(renderer, x + 4*size, y + 2*size, size);
		draw_pixel(renderer, x + 4*size, y + 3*size, size);
	}
	else if (c == 'S')
	{
		draw_pixel(renderer, x + size, y, size);
		draw_pixel(renderer, x + 2*size, y, size);
		draw_pixel(renderer, x, y + size, size);
		draw_pixel(renderer, x + size, y + 2*size, size);
		draw_pixel(renderer, x + 2*size, y + 3*size, size);
		draw_pixel(renderer, x, y + 4*size, size);
		draw_pixel(renderer, x + size, y + 4*size, size);
	}
	else if (c == 'v')
	{
		draw_pixel(renderer, x, y, size);
		draw_pixel(renderer, x, y + size, size);
		draw_pixel(renderer, x + size, y + 2*size, size);
		draw_pixel(renderer, x + 2*size, y, size);
		draw_pixel(renderer, x + 2*size, y + size, size);
	}
	else if (c == 'd')
	{
		draw_pixel(renderer, x + 2*size, y, size);
		draw_pixel(renderer, x + 2*size, y + size, size);
		draw_pixel(renderer, x + 2*size, y + 2*size, size);
		draw_pixel(renderer, x + 2*size, y + 3*size, size);
		draw_pixel(renderer, x, y + size, size);
		draw_pixel(renderer, x + size, y, size);
		draw_pixel(renderer, x + size, y + 3*size, size);
	}
	else if (c == 'n')
	{
		draw_pixel(renderer, x, y + size, size);
		draw_pixel(renderer, x, y + 2*size, size);
		draw_pixel(renderer, x, y + 3*size, size);
		draw_pixel(renderer, x + size, y, size);
		draw_pixel(renderer, x + 2*size, y + size, size);
		draw_pixel(renderer, x + 2*size, y + 2*size, size);
		draw_pixel(renderer, x + 2*size, y + 3*size, size);
	}
	else if (c == 'l')
	{
		draw_pixel(renderer, x, y, size);
		draw_pixel(renderer, x, y + size, size);
		draw_pixel(renderer, x, y + 2*size, size);
		draw_pixel(renderer, x, y + 3*size, size);
	}
	else if (c == 's')
	{
		draw_pixel(renderer, x + size, y, size);
		draw_pixel(renderer, x, y + size, size);
		draw_pixel(renderer, x + size, y + 2*size, size);
		draw_pixel(renderer, x + 2*size, y + 3*size, size);
	}
	else if (c == ':')
	{
		draw_pixel(renderer, x, y + size, size);
		draw_pixel(renderer, x, y + 3*size, size);
	}
	else if (c == 'R')
	{
		draw_pixel(renderer, x, y, size);
		draw_pixel(renderer, x, y + size, size);
		draw_pixel(renderer, x, y + 2*size, size);
		draw_pixel(renderer, x, y + 3*size, size);
		draw_pixel(renderer, x + size, y, size);
		draw_pixel(renderer, x + size, y + 2*size, size);
		draw_pixel(renderer, x + 2*size, y, size);
		draw_pixel(renderer, x + 2*size, y + size, size);
		draw_pixel(renderer, x + 2*size, y + 3*size, size);
	}
	else if (c == 'W' || c == 'A' || c == 'D' || c == 'Z')
	{
		draw_pixel(renderer, x, y, size);
		draw_pixel(renderer, x, y + size, size);
		draw_pixel(renderer, x + size, y + 2*size, size);
		draw_pixel(renderer, x + 2*size, y, size);
		draw_pixel(renderer, x + 2*size, y + size, size);
	}
}

/*
** Dessine un bouton avec du texte
** @param renderer: le renderer SDL
** @param text: le texte a afficher
** @param x: position X
** @param y: position Y
** @param selected: si l'option est selectionnee
*/
static void	draw_button(SDL_Renderer *renderer, const char *text, int x,
		int y, int selected)
{
	SDL_Rect	bg_rect;
	SDL_Rect	border_rect;
	int			i;
	int			offset_x;
	int			pixel_size;

	bg_rect.x = x - 20;
	bg_rect.y = y - 10;
	bg_rect.w = 280;
	bg_rect.h = 60;
	border_rect = bg_rect;
	border_rect.x -= 3;
	border_rect.y -= 3;
	border_rect.w += 6;
	border_rect.h += 6;
	if (selected)
	{
		SDL_SetRenderDrawColor(renderer, 255, 255, 100, 255);
		SDL_RenderFillRect(renderer, &border_rect);
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200);
	}
	else
	{
		SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
		SDL_RenderFillRect(renderer, &border_rect);
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180);
	}
	SDL_RenderFillRect(renderer, &bg_rect);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	i = 0;
	offset_x = x;
	pixel_size = 4;
	while (text[i])
	{
		draw_char(renderer, text[i], offset_x, y, pixel_size);
		offset_x += pixel_size * 4;
		i++;
	}
}

/*
** Affiche le labyrinthe en arriere-plan
** @param menu: pointeur vers la structure de menu
*/
static void	render_background(t_menu *menu)
{
	SDL_Rect	cell_rect;
	int			x;
	int			y;
	t_cellType	cell_type;

	if (!menu->background_map)
		return ;
	cell_rect.w = 20;
	cell_rect.h = 20;
	y = 0;
	while (y < map_get_height(menu->background_map) && y * 20 < WINDOW_HEIGHT)
	{
		x = 0;
		while (x < map_get_width(menu->background_map) && x * 20 < WINDOW_WIDTH)
		{
			cell_rect.x = x * 20;
			cell_rect.y = y * 20;
			cell_type = map_get_cell(menu->background_map, x, y);
			if (cell_type == CELL_WALL)
			{
				if (menu->textures)
				{
					SDL_Texture	*wall = get_wall_texture(menu->textures,
							menu->background_map, x, y);
					if (wall)
						SDL_RenderCopy(menu->renderer, wall, NULL, &cell_rect);
				}
				else
				{
					SDL_SetRenderDrawColor(menu->renderer, 80, 80, 80, 255);
					SDL_RenderFillRect(menu->renderer, &cell_rect);
				}
			}
			else if (cell_type == CELL_EMPTY || cell_type == CELL_PLAYER)
			{
				if (menu->textures && menu->textures->floor)
					SDL_RenderCopy(menu->renderer, menu->textures->floor, NULL, &cell_rect);
				else
				{
					SDL_SetRenderDrawColor(menu->renderer, 180, 180, 180, 255);
					SDL_RenderFillRect(menu->renderer, &cell_rect);
				}
			}
			else if (cell_type == CELL_EXIT)
			{
				if (menu->textures && menu->textures->exit)
					SDL_RenderCopy(menu->renderer, menu->textures->exit, NULL, &cell_rect);
				else
				{
					SDL_SetRenderDrawColor(menu->renderer, 0, 200, 0, 255);
					SDL_RenderFillRect(menu->renderer, &cell_rect);
				}
			}
			x++;
		}
		y++;
	}
	SDL_SetRenderDrawColor(menu->renderer, 0, 0, 0, 150);
	cell_rect.x = 0;
	cell_rect.y = 0;
	cell_rect.w = WINDOW_WIDTH;
	cell_rect.h = WINDOW_HEIGHT;
	SDL_SetRenderDrawBlendMode(menu->renderer, SDL_BLENDMODE_BLEND);
	SDL_RenderFillRect(menu->renderer, &cell_rect);
}

/*
** Affiche le menu principal
** @param menu: pointeur vers la structure de menu
*/
void	menu_render(t_menu *menu)
{
	if (!menu || !menu->renderer)
		return ;
	SDL_SetRenderDrawColor(menu->renderer, 20, 20, 40, 255);
	SDL_RenderClear(menu->renderer);
	render_background(menu);
	draw_button(menu->renderer, "Jouer", WINDOW_WIDTH / 2 - 100,
		WINDOW_HEIGHT / 2 - 120, menu->selected_option == 0);
	draw_button(menu->renderer, "Charger", WINDOW_WIDTH / 2 - 100,
		WINDOW_HEIGHT / 2 - 40, menu->selected_option == 1);
	draw_button(menu->renderer, "Parametres", WINDOW_WIDTH / 2 - 100,
		WINDOW_HEIGHT / 2 + 40, menu->selected_option == 2);
	draw_button(menu->renderer, "Quitter", WINDOW_WIDTH / 2 - 100,
		WINDOW_HEIGHT / 2 + 120, menu->selected_option == 3);
	SDL_RenderPresent(menu->renderer);
}

/*
** Lance le menu principal et retourne le choix de l'utilisateur
** @param menu: pointeur vers la structure de menu
** @return le choix selectionne par l'utilisateur
*/
t_menu_choice	menu_run(t_menu *menu)
{
	t_menu_choice	choice;

	choice = MENU_NONE;
	if (!menu)
		return (MENU_QUIT);
	menu->running = 1;
	while (menu->running)
	{
		menu_handle_events(menu, &choice);
		menu_render(menu);
		SDL_Delay(16);
	}
	return (choice);
}

/*
** Dessine le menu de parametres
** @param menu: pointeur vers la structure de menu
** @param selected: option selectionnee
** @param use_wasd: si WASD est active (1) ou ZQSD (0)
*/
static void	render_settings(t_menu *menu, int selected, int use_wasd)
{
	SDL_Rect	text_rect;

	SDL_SetRenderDrawColor(menu->renderer, 20, 20, 40, 255);
	SDL_RenderClear(menu->renderer);
	render_background(menu);
	text_rect.x = WINDOW_WIDTH / 2 - 150;
	text_rect.y = 80;
	text_rect.w = 300;
	text_rect.h = 50;
	SDL_SetRenderDrawColor(menu->renderer, 255, 255, 255, 255);
	SDL_RenderDrawRect(menu->renderer, &text_rect);
	if (use_wasd)
		draw_button(menu->renderer, "Controles: WASD", WINDOW_WIDTH / 2 - 100,
			WINDOW_HEIGHT / 2 - 60, selected == 0);
	else
		draw_button(menu->renderer, "Controles: ZQSD", WINDOW_WIDTH / 2 - 100,
			WINDOW_HEIGHT / 2 - 60, selected == 0);
	draw_button(menu->renderer, "Sauvegarder", WINDOW_WIDTH / 2 - 100,
		WINDOW_HEIGHT / 2 + 20, selected == 1);
	draw_button(menu->renderer, "Retour", WINDOW_WIDTH / 2 - 100,
		WINDOW_HEIGHT / 2 + 100, selected == 2);
	SDL_RenderPresent(menu->renderer);
}

/*
** Charge les parametres depuis un fichier
** @param use_wasd: pointeur vers la variable use_wasd
** @return 1 en cas de succes, 0 en cas d'erreur
*/
static int	load_settings(int *use_wasd)
{
	FILE	*file;
	char	buffer[256];

	file = fopen("settings.txt", "r");
	if (!file)
	{
		*use_wasd = 1;
		return (0);
	}
	while (fgets(buffer, sizeof(buffer), file))
	{
		if (sscanf(buffer, "USE_WASD %d", use_wasd) == 1)
		{
			fclose(file);
			return (1);
		}
	}
	fclose(file);
	*use_wasd = 1;
	return (0);
}

/*
** Sauvegarde les parametres dans un fichier
** @param use_wasd: si WASD est active
** @return 1 en cas de succes, 0 en cas d'erreur
*/
static int	save_settings(int use_wasd)
{
	FILE	*file;

	file = fopen("settings.txt", "w");
	if (!file)
	{
		printf("Erreur: impossible de sauvegarder les paramètres\n");
		return (0);
	}
	fprintf(file, "USE_WASD %d\n", use_wasd);
	fclose(file);
	printf("Paramètres sauvegardés\n");
	return (1);
}

/*
** Menu de parametres
** @param menu: pointeur vers la structure de menu
*/
void	menu_settings(t_menu *menu)
{
	SDL_Event	event;
	int			selected;
	int			running;
	int			use_wasd;

	if (!menu)
		return ;
	selected = 0;
	running = 1;
	load_settings(&use_wasd);
	while (running)
	{
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN
					&& event.key.keysym.sym == SDLK_ESCAPE))
				running = 0;
			else if (event.type == SDL_KEYDOWN)
			{
				if (event.key.keysym.sym == SDLK_UP)
				{
					selected--;
					if (selected < 0)
						selected = 2;
				}
				else if (event.key.keysym.sym == SDLK_DOWN)
				{
					selected++;
					if (selected > 2)
						selected = 0;
				}
				else if (event.key.keysym.sym == SDLK_RETURN)
				{
					if (selected == 0)
						use_wasd = !use_wasd;
					else if (selected == 1)
						save_settings(use_wasd);
					else if (selected == 2)
					{
						save_settings(use_wasd);
						running = 0;
					}
				}
			}
		}
		render_settings(menu, selected, use_wasd);
		SDL_Delay(16);
	}
}
