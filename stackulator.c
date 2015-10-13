#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "help.c"
#include "context.c"
#include "context_stack.c"
#include "bignum.c"
#include "stack.c"
#include "test.c"


void prompt(char interactive_mode, stack* s) {
    if(interactive_mode == 1) {
        printf("> ");
    } else if(interactive_mode == 2) {
        if(stack_empty(s)) {
            printf("0:(nil)> ");
        } else {
            char* str = bignum_tostr(*stack_peek(s));
            printf("%ld:%s> ", stack_len(s), str);
            free(str);
        }
    }
}

/**
 * Donne la prochaine instruction à exécuter
 */
char get_next(FILE* in, context_stack* c_s, char* saved){
    /* Dans le cas ou saved est NULL, on lit l'entrée de l'utilisateur et
    il est impossible qu'il y ait de char sauvés.*/
    if(saved != NULL && *saved != -1){
        char val = *saved;
        *saved = -1;
        return val;
    }
    if(context_stack_empty(c_s)){
        return getc(in);
    }

    return context_next(context_stack_peek(c_s));
}

char push_number(char c, stack* s, FILE* in, context_stack* c_s, char* saved) {
    bignum* num = bignum_init();

    bigdigit* digit = NULL;
    bigdigit* prev_addr = NULL;

    // Si ça commence avec 0, on veut potentiellement garder le premier seulement
    if(c == '0') {
        digit = bigdigit_init();

        prev_addr = digit;

        while((c = get_next(in, c_s, saved)) == '0');

        // Si ça commençait avec des 0 non-significatifs, on les flush
        if(c >= '1' && c <= '9') {
            digit->value = c - '0';
            c = get_next(in, c_s, saved);
        }
    }

    while(c >= '0' && c <= '9') {

        digit = bigdigit_init();

        digit->next = prev_addr;
        digit->value = c - '0';

        prev_addr = digit;

        c = get_next(in, c_s, saved);
    }

    /* Si c'est un autre char que space et newline, sauve le pour la
       prochaine évaluation. Si saved est NULL, l'entrée utilisateur
       est lu et pour éviter l'injection de code, on ne sauve pas 
       le dernier char.*/
    if(c != ' ' && c != '\n' && saved != NULL) {
        *saved = c;
    }

    num->first = prev_addr;

    stack_push(s, num);

    return c;
}

char push_op(char c, stack* s, bignum* (*fct)(bignum, bignum)) {
    if(stack_len(s) < 2) {
        fprintf(stderr, "%c nécessite deux opérandes, taille du stack insuffisante\n", c);
        return c;
    }

    bignum* a = stack_pop(s);
    bignum* b = stack_pop(s);

    bignum* num = (*fct)(*b, *a);
    stack_push(s, num);

    bignum_destoroyah(a);
    bignum_destoroyah(b);

    return c;
}

void push_shift(char c, stack* s) {
    if(stack_len(s) < 1) {
        fprintf(stderr, "%c nécessite une opérandes, taille du stack insuffisante\n", c);
        return;
    }
    bignum* popped = stack_pop(s);
    bignum* shifted = bignum_copy(popped);
    if(c == '{') {
        bignum_shift_left(shifted, 1);
    } else {
        bignum_shift_right(shifted, 1);
    }
    stack_push(s, shifted);
    bignum_destoroyah(popped);
}

char push_test(char c, stack* s) {
    if(stack_len(s) < 2) {
        fprintf(stderr, "Une comparaison booléenne nécessite deux opérandes, taille du stack insuffisante\n");
        return c;
    }

    bignum* a = stack_pop(s);
    bignum* b = stack_pop(s);

    bignum* num = bignum_fromstr("0");
    bignum* zero = bignum_fromstr("0");

    switch(c) {
        case '<':
            num->first->value = bignum_gt(*a, *b) && !bignum_eq(*a, *b);
            break;
        case '>':
            num->first->value = bignum_gt(*b, *a);
            break;
        case '=':
            num->first->value = bignum_eq(*a, *b);
            break;
        case '|':
            num->first->value = !bignum_eq(*a, *zero) || !bignum_eq(*b, *zero);
            break;
        case '&':
            num->first->value = !bignum_eq(*a, *zero) && !bignum_eq(*b, *zero);
            break;
        default: break;
    }

    stack_push(s, num);

    bignum_destoroyah(a);
    bignum_destoroyah(zero);
    bignum_destoroyah(b);

    return c;
}

