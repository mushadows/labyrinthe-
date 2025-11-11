#include "../lab.h"

int main()
{
	t_player	*user;
	t_position	tp;

	user = create_player();
	printf("user have %d PV, and start at position (%d,%d)\n", user->life, user->position.x, user->position.y);
	printf("\nadd 10 PV with 5 PV limit\n");
	add_life(user, 10, 5);
	printf("user have %d PV, and position (%d,%d)\n", user->life, user->position.x, user->position.y);
	printf("\nadd 10 PV with no limit\n");
	add_life(user, 10, -1);
	printf("user have %d PV, and position (%d,%d)\n", user->life, user->position.x, user->position.y);
	tp.x = 5;
	tp.y = 10;
	printf("\nteleport user at (%d,%d)\n", tp.x, tp.y);
	tp_player(user, tp);
	printf("user have %d PV, and position (%d,%d)\n", user->life, user->position.x, user->position.y);
	printf("\ndeplace the user to Left and Down\n");
	player_move_left(user);
	player_move_down(user);
	printf("user have %d PV, and position (%d,%d)\n", user->life, user->position.x, user->position.y);
	printf("\ndeplace the user to Right and Up\n");
	player_move_right(user);
	player_move_up(user);
	printf("user have %d PV, and position (%d,%d)\n", user->life, user->position.x, user->position.y);
	free(user);
	return 0;
}

