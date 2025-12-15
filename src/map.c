#include "map.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

/*
** Cree une nouvelle structure de carte
** Alloue la memoire et initialise les valeurs par defaut
** @return pointeur vers la structure t_map creee
*/
t_map	*map_create(void)
{
	t_map	*map;

	map = (t_map *)malloc(sizeof(t_map));
	assert(map != NULL);
	map->grid = NULL;
	map->width = 0;
	map->height = 0;
	map->player_x = -1;
	map->player_y = -1;
	return (map);
}

/*
** Detruit une structure de carte et libere la memoire
** Libere la grille et la structure elle-meme
** @param map: pointeur vers la structure de carte a detruire
*/
void	map_destroy(t_map *map)
{
	int	i;

	if (!map)
		return ;
	if (map->grid)
	{
		i = 0;
		while (i < map->height)
		{
			free(map->grid[i]);
			i++;
		}
		free(map->grid);
	}
	free(map);
}

/*
** Compte les dimensions d'un fichier de carte
** Lit le fichier pour determiner la largeur et hauteur maximales
** @param filename: nom du fichier a analyser
** @param height: pointeur pour stocker la hauteur
** @param width: pointeur pour stocker la largeur
** @return 1 en cas de succes, 0 en cas d'erreur
*/
static int	map_count_dimensions(const char *filename, int *height, int *width)
{
	FILE	*file;
	char	buffer[256];
	int		line_length;

	file = fopen(filename, "r");
	if (!file)
	{
		printf("Erreur: Impossible d'ouvrir le fichier %s\n", filename);
		return (0);
	}
	*height = 0;
	*width = 0;
	while (fgets(buffer, sizeof(buffer), file))
	{
		line_length = strlen(buffer);
		if (line_length > 0 && buffer[line_length - 1] == '\n')
		{
			buffer[line_length - 1] = '\0';
			line_length--;
		}
		if (line_length > *width)
			*width = line_length;
		(*height)++;
	}
	fclose(file);
	return (1);
}

/*
** Alloue la grille 2D pour la carte
** @param map: pointeur vers la structure de carte
** @param height: hauteur de la grille a allouer
** @param width: largeur de la grille a allouer
** @return 1 en cas de succes, 0 en cas d'erreur d'allocation
*/
static int	map_allocate_grid(t_map *map, int height, int width)
{
	int	i;

	map->width = width;
	map->height = height;
	map->grid = (char **)malloc(height * sizeof(char *));
	assert(map->grid != NULL);
	i = 0;
	while (i < height)
	{
		map->grid[i] = (char *)malloc((width + 1) * sizeof(char));
		assert(map->grid[i] != NULL);
		if (!map->grid[i])
		{
			while (--i >= 0)
				free(map->grid[i]);
			free(map->grid);
			return (0);
		}
		i++;
	}
	return (1);
}

/*
** Remplit une ligne de la grille avec les donnees du buffer
** Gere le padding avec des espaces si necessaire et detecte la position du joueur
** @param map: pointeur vers la structure de carte
** @param buffer: ligne lue depuis le fichier
** @param row: numero de la ligne a remplir
*/
static void	map_fill_row(t_map *map, char *buffer, int row)
{
	int	line_length;
	int	col;

	line_length = strlen(buffer);
	if (line_length > 0 && buffer[line_length - 1] == '\n')
	{
		buffer[line_length - 1] = '\0';
		line_length--;
	}
	strncpy(map->grid[row], buffer, line_length);
	col = line_length;
	while (col < map->width)
	{
		map->grid[row][col] = ' ';
		col++;
	}
	map->grid[row][map->width] = '\0';
	if (map->player_x == -1 && map->player_y == -1)
	{
		col = 0;
		while (col < line_length)
		{
			if (buffer[col] == ' ')
			{
				map->player_x = col;
				map->player_y = row;
				break ;
			}
			col++;
		}
	}
}

/*
** Charge une carte depuis un fichier texte
** Lit le fichier, alloue la grille et remplit les donnees
** @param map: pointeur vers la structure de carte
** @param filename: nom du fichier contenant la carte
** @return 1 en cas de succes, 0 en cas d'erreur
*/
int	map_load_from_file(t_map *map, const char *filename)
{
	FILE	*file;
	char	buffer[256];
	int		height;
	int		width;
	int		row;

	if (!map_count_dimensions(filename, &height, &width))
		return (0);
	if (!map_allocate_grid(map, height, width))
		return (0);
	file = fopen(filename, "r");
	if (!file)
		return (0);
	row = 0;
	while (fgets(buffer, sizeof(buffer), file) && row < height)
	{
		map_fill_row(map, buffer, row);
		row++;
	}
	fclose(file);
	return (1);
}