char push_ternary(char c, stack* s) {
    if(stack_len(s) < 3) {
        fprintf(stderr, "La ternaire (?) nécessite trois opérandes, taille du stack insuffisante\n");
        return c;
    }

    bignum* zero = bignum_fromstr("0");

    bignum* condition = stack_pop(s);
    bignum* v_false = stack_pop(s);
    bignum* v_true = stack_pop(s);

    if(!bignum_eq(*condition, *zero)) {
        stack_push(s, v_true);
        bignum_destoroyah(v_false);
    } else {
        stack_push(s, v_false);
        bignum_destoroyah(v_true);
    }

    bignum_destoroyah(zero);
    bignum_destoroyah(condition);

    return c;
}

char assign_var(char c, stack* s, bignum* variables[]) {
    if(c < 'a' || c > 'z') {
        fprintf(stderr, "Le nom de variable `%c` est erroné\n", c);
        return c;
    }

    // Si le stack est vide, on met (ou on laisse) NULL dans la variable
    if(stack_empty(s)) {
        if(variables[c - 'a']) {
            bignum_destoroyah(variables[c - 'a']);
        }

        variables[c - 'a'] = NULL;
        return c;
    }

    // Empêche d'incrémenter refs avec `100 =a =b =a`
    if(variables[c - 'a'] != stack_peek(s)) {
        variables[c - 'a'] = stack_peek(s);
        stack_peek(s)->refs++;
    }

    return c;
}

char push_var(char c, stack* s, bignum* variables[]) {
    if(variables[c - 'a'] == NULL) {
        fprintf(stderr, "La variable `%c` n'a pas été définie\n", c);
        return c;
    }

    stack_push(s, variables[c - 'a']);
    variables[c - 'a']->refs++;

    return c;
}

context* create_context(FILE* in, context_stack* c_s, char* saved, char type,  char enter, char exit) {
    context* context = context_init(type);

    // Pour supporter les contextes dans des contextes (ex.: loop dans une loop)
    int enter_count = 1;
    char c;

    while(enter_count) {
        c = get_next(in, c_s, saved);

        if(c == enter) {
            enter_count++;
        } else if (c == exit) {
            enter_count--;
        }

        context_append(context, c);
    }
    return context;
}

