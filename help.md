# Opérations disponibles

         | opérandes | résultat
 symbole | dépilées  | empilé
---------+-----------+-------------------------------
 nombre  |     -     | le nombre naturel entré
    ,    |     -     | un nombre naturel entré sur l'entrée standard
    @    |     -     | une copie du dessus de la pile
    +    |     2     | la somme des deux opérandes
    -    |     2     | la différence des deux opérandes
    *    |     2     | le produit des deux opérandes (méthode de Karatsuba)
    !    |     2     | le produit des deux opérandes (par additions successives)
   ==    |     2     | 1 si les opérandes sont égales, 0 sinon
    >    |     2     | 1 si op1 > op2
    <    |     2     | 1 si op1 < op2
    &    |     2     | 1 si op1 != 0 *et* op2 != 0
    |    |     2     | 1 si op1 != 0 *ou* op2 != 0
    ?    |     3     | op1 si op3 != 0, op2 sinon

# Autres commandes

 symbole | effet
---------+-------------------------------------------
    .    | pop le dessus de la pile
  "..."  | copie le texte entre " sur la sortie standard
         |      (\" affiche ", \\ affiche \)
    ~    | vide la pile
    $    | dump le contenu de la pile
    %    | dump les adresses contenues dans les variables
    ^    | affiche le dessus du stack

# Variables

   =a : assigne le dessus de la pile dans la variable a
   _a : assigne NULL dans la variable a
    a : push le contenu de la variable a sur la pile

Les variables disponibles sont les lettres a à z

# Procédures

   :A expressions; : assigne les expressions de A à ; exclusivement à la procédure A
    A              : appelle la procédure A.

# Structure de contrôle

La seule structure de contrôle est la boucle : `20 =a . 10 [a 10 + =a . 1 - ]`

Le contenu entre crochets est exécuté tant que le top de la pile n'est pas 0 rendu à ']'
