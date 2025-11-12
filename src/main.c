#include "game.h"
#include <stdio.h>
#include <stdlib.h>

/*
** Fonction principale du programme
** Initialise le jeu, charge la carte et lance la boucle principale
** @param argc: nombre d'arguments de la ligne de commande
** @param argv: tableau des arguments de la ligne de commande
** @return EXIT_SUCCESS si le jeu s'est execute correctement, EXIT_FAILURE sinon
*/
int	main(int argc, char *argv[])
{
	t_game	*game;

	(void)argc;
	(void)argv;
	printf("=== Labyrinthe SDL2 ===\n");
	printf("Génération d'un nouveau labyrinthe...\n");
	creer_labyrinthe();
	printf("Labyrinthe généré dans maps/labyrinthe.txt\n");
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
	printf("  - Flèches directionnelles ou WASD pour bouger\n");
	printf("  - Échap pour quitter\n");
	printf("  - Fermer la fenêtre pour quitter\n");
	printf("\nLancement du jeu...\n");
	game_run(game);
	printf("Fermeture du jeu...\n");
	game_destroy(game);
	printf("Au revoir!\n");
	return (EXIT_SUCCESS);
}
