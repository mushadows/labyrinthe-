#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "map.h"

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
	map->grid = (char **)malloc(height * sizeof(char*));
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
