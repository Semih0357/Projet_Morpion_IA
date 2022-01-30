# Projet_Morpion_IA
Une machine qui apprend à jouer au morpion

Dans le cadre de notre première année de cycle d’ingénieur informatique à Sup Galilée, 
j'ai réalisé ce projet du devoir final de la matière de Structure de Données. 
L’objectif est d’utiliser nos acquis pendant les séances de cours pour manipuler des structures de données sur un jeu très populaire.

Mon but est d’implémenter un programme qui est capable d’apprendre à jouer au morpion par lui- même.
Le travail consiste à coder une intelligence artificielle basé sur un système de « récompenses » et de « punitions » dépendant de l’état final de la partie. 
J'ai utilisé une structure de donnée qui au fur et à mesure des parties, mémorise les coups gagnants à ce jeu.
Mis un algorithme en place permettant d’éduquer notre machine rien qu’en jouant aléatoirement en manipulant les informations contenues dans notre structure,
les affinées jusqu’à ce que l’ensemble sache jouer parfaitement au morpion.


Lors de l’exécution du programme vous serez face à ce menu ci-dessus.
Il vous suffit de vous aiguiller à l’aide des touches ‘1’, ‘2’ et ‘3’ pour accéder aux différentes fonctionnalités
Il très fortement déconseiller de taper autre chose que ces touches afin d’éviter toutes sortes de plantage du programme et tourner sur une boucle infinie. 
Cela empêchera la libération de la mémoire.
- Le mode de jeu 1 vous lancera une partie contre la machine, le joueur qui commence sera aléatoire.
- Le mode de jeu 2 vous demandera de rentrer un nombre n, pour lancer n parties contre elle même afin de s'entrainer.
- Le mode de jeu 3 permettra de quitter le jeu en sauvegardant les données de nos 592 grilles dans un
fichier de sortie.


Il me semble qu'au bout d'une centaine de partie, la machine devient imbattable et assure un match nul à tout les coups.



