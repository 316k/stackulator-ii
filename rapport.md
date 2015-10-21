#Concept de langages de programmation : devoir 1
##Stackulator
### Nicolas Hurtubise, Guillaume Riou
####Explications générales
Pour ce devoir, nous avons implantés une calculatrice à pile comprenant les instructions de base demandées telles que l'addition, la multiplication et la soustraction ainsi que le storage de variables. Nous avons aussi implantés plusieurs fonctionnalités non demandées telles que des comparaisons booléennes, des shifts arithmétiques en base 10, des méthodes pour afficher le contenu de la pile et des variables, une manière de lire et d'afficher des caractères et une manière de renverser la pile. Nous avons aussi implanté un système de boucles et de procédures storées dans les variables allant de A à Z. 

Lorsque notre programme est lancé sans arguments, il se comporte de la manière décrite selon l'énoncé du devoir, c'est à dire qu'il affiche '>' en attendant une entrée et qu'il affiche le dessus de la pile après avoir reçu et exécuté les instructions. Nous avons aussi ajoutés des arguments possibles à notre programme. Ainsi, il est possible de lancer stackulator en mode silencieux de manière à ce que celui-ci n'affiche pas. Utilisé en conjonction avec un fichier de code en argument, ce mode est pratique pour que le programme n'affiche que ce que l'on veut. Nous avons aussi inclus un mode verbeux qui affiche x:(y)> lorsqu'il attend une entrée où x est la hauteur de la pile et y est le nombre sur le dessus. L'argument -t permet de faire rouler les tests unitaires et l'argument --help permet d'avoir un résumé des fonctionnalités possibles.

####Détails d'implantation.

a) à continuer... 
Durant le développement de stackulator nous avons du faire plusieurs choix de développement pour nous assurer que le programme utilises bien la mémoire et se comporte correctement. Dans notre implantation de la machine à pile, les nombres sont représentés sous la forme d'une liste chainée de nombres en base 10 ainsi qu'un char représentant le signe. 

b) :analyse et calcul de la réponse.
expliquer vaguement le contenu de stackulator.c I guess.

c) : Gestion de la mémoire
Comptage de référence pour les bignums et free() le plus de choses que l'on peut.

d) : expliquer les algos pour les ops

e) : c'est quoi une erreur ???

####ANNEXES :

Mettre help.md, l'interpréteur de brainfuck et la preuve que brainfuck est turing complete.