/*
** Recupere le type de cellule a une position donnee
** @param map: pointeur vers la structure de carte
** @param x: coordonnee X
** @param y: coordonnee Y
** @return type de cellule ou CELL_WALL si position invalide
*/
t_cellType	map_get_cell(const t_map *map, int x, int y)
{
	if (!map || x < 0 || x >= map->width || y < 0 || y >= map->height)
		return (CELL_WALL);
	return ((t_cellType)map->grid[y][x]);
}

/*
** Definit le type d'une cellule a une position donnee
** @param map: pointeur vers la structure de carte
** @param x: coordonnee X
** @param y: coordonnee Y
** @param cell: nouveau type de cellule
*/
void	map_set_cell(t_map *map, int x, int y, t_cellType cell)
{
	if (!map || x < 0 || x >= map->width || y < 0 || y >= map->height)
		return ;
	map->grid[y][x] = (char)cell;
}

/*
** Definit la position du joueur sur la carte
** @param map: pointeur vers la structure de carte
** @param x: nouvelle coordonnee X du joueur
** @param y: nouvelle coordonnee Y du joueur
*/
void	map_set_player_position(t_map *map, int x, int y)
{
	if (!map)
		return ;
	map->player_x = x;
	map->player_y = y;
}

/*
** Verifie si une position est valide pour le deplacement
** Une position est valide si elle n'est pas un mur et dans les limites
** @param map: pointeur vers la structure de carte
** @param x: coordonnee X a verifier
** @param y: coordonnee Y a verifier
** @return 1 si la position est valide, 0 sinon
*/
int	map_is_valid_position(const t_map *map, int x, int y)
{
	if (!map || x < 0 || x >= map->width || y < 0 || y >= map->height)
		return (0);
	return (map->grid[y][x] != CELL_WALL);
}

/*
** Retourne la largeur de la carte
** @param map: pointeur vers la structure de carte
** @return largeur de la carte, 0 si map est NULL
*/
int	map_get_width(const t_map *map)
{
	if (!map)
		return (0);
	return (map->width);
}

/*
** Retourne la hauteur de la carte
** @param map: pointeur vers la structure de carte
** @return hauteur de la carte, 0 si map est NULL
*/
int	map_get_height(const t_map *map)
{
	if (!map)
		return (0);
	return (map->height);
}

/*
** Retourne la position X du joueur
** @param map: pointeur vers la structure de carte
** @return position X du joueur, -1 si map est NULL
*/
int	map_get_player_x(const t_map *map)
{
	if (!map)
		return (-1);
	return (map->player_x);
}

/*
** Retourne la position Y du joueur
** @param map: pointeur vers la structure de carte
** @return position Y du joueur, -1 si map est NULL
*/
int	map_get_player_y(const t_map *map)
{
	if (!map)
		return (-1);
	return (map->player_y);
}

/*
** ------------------------------------------------------------------
** Fonctions de generation et sauvegarde de labyrinthe
** (deplacees depuis utils.c pour centraliser la logique dans map.c)
** ------------------------------------------------------------------
*/

/*
** Initialise la grille de directions pour la generation de labyrinthe
** Remplit la grille avec des directions par defaut
** @param grille: tableau 2D de directions a initialiser
*/
void	map_init(t_Direction grille[TAILLE][TAILLE])
{
	int	i;
	int	j;

	i = 0;
	while (i < TAILLE)
	{
		j = 0;
		while (j < TAILLE - 1)
		{
			grille[i][j].direction = 'D';
			j++;
		}
		i++;
	}
	i = 0;
	while (i < TAILLE - 1)
	{
		grille[i][TAILLE - 1].direction = 'B';
		i++;
	}
	grille[TAILLE - 1][TAILLE - 1].direction = 'N';
}

/*
** Genere un labyrinthe aleatoire en utilisant une marche aleatoire
** Parcourt la grille de maniere aleatoire pour creer des chemins
** @param grille: tableau 2D de directions a remplir
*/
void	generer(t_Direction grille[TAILLE][TAILLE])
{
	int		origine_i;
	int		origine_j;
	int		nb_iterations;
	char	voisins[4];
	int		nb_voisins;
	int		index;
	int		iter;
	char	voisin_choisi;

	srand(time(NULL));
	origine_i = TAILLE - 1;
	origine_j = TAILLE - 1;
	nb_iterations = TAILLE * TAILLE * 10;
	iter = 0;
	while (iter < nb_iterations)
	{
		nb_voisins = 0;
		if (origine_i > 0)
			voisins[nb_voisins++] = 'H';
		if (origine_i < TAILLE - 1)
			voisins[nb_voisins++] = 'B';
		if (origine_j > 0)
			voisins[nb_voisins++] = 'G';
		if (origine_j < TAILLE - 1)
			voisins[nb_voisins++] = 'D';
		index = rand() % nb_voisins;
		voisin_choisi = voisins[index];
		grille[origine_i][origine_j].direction = voisin_choisi;
		if (voisin_choisi == 'H')
			origine_i--;
		else if (voisin_choisi == 'B')
			origine_i++;
		else if (voisin_choisi == 'G')
			origine_j--;
		else if (voisin_choisi == 'D')
			origine_j++;
		grille[origine_i][origine_j].direction = 'N';
		iter++;
	}
}

