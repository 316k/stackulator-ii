# Concept de langages de programmation : devoir 1

Stackulator : la calculatrice à pile du futur

par Nicolas Hurtubise et Guillaume Riou

## Explications générales

Pour ce devoir, nous avons implanté une calculatrice à pile comprenant les
instructions de base demandées telles que l'addition, la multiplication et la
soustraction ainsi que le storage de variables. Nous avons également implanté
quelques fonctionnalités supplémentaires utiles, telles que des comparaisons
booléennes (`!`, `<`, `>`, `==`), des shifts arithmétiques en base 10
(`}` et `{`) et une opération pour renverser la pile (`'`).

Nous avons aussi implanté un système de boucles (délimitées par `[` et `]`)
ainsi que des procédures nommées avec les lettres majuscules allant de A à Z.

Lancé sans arguments, `stackulator` se comporte de la manière
décrite dans l'énoncé du devoir, c'est à dire qu'il affiche '>' en attendant
une entrée et qu'il affiche le dessus de la pile après avoir reçu et exécuté les
instructions d'une ligne.

Pour rendre l'utilisation plus conviviale, nous avons ajouté quelques opérations :

    -s, --silence : mode silencieux, n'affiche pas de caractère de "prompt"
        Le mode silencieux est fait pour être utilisé en conjonction avec un fichier
        de code `stackulator` (généralement des `.skii`) en argument.
    -v, --verbose : mode verbeux qui affiche `x:(y)>` en attendant une entrée
        (où x est la hauteur de la pile et y est le nombre sur le dessus).
    -h, --help : permet d'avoir un résumé des fonctionnalités possibles.

#### Détails d'implantation

a) à continuer...

Durant le développement de stackulator nous avons dû faire plusieurs choix de développement pour nous assurer que le programme utilises bien la mémoire et se comporte correctement. Dans notre implantation de la machine à pile, les nombres sont représentés sous la forme d'une liste chainée de nombres en base 10 ainsi qu'un char représentant le signe. 

b) :analyse et calcul de la réponse.
expliquer vaguement le contenu de stackulator.c I guess.

c) : Gestion de la mémoire
Comptage de référence pour les bignums et free() le plus de choses que l'on peut.

d) : expliquer les algos pour les ops

e) : c'est quoi une erreur ???

#### ANNEXES :

Mettre help.md, l'interpréteur de brainfuck et la preuve que brainfuck est turing complete.
