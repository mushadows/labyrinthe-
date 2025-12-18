#include "game.h"
#include "menu.h"
#include <stdio.h>
#include <stdlib.h>

/*
** Lance une nouvelle partie
** @return EXIT_SUCCESS si le jeu s'est execute correctement, EXIT_FAILURE sinon
*/
static int	start_new_game(int difficulty)
{
	int		run;
	t_game	*game;

	printf("Chargement du jeu...\n");
	game = game_create();
	if (!game)
	{
		printf("Erreur: Impossible de créer le jeu\n");
		return (EXIT_FAILURE);
	}
	if (!game_init(game))
	{
		printf("Erreur: Impossible d'initialiser le jeu\n");
		game_destroy(game);
		return (EXIT_FAILURE);
	}
	if (!game_load_map(game, "maps/labyrinthe.txt"))
	{
		printf("Erreur: Impossible de charger la carte 'maps/labyrinthe.txt'\n");
		game_destroy(game);
		return (EXIT_FAILURE);
	}
	printf("Jeu initialisé avec succès!\n");
	printf("Contrôles:\n");
	printf("  - Flèches directionnelles ou WASD/ZQSD pour bouger\n");
	printf("  - F1 pour basculer entre WASD et ZQSD\n");
	printf("  - F5 pour sauvegarder\n");
	printf("  - Échap pour quitter\n");
	printf("\nLancement du jeu...\n");
	game->running = difficulty;
	run = game_run(game);
	printf("Fermeture du jeu...\n");
	game_destroy(game);
	return (run);
}

/*
** Charge une partie sauvegardee
** @return EXIT_SUCCESS si le jeu s'est execute correctement, EXIT_FAILURE sinon
*/
static int	load_saved_game(void)
{
	int		run;
	t_game	*game;

	printf("Chargement d'une partie sauvegardée...\n");
	printf("Chargement de la carte...\n");
	game = game_create();
	if (!game)
	{
		printf("Erreur: Impossible de créer le jeu\n");
		return (EXIT_FAILURE);
	}
	if (!game_init(game))
	{
		printf("Erreur: Impossible d'initialiser le jeu\n");
		game_destroy(game);
		return (EXIT_FAILURE);
	}
	if (!game_load_map(game, "maps/labyrinthe.txt"))
	{
		printf("Erreur: Impossible de charger la carte 'maps/labyrinthe.txt'\n");
		game_destroy(game);
		return (EXIT_FAILURE);
	}
	if (!game_load(game, "save.txt"))
	{
		printf("Erreur: Impossible de charger la sauvegarde\n");
		game_destroy(game);
		return (EXIT_FAILURE);
	}
	printf("Partie chargée avec succès!\n");
	printf("Contrôles:\n");
	printf("  - Flèches directionnelles ou WASD/ZQSD pour bouger\n");
	printf("  - F1 pour basculer entre WASD et ZQSD\n");
	printf("  - F5 pour sauvegarder\n");
	printf("  - Échap pour quitter\n");
	printf("\nLancement du jeu...\n");
	run = game_run(game);
	printf("Fermeture du jeu...\n");
	game_destroy(game);
	return (run);
}

/*
** Fonction principale du programme
** Affiche le menu principal et lance le jeu selon le choix de l'utilisateur
** @param argc: nombre d'arguments de la ligne de commande
** @param argv: tableau des arguments de la ligne de commande
** @return EXIT_SUCCESS si le jeu s'est execute correctement, EXIT_FAILURE sinon
*/
int	main(int argc, char *argv[])
{
	t_menu			*menu;
	t_menu_choice	choice;
	int				result;

	(void)argc;
	(void)argv;
	printf("=== Labyrinthe SDL2 ===\n");
	printf("Génération du labyrinthe pour le menu...\n");
	creer_labyrinthe();
	printf("Labyrinthe généré dans maps/labyrinthe.txt\n");
	menu = menu_create();
	if (!menu)
	{
		printf("Erreur: Impossible de créer le menu\n");
		return (EXIT_FAILURE);
	}
	if (!menu_init(menu))
	{
		printf("Erreur: Impossible d'initialiser le menu\n");
		menu_destroy(menu);
		return (EXIT_FAILURE);
	}
	result = 1;
	while (result)
	{
		choice = menu_run(menu);
		if (choice == MENU_PLAY)
		{
			menu_destroy(menu);
			result = start_new_game(1);
			while (result <= 2 && result != 0)
				result = start_new_game(result);
			result = EXIT_SUCCESS;
			break ;
		}
		else if (choice == MENU_LOAD)
		{
			menu_destroy(menu);
			result = load_saved_game();
			while (result <= 5 && result != 0)
				result = start_new_game(result);
			result = EXIT_SUCCESS;
			break ;
		}
		else if (choice == MENU_SETTINGS)
		{
			menu_settings(menu);
			menu->selected_option = 0;
			menu->running = 1;
		}
		else if (choice == MENU_QUIT)
		{
			printf("Au revoir!\n");
			result = EXIT_SUCCESS;
			break ;
		}
		else
			break ;
	}
	if (choice == MENU_SETTINGS || choice == MENU_QUIT)
		menu_destroy(menu);
	return (result);
}