/*
** Traite une direction pour creer les passages dans le labyrinthe final
** Modifie le tableau final en fonction de la direction specifiee
** @param tab_final: tableau 2D representant le labyrinthe final
** @param start_i: coordonnee I de depart
** @param start_j: coordonnee J de depart
** @param direction: direction a traiter (H/B/G/D)
*/
static void	process_direction(char tab_final[TAILLE * GEN_CELL_SIZE]\
[TAILLE * GEN_CELL_SIZE], int start_i, int start_j, char direction)
{
	if (direction == 'H')
	{
		tab_final[start_i][start_j + 1] = ' ';
		tab_final[start_i][start_j + 2] = ' ';
		tab_final[start_i - 1][start_j + 1] = ' ';
		tab_final[start_i - 1][start_j + 2] = ' ';
		tab_final[start_i - 2][start_j + 1] = ' ';
		tab_final[start_i - 2][start_j + 2] = ' ';
	}
	else if (direction == 'B')
	{
		tab_final[start_i + 3][start_j + 1] = ' ';
		tab_final[start_i + 3][start_j + 2] = ' ';
		tab_final[start_i + 4][start_j + 1] = ' ';
		tab_final[start_i + 4][start_j + 2] = ' ';
	}
	else if (direction == 'G')
	{
		tab_final[start_i + 1][start_j] = ' ';
		tab_final[start_i + 2][start_j] = ' ';
		tab_final[start_i + 1][start_j - 1] = ' ';
		tab_final[start_i + 2][start_j - 1] = ' ';
		tab_final[start_i + 1][start_j - 2] = ' ';
		tab_final[start_i + 2][start_j - 2] = ' ';
	}
	else if (direction == 'D')
	{
		tab_final[start_i + 1][start_j + 3] = ' ';
		tab_final[start_i + 2][start_j + 3] = ' ';
		tab_final[start_i + 1][start_j + 4] = ' ';
		tab_final[start_i + 2][start_j + 4] = ' ';
	}
}

/*
** Sauvegarde le labyrinthe genere dans un fichier texte
** Convertit la grille de directions en un labyrinthe ASCII et l'ecrit
** @param grille: tableau 2D de directions representant le labyrinthe
*/
void	sauver_labyrinthe(t_Direction grille[TAILLE][TAILLE])
{
	int		h;
	int		w;
	char	tab_final[TAILLE * GEN_CELL_SIZE][TAILLE * GEN_CELL_SIZE];
	FILE	*f;
	int		i;
	int		j;

	h = TAILLE * GEN_CELL_SIZE;
	w = TAILLE * GEN_CELL_SIZE;
	i = 0;
	while (i < h)
	{
		j = 0;
		while (j < w)
		{
			tab_final[i][j] = '#';
			j++;
		}
		i++;
	}
	i = 0;
	while (i < TAILLE)
	{
		j = 0;
		while (j < TAILLE)
		{
			tab_final[(i * GEN_CELL_SIZE) + 1][(j * GEN_CELL_SIZE) + 1] = ' ';
			tab_final[(i * GEN_CELL_SIZE) + 1][(j * GEN_CELL_SIZE) + 2] = ' ';
			tab_final[(i * GEN_CELL_SIZE) + 2][(j * GEN_CELL_SIZE) + 1] = ' ';
			tab_final[(i * GEN_CELL_SIZE) + 2][(j * GEN_CELL_SIZE) + 2] = ' ';
			process_direction(tab_final, i * GEN_CELL_SIZE, j * GEN_CELL_SIZE, grille[i][j].direction);
			j++;
		}
		i++;
	}
	tab_final[h - 2][w - 2] = 'E';
	f = fopen("maps/labyrinthe.txt", "w");
	if (!f)
	{
		printf("impossible d'ouvrir le fichier dommage...\n");
		return ;
	}
	i = 0;
	while (i < h)
	{
		j = 0;
		while (j < w)
		{
			fputc(tab_final[i][j], f);
			j++;
		}
		fputc('\n', f);
		i++;
	}
	fclose(f);
}

/*
** Cree un nouveau labyrinthe complet
** Initialise, genere et sauvegarde un labyrinthe dans un fichier
*/
void	creer_labyrinthe(void)
{
	t_Direction		grille[TAILLE][TAILLE];

	map_init(grille);
	generer(grille);
	sauver_labyrinthe(grille);
}
