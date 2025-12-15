NAME		= labyrinthe
CC			= gcc

WFLAGS		= -Wall -Werror -Wextra
SDLFLAGS	= -lSDL2 -lSDL2_image
FLAGS		= -std=c99 -lm
DEBUGFLAG	= -g

SRCDIR		= src
SRC			= $(wildcard $(SRCDIR)/*.c)
OBJ			= $(SRC:.c=.o)
OBJ_NO_MAIN	= $(filter-out $(SRCDIR)/main.o, $(OBJ))
RM			= rm -f

$(NAME):	$(OBJ)
	$(CC) $(WFLAGS) $(FLAGS) $^ $(SDLFLAGS) -o $(NAME)


all:		$(NAME)

debug:		$(OBJ)
	$(CC) $(WFLAGS) $(DEBUGFLAG) $(SDLFLAGS) $(FLAGS) $^ -o $(NAME)


.c.o:
	$(CC) $(WFLAGS) -c $< -o $(<:.c=.o)

clean:
	$(RM) $(OBJ)

fclean: 	clean
	$(RM) $(NAME)
	$(RM) test_*

re:		fclean all

run:		$(NAME)
	./$(NAME)

run-debug:	debug
	./$(NAME)

test/%:		$(OBJ_NO_MAIN) test/%.c
	$(CC) $(WFLAGS) $(SDLFLAGS) $(DEBUGFLAG) $^ -o $*
	./$*

help:
	@echo "  make              - Compiler le projet"
	@echo "  make run          - Compiler et lancer le jeu"
	@echo "  make clean        - Supprimer les fichiers objets"
	@echo "  make fclean       - Nettoyer complètement"
	@echo "  make debug        - Compiler en mode debug"
	@echo "  make run-debug    - Compiler et lancer le jeu"
	@echo "  make help         - Afficher cette aide"
	@echo "  make test/<name>  - Test le fichier <name> (ne pas mettre l'extension)"
	@echo ""
	@echo "Prérequis:"
	@echo "  - SDL2 development libraries"
	@echo "  - SDL2_ttf development libraries"
	@echo "  - GCC compiler"
