#include "position.h"

t_position	coordonnee_to_position(int x, int y)
{
	t_position position;

	position.x = x;
	position.y = y;
	return (position);
}