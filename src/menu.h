#ifndef MENU_H
# define MENU_H

# include <SDL2/SDL.h>
# include "map.h"
# include "utils.h"

/* Types de choix du menu */
typedef enum e_menu_choice
{
	MENU_PLAY,
	MENU_LOAD,
	MENU_SETTINGS,
	MENU_QUIT,
	MENU_NONE
}	t_menu_choice;

/* Structure du menu */
typedef struct s_menu
{
	SDL_Window		*window;
	SDL_Renderer	*renderer;
	int				selected_option;
	int				running;
	t_map			*background_map;
	t_textures		*textures;
}	t_menu;

/* Fonctions du menu */
t_menu			*menu_create(void);
void			menu_destroy(t_menu *menu);
int				menu_init(t_menu *menu);
void			menu_cleanup(t_menu *menu);
void			menu_handle_events(t_menu *menu, t_menu_choice *choice);
void			menu_render(t_menu *menu);
t_menu_choice	menu_run(t_menu *menu);

/* Menu de parametres */
void			menu_settings(t_menu *menu);

#endif