int main(int argc, char* argv[]) {
    int i, j;
    FILE* in = stdin;
    char c = ' ', waiting, interactive_mode = TRUE;
    stack* s = stack_init();
    context_stack* c_s = context_stack_init();
    // Char sauvegardé dans le cas ou un char est inputté directement après un nombre.
    char* saved = malloc(sizeof(char));
    *saved = -1;
    // Variables
    bignum* variables[26];

    // Procédures
    context* procedures[26];

    bignum* zero = bignum_fromstr("0");

    //Set toutes les variables et procédures à NULL
    for(i=0; i<26; i++) {
        variables[i] = NULL;
        procedures[i] = NULL;
    }

    // Parse les arguments
    for(i = 1; i < argc && argv[i][0] == '-'; i++) {
        if(strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--silent") == 0) {
            interactive_mode = FALSE;
        } else if(strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
            interactive_mode = 2; // verbose
        } else if(strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            printf("usage : stackulator [options]\n");
            printf("stackulator is a postfix stack machine that evaluates stdin into stdout\n");
            printf("options :\n");
            printf("    -s, --silent : don't show '>' prompt character\n");
            printf("    -v, --verbose : show useful stuff instead of just the '>' prompt character\n");
            printf("    -h, --help : displays help\n");
            printf("    -t, --test : runs the tests\n");
            return EXIT_SUCCESS;
        } else if(strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--test") == 0) {
            return test_all() ? EXIT_SUCCESS : EXIT_FAILURE;
        }
    }

    // Fichier en entrée
    if(i < argc) {
        in = fopen(argv[i], "r");
        if(in == NULL) {
            fprintf(stderr, "Impossible d'ouvrir le fichier %s\n", argv[i]);
            return EXIT_FAILURE;
        }
        interactive_mode = FALSE;
    }

    if(interactive_mode) {
        printf("-- Stackulator v1.0 --\n");
        printf("Entrez Ctrl+D pour quitter\n");
    }

    prompt(interactive_mode, s);

    while((c = get_next(in, c_s, saved)) != EOF) {
        waiting = FALSE;

        // Push un nouveau nombre sur la pile
        if(c >= '0' && c <= '9') {

            c = push_number(c, s, in, c_s, saved);
            waiting = c == '\n';

        // Addition
        } else if(c == '+') {
            push_op(c, s, bignum_add);
        // Soustraction
        } else if(c == '-') {
            push_op(c, s, bignum_sub);
        // Multiplication
        } else if(c == '*') {
            push_op(c, s, bignum_mul);
        // Extra : Multiplication par additions successives (dumb_mul)
        } else if(c == '!') {
            push_op(c, s, bignum_dumb_mul);
        // Assignation & test d'égalité
        } else if(c == '=') {
            char c = get_next(in, c_s, saved);

            // Extra : Test d'égalité
            if(c == '=') {
                push_test(c, s);
                continue;
            }

            assign_var(c, s, variables);

        // Assignation explicite de NULL dans une variable
        } else if(c == '_') {
            char c = get_next(in, c_s, saved);

            stack* empty_stack = stack_init();
            assign_var(c, empty_stack, variables);
            free(empty_stack);

        // Push une variable sur la pile
        } else if(c >= 'a' && c <= 'z') {

            push_var(c, s, variables);

        // Ignore les espaces
        } else if(c == ' ') {
        // Extra : commentaires. Ignore tout ce qu'il y a entre # et \n
        } else if(c == '#') {
            while(c != '\n'){
                c = get_next(in, c_s, saved);
            }
        // affiche le top du stack : ^ (ou \n en mode interactif)
        } else if(c == '^' || (c == '\n' && interactive_mode)) {

            if(!stack_empty(s)) {
                char* str = bignum_tostr(*stack_peek(s));
                printf("%s", str);

                if(interactive_mode){
                    printf("\n");
                }

                free(str);

            } else if(interactive_mode) {
                printf("Stack vide\n");
            }

            waiting = c == '\n';
        // Ignore le \n si le mode interactif est off
        } else if(c == '\n') {
        // Extra : comparateurs booléens &, |, > et <
        } else if(c == '>' || c == '<' || c == '&' || c == '|') {

            push_test(c, s);

        // Extra : opérateur ternaire
        } else if(c == '?') {
            push_ternary(c, s);
        // Extra : duplique le top du stack
        } else if(c == '@') {
            if(stack_empty(s)) {
                fprintf(stderr, "La copie nécessite un élément sur le stack\n");
                continue;
            }
            stack_push(s, bignum_copy(stack_peek(s)));
        // Extra : Shifts arithmétiques.
        } else if(c == '{' || c == '}') {
            push_shift(c, s);
        // Extra : Début de loop.
        } else if(c == '[') {
            //Le contexte est créé inconditionellement car il faut faire avancer
            //le contexte dans lequel la boucle est déclarée jusqu'a la fin
            //de la boucle dans les deux cas.
            context* loop_context = create_context(in, c_s, saved, CONTEXT_LOOP, '[', ']');
            if(stack_empty(s) || bignum_eq(*stack_peek(s), *zero) ) {
                context_destoroyah(loop_context);
                continue;
            }

            context_stack_push(c_s, loop_context);
        // Fin de boucle. (Seulement si on est dans une boucle)
        } else if(c == ']' && context_stack_peek(c_s)->type == CONTEXT_LOOP) {
            // Si le top du stack est existant et positif, continue comme ça
            if(!stack_empty(s) && bignum_absgt(*stack_peek(s), *zero) ) {
                continue;
            }
            // Sinon, tue le contexte.
            context_destoroyah(context_stack_pop(c_s));
        // Extra : début d'une procédure.
        } else if(c == ':') {
            if(!context_stack_empty(c_s)) {
                fprintf(stderr, "Impossible de définir une procédure depuis une boucle ou une procédure\n");
            }

            c = get_next(in, c_s, saved);
            if(c < 'A' || c > 'Z') {
                fprintf(stderr, "Le nom de procédure `%c` est erroné\n", c);
                continue;
            }

            context* procedure_context = create_context(in, c_s, saved, CONTEXT_PROC, ':', ';');
            procedures[c - 'A'] = procedure_context;

        // Extra : le retour d'une procédure (seulement si on est dans une proc.)
        } else if(c == ';' && !context_stack_empty(c_s) && context_stack_peek(c_s)->type == CONTEXT_PROC) {
            context_stack_pop(c_s);
        // Extra : L'appel d'une procédure
        } else if(c >= 'A' && c <= 'Z') {
            if(procedures[c - 'A'] == NULL){
                fprintf(stderr, "La procédure `%c` n'est pas définie.\n", c);
                continue;
            }
            context_stack_push(c_s, procedures[c - 'A']);
        // Extra : dump le contenu du stack
        } else if(c == '$') {
            stack_dump(s);
        // Extra : dump le contenu des variables (adresses des bignums référencés)
        } else if(c == '%') {
            printf("-- Variables --\n");
            for(i = 0; i < 5; i++) {
                for(j = 0; j < 5; j++) {
                    printf("%c: %9p |", 'a' + i * 5 + j, variables[i * 5 + j]);
                }
                printf("\n");
            }
            printf("%c: %9p\n", 'z', variables[25]);
        // Extra : pop le top
        } else if(c == '.') {
           if(!stack_empty(s)) {
                bignum_destoroyah(stack_pop(s));
            }
        // Extra : lis un nombre depuis stdin
        } else if(c == ',') {

            c = getc(stdin);
            while(c == '\n' || c == ' ') {
                c = getc(stdin);
            }

            char negative = 0;

            if(c == '-') {
                negative = 1;
                c = getc(stdin);
            }

            while(c < '0' || c > '9') {
                c = getc(stdin);
            }

            //Si des char sont lu de l'entrée, il ne faut pas les prendres dans le contexte courrant.
            context_stack* empty_stack = context_stack_init();
            /* On passe NULL comme saved car on ne veut pas que l'utilisateur
               puisse injecter du code en mettant une opération comme dernier
               char de son entrée.
            */
            c = push_number(c, s, stdin, empty_stack, NULL);
            // Si le dernier char lu n'est pas ' ' ou '\n', on avertis l'utilisateur
            // que ce char a été ignoré.
            if(c != ' ' && c != '\n') {
                printf("Attention : les suffixes de nombres ne sont pas pris en compte ('%c' ignoré)", c);
            }
            free(empty_stack);

            stack_peek(s)->sign = negative;
            waiting = TRUE;

        // Extra : echo
        } else if(c == '"') {

            char last_char = c;
            c = get_next(in, c_s, saved);

            while((c != '"' || last_char == '\\') && c != EOF) {
                if(last_char != '\\' && c == '\\') {
                    last_char = c;
                    c = get_next(in, c_s, saved);
                    continue;
                }

                printf("%c", c);
                last_char = c;
                c = get_next(in, c_s, saved);
            }

            waiting = c == '\n';
        // Extra : vide le stack
        } else if(c == '~') {
            while(!stack_empty(s)) {
                bignum_destoroyah(stack_pop(s));
            }
        // Gestion des expressions inconnues
        } else {
            fprintf(stderr, "Expression inconnue ignorée : %c", c);
            while(c != ' ' && c != '\n') {
                c = get_next(in, c_s, saved);
                fprintf(stderr, "%c", c);
            }
            fprintf(stderr, "\n");
            waiting = c == '\n';
        }

        if(waiting) {
            prompt(interactive_mode, s);
        }
    }

    while(!stack_empty(s)) {
        bignum_destoroyah(stack_pop(s));
    }

    free(s);

    while(!context_stack_empty(c_s)) {
        context_destoroyah(context_stack_pop(c_s));
    }

    free(c_s);

    for(i=0; i<26; i++) {
        if(variables[i] != NULL) {
            bignum_destoroyah(variables[i]);
        }
        if(procedures[i] != NULL) {
            context_destoroyah(procedures[i]);
        }
    }

    free(saved);
    bignum_destoroyah(zero);

    return EXIT_SUCCESS;
}
