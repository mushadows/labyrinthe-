#include "../lab.h"

int main()
{
	t_position	position;

	printf("\nAbuses tu dois vraiment tester ca ?\n");
	position = coordonnee_to_position(5, 10);
	printf("position (%d,%d)\n", position.x, position.y);
	return 0;
}
