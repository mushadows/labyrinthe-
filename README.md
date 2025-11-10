# Labyrinthe SDL

Un jeu de labyrinthe en 2D avec gestion du joueur, objets interactifs, pièges et interface console, développé en C avec (SDL).

## Description

Ce projet est un jeu de labyrinthe où le joueur doit naviguer à travers un labyrinthe généré aléatoirement, éviter les pièges, collecter des objets et atteindre la sortie pour gagner.

## Fonctionnalités

### 1. Base du jeu
- **Affichage du labyrinthe** : Rendu graphique du labyrinthe via SDL.
- **Contrôles du joueur** : Déplacements avec les touches **ZQSD**/**wASD** ou les **flèches directionnelles**.
- **Collisions** : Détection des collisions avec les murs et les limites de la carte.
- **Gestion du joueur** : Suivi des **points de vie** et de la **position** du joueur.

### 2. Génération et gestion du labyrinthe
- **Génération aléatoire** : Création d'un labyrinthe avec des murs, des sols et une sortie.
- **Sauvegarde/Chargement** : Lecture et écriture de cartes via fichier texte (`.txt`).

### 3. Gestion du joueur
- **Suivi de l'état** : Position, points de vie et inventaire du joueur.
- **Conditions de fin** :
  - **Victoire** : Atteinte de la sortie.
  - **Défaite** : Aucun point de vie restant.

### 4. Objets et pièges
- **Objets aléatoires** : Clés, potions, trésors.
- **Pièges** : Piques, trous.
- **Interactions** :
  - Gain/perte de points de vie.
  - Ramassage d'objets.
  - Ouverture de portes avec des clés.

### 5. Interface console
- **Menu principal** : Options pour **Jouer**, **Charger une partie** ou **Quitter**.
- **Affichage en temps réel** : Score, nombre de déplacements, état du joueur.
- **Sauvegarde/Chargement** : Possibilité de sauvegarder et charger une partie en cours.

### 6. Bonus (optionnels)
- **Génération procédurale avancée** : Algorithme de backtracking pour des labyrinthes plus complexes.
- **Ennemi mobile** : IA simple pour un ennemi qui se déplace dans le labyrinthe.
- **Système de niveaux** : Labyrinthes de taille croissante.
- **Améliorations graphiques** : Interface SDL plus élaborée.
- **Adversaires supplémentaires** : Ajout d'ennemis.
- **Téléporteurs et portes** : Mécaniques scriptables via des fichiers ou plugins.

## Prérequis

- **SDL2** : Bibliothèque graphique pour l'affichage.

## Installation

```bash
git clone https://github.com/votre-utilisateur/labyrinthe-sdl.git
cd labyrinthe-sdl
```
