# Makefile pour le projet Labyrinthe SDL2

# Variables
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
LIBS = -lSDL2 -lSDL2_ttf -lm
TARGET = labyrinthe
SRCDIR = src
OBJDIR = obj
SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

# Couleurs pour l'affichage
GREEN = \033[0;32m
YELLOW = \033[1;33m
RED = \033[0;31m
NC = \033[0m # No Color

# Règle principale
all: $(TARGET)

# Créer le répertoire obj s'il n'existe pas
$(OBJDIR):
	@echo "$(YELLOW)Création du répertoire $(OBJDIR)...$(NC)"
	@mkdir -p $(OBJDIR)

# Compilation du programme principal
$(TARGET): $(OBJDIR) $(OBJECTS)
	@echo "$(GREEN)Linking $(TARGET)...$(NC)"
	$(CC) $(OBJECTS) -o $(TARGET) $(LIBS)
	@echo "$(GREEN)Compilation terminée avec succès!$(NC)"
	@echo "$(YELLOW)Lancer avec: ./$(TARGET)$(NC)"

# Compilation des fichiers objets
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@echo "$(YELLOW)Compilation de $<...$(NC)"
	$(CC) $(CFLAGS) -c $< -o $@

# Nettoyer les fichiers compilés
clean:
	@echo "$(RED)Nettoyage des fichiers objets...$(NC)"
	rm -rf $(OBJDIR)
	@echo "$(GREEN)Nettoyage terminé.$(NC)"

# Nettoyer complètement
distclean: clean
	@echo "$(RED)Suppression de l'exécutable...$(NC)"
	rm -f $(TARGET)
	rm -f save.txt
	@echo "$(GREEN)Nettoyage complet terminé.$(NC)"

# Installation des dépendances (Ubuntu/Debian)
install-deps:
	@echo "$(YELLOW)Installation des dépendances SDL2...$(NC)"
	sudo apt update
	sudo apt install libsdl2-dev libsdl2-ttf-dev
	@echo "$(GREEN)Dépendances installées avec succès!$(NC)"

# Installation des dépendances (Arch Linux)
install-deps-arch:
	@echo "$(YELLOW)Installation des dépendances SDL2 (Arch Linux)...$(NC)"
	sudo pacman -S sdl2 sdl2_ttf
	@echo "$(GREEN)Dépendances installées avec succès!$(NC)"

# Installation des dépendances (macOS avec Homebrew)
install-deps-mac:
	@echo "$(YELLOW)Installation des dépendances SDL2 (macOS)...$(NC)"
	brew install sdl2 sdl2_ttf
	@echo "$(GREEN)Dépendances installées avec succès!$(NC)"

# Lancer le jeu
run: $(TARGET)
	@echo "$(GREEN)Lancement du jeu...$(NC)"
	./$(TARGET)

# Compilation en mode debug
debug: CFLAGS += -DDEBUG -O0
debug: $(TARGET)
	@echo "$(GREEN)Version debug compilée avec succès!$(NC)"

# Compilation en mode release
release: CFLAGS += -O2 -DNDEBUG
release: clean $(TARGET)
	@echo "$(GREEN)Version release compilée avec succès!$(NC)"

# Afficher l'aide
help:
	@echo "$(YELLOW)Makefile pour le projet Labyrinthe SDL2$(NC)"
	@echo ""
	@echo "$(GREEN)Commandes disponibles:$(NC)"
	@echo "  make              - Compiler le projet"
	@echo "  make run          - Compiler et lancer le jeu"
	@echo "  make clean        - Supprimer les fichiers objets"
	@echo "  make distclean    - Nettoyer complètement"
	@echo "  make debug        - Compiler en mode debug"
	@echo "  make release      - Compiler en mode release optimisé"
	@echo "  make install-deps - Installer les dépendances (Ubuntu/Debian)"
	@echo "  make install-deps-arch - Installer les dépendances (Arch Linux)"
	@echo "  make install-deps-mac  - Installer les dépendances (macOS)"
	@echo "  make help         - Afficher cette aide"
	@echo ""
	@echo "$(YELLOW)Prérequis:$(NC)"
	@echo "  - SDL2 development libraries"
	@echo "  - SDL2_ttf development libraries"
	@echo "  - GCC compiler"
	@echo ""
	@echo "$(YELLOW)Installation rapide des dépendances:$(NC)"
	@echo "  Ubuntu/Debian: sudo apt install libsdl2-dev libsdl2-ttf-dev"
	@echo "  Arch Linux:    sudo pacman -S sdl2 sdl2_ttf"
	@echo "  macOS:         brew install sdl2 sdl2_ttf"

# Vérifier si les dépendances sont installées
check-deps:
	@echo "$(YELLOW)Vérification des dépendances...$(NC)"
	@pkg-config --exists sdl2 && echo "$(GREEN)✓ SDL2 trouvé$(NC)" || echo "$(RED)✗ SDL2 manquant$(NC)"
	@pkg-config --exists SDL2_ttf && echo "$(GREEN)✓ SDL2_ttf trouvé$(NC)" || echo "$(RED)✗ SDL2_ttf manquant$(NC)"
	@which gcc > /dev/null && echo "$(GREEN)✓ GCC trouvé$(NC)" || echo "$(RED)✗ GCC manquant$(NC)"

# Créer un package de distribution
package: release
	@echo "$(YELLOW)Création du package de distribution...$(NC)"
	@mkdir -p dist/labyrinthe
	@cp $(TARGET) dist/labyrinthe/
	@cp README.md dist/labyrinthe/
	@cp -r ressources dist/labyrinthe/ 2>/dev/null || true
	@cd dist && tar -czf labyrinthe-sdl2.tar.gz labyrinthe
	@echo "$(GREEN)Package créé: dist/labyrinthe-sdl2.tar.gz$(NC)"

.PHONY: all clean distclean install-deps install-deps-arch install-deps-mac run debug release help check-deps package