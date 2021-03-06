# Opérations disponibles

         | opérandes | résultat
 symbole | dépilées  | empilé
---------|-----------|-------------------------------
 nombre  |     -     | le nombre naturel entré
    ,    |     -     | un nombre naturel entré sur l'entrée standard
    `    |     -     | un code ASCII lu depuis un char sur l'entrée
    @    |     -     | une copie du dessus de la pile
    +    |     2     | la somme des deux opérandes
    -    |     2     | la différence des deux opérandes
    *    |     2     | le produit des deux opérandes (méthode de Karatsuba)
   ==    |     2     | 1 si les opérandes sont égales, 0 sinon
    >    |     2     | 1 si op1 > op2
    <    |     2     | 1 si op1 < op2
    &    |     2     | 1 si op1 != 0 *et* op2 != 0
   &#124;|     2     | 1 si op1 != 0 *ou* op2 != 0
    !    |     1     | la négation binaire du dessus de la pile
    ?    |     3     | op1 si op3 != 0, op2 sinon
    {    |     1     | op1 << 1 (les shifts sont en base 10)
    }    |     1     | op1 >> 1

# Autres commandes

 symbole | effet
---------|-------------------------------------------
    .    | pop le dessus de la pile
  "..."  | copie le texte entre " sur la sortie standard
         |      (\" affiche ", \\ affiche \)
    ~    | vide la pile
    $    | dump le contenu de la pile
    %    | dump les adresses contenues dans les variables
    ^    | affiche le dessus de la pile
    '    | renverse la pile
    \    | affiche le charactère ASCII ayant le code du dessus de la pile

# Variables

   =a : assigne le dessus de la pile dans la variable a
   _a : assigne NULL dans la variable a
    a : push le contenu de la variable a sur la pile

Les variables disponibles sont les lettres a à z

# Procédures

   :A expressions; : assigne les expressions de A à ; exclusivement à la procédure A
    A              : appelle la procédure A.

# Structure de contrôle

La seule structure de contrôle est la boucle :

        20 =a .
        10 [
            a 10 + =a .
            1 -
        ]

Le contenu entre crochets est exécuté tant que le top de la pile existe et est
différent de zéro.

Pour obtenir un bloc conditionnel, il suffit de boucler exactement une fois :

        ...
        a 100 < [
            "Vous avez entré un nombre inférieur à 100 !" . 0
        ] .
