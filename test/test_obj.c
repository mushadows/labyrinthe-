#include "../lab.h"

int main()
{
	t_player	*user;
	t_obj		*obj;

	user = create_player();
	printf("user have %d PV, and start at position (%d,%d)\n", user->life, user->position.x, user->position.y);
	obj = create_obj('P', 15, coordonnee_to_position(5, 10));
	printf("\nCreate potion %C (%d PV) at (%d,%d)\n", obj->name, obj->effect, obj->position.x, obj->position.y);
	printf("\nuser use the potion...\n");
	use_obj(user, *obj, -1);
	printf("user have %d PV, and position (%d,%d)\n", user->life, user->position.x, user->position.y);
	free(user);
	free(obj);
	return 0;
}
