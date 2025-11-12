#ifndef POSITION_H
# define POSITION_H

typedef struct s_position
{
	int	x;
	int	y;
}		t_position;

t_position	coordonnee_to_position(int x, int y);

#endif