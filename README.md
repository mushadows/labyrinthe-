1. Base du jeu

Affichage du labyrinthe SDL

Déplacements du joueur via les touches ZQSD ou les flèches directionnelles.

Détection des collisions avec les murs et les limites de la carte.

Gestion des points de vie et de la position du joueur.

2. Génération et gestion du labyrinthe

Création aléatoire d’un labyrinthe (murs, sols, sortie).

Lecture et sauvegarde d’une carte depuis un fichier texte (.txt).

3. Gestion du joueur

Suivi de la position, des points de vie et de l’inventaire.

Détection de la victoire (atteinte de la sortie).

Détection de la défaite (piège déclenché).

4. Objets et pièges

Apparition aléatoire d’objets : clé, potion, trésor.

Apparition de pièges : piques, trous.

Interactions selon la case occupée : gain ou perte de points de vie, ramassage d’objets, ouverture de portes, etc.

5. Interface console

Menu principal : Jouer / Charger / Quitter.

Affichage du score, du nombre de déplacements et de l’état du joueur.

Option de sauvegarde et de chargement de la partie.

6. Bonus (optionnels)

Génération procédurale avancée du labyrinthe (algorithme de backtracking).

Présence d’un ennemi mobile doté d’une intelligence artificielle simple.

Système de niveaux progressifs (labyrinthes de taille croissante).

Améliorations :
- sous interface graphique avec SDL.
- ajout d'adversaires.
- ajout de téléporteurs, portes, clés (scriptable : fichiers ou plugins).
