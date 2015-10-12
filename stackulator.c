#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "help.c"
#include "circular_list.c"
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
//Retourne le prochain charactère du programme.
char get_next(FILE* in, context_stack* c_s){
    if(context_stack_empty(c_s)){
        return getc(in);
    }

    return circular_list_next(context_stack_peek(c_s));
}

char push_number(char c, stack* s, FILE* in, context_stack* c_s) {
    bignum* num = bignum_init();

    bigdigit* digit = NULL;
    bigdigit* prev_addr = NULL;

    // Si ça commence avec 0, on veut potentiellement garder le premier seulement
    if(c == '0') {
        digit = bigdigit_init();

        prev_addr = digit;

        while((c = get_next(in, c_s)) == '0');

        // Si ça commençait avec des 0 non-significatifs, on les flush
        if(c >= '1' && c <= '9') {
            digit->value = c - '0';
            c = get_next(in, c_s);
        }
    }

    while(c >= '0' && c <= '9') {

        digit = bigdigit_init();

        digit->next = prev_addr;
        digit->value = c - '0';

        prev_addr = digit;

        c = get_next(in, c_s);
    }

    if(c != ' ' && c != '\n') {
        fprintf(stderr, "Attention : les suffixes de nombres ne sont pas pris en compte (%c ignoré)\n", c);
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

    variables[c - 'a'] = stack_peek(s);
    stack_peek(s)->refs++;

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

void fill_context(circular_list* context, FILE* in, context_stack* c_s,  char enter, char exit){
    int enter_count = 1;
    char c;
    while(enter_count){
        c = get_next(in, c_s);
        if(c == enter){
            enter_count++;
        }else if (c == exit){
            enter_count--;
        }
        circular_list_append(context, c);
    }
}

int main(int argc, char* argv[]) {
    int i, j;
    FILE* in = stdin;
    char c = ' ', waiting, interactive_mode = TRUE;
    stack* s = stack_init();
    context_stack* c_s = context_stack_init();
    // Variables
    bignum* variables[26];
    // Procédures
    circular_list* procedures[26];

    bignum* zero = bignum_fromstr("0");

    //Sette toutes les vars et proc à NULL;
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
            printf("stackulator is a postfix calculator evaluates stdin into stdout\n");
            printf("Enter prints the top of the stack, supported operations are + - *\n");
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

        // On ignore les #!/path/to/bin au besoin
        char shebang[] = "00";
        fread(shebang, sizeof(char), 2, in);

        if(strcmp(shebang, "#!") == 0) {
            while(getc(in) != '\n');
        }

        interactive_mode = FALSE;
    }

    if(interactive_mode) {
        printf("-- Stackulator v1.0 --\n");
        printf("Entrez Ctrl+D pour quitter\n");
    }

    prompt(interactive_mode, s);

    while((c = get_next(in, c_s)) != EOF) {
        waiting = FALSE;

        // Push un nouveau nombre sur la pile
        if(c >= '0' && c <= '9') {

            c = push_number(c, s, in, c_s);
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
            char c = get_next(in, c_s);

            // Extra : Test d'égalité
            if(c == '=') {
                push_test(c, s);
                continue;
            }

            assign_var(c, s, variables);

        // Assignation explicite de NULL dans une variable
        } else if(c == '_') {
            char c = get_next(in, c_s);

            stack* empty_stack = stack_init();
            assign_var(c, empty_stack, variables);
            free(empty_stack);

        // Push une variable sur la pile
        } else if(c >= 'a' && c <= 'z') {

            push_var(c, s, variables);

        // Ignore les espaces
        } else if(c == ' ') {
        // ^ affiche le top du stack et \n aussi si interactif
        } else if(((c == '^') && !interactive_mode) || ((c == '\n') && interactive_mode)) {

            if(!stack_empty(s)) {
                char* str = bignum_tostr(*stack_peek(s));
                printf("%s", str);
                free(str);
            } else if(interactive_mode) {
                printf("Stack vide\n");
            }

            if(interactive_mode){
                printf("\n");
            }

            waiting = TRUE;
        // Ignore le \n si le mode interactif est off
        } else if(c == '\n'){
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
        // Extra : Début de loop.
        } else if(c == '[') {
            circular_list* loop_context = circular_list_init();
            //Construit le contexte
            fill_context(loop_context, in, c_s, '[', ']');
            //Si le stack est vide ou que le top == 0 détruit le contexte et continue.
            if(stack_empty(s) || bignum_eq(*stack_peek(s), *zero) ) {
                circular_list_destoroyah(loop_context);
            } else {
                //Sinon ajoute le contexte sur le context_stack.
                context_stack_push(c_s, loop_context);
            }
        // Fin de boucle.
        } else if(c == ']') {
            // Si le top du stack est existant et positif, continue comme ça
            if(!stack_empty(s) && bignum_absgt(*stack_peek(s), *zero) ) {
                continue;
            }
            // Sinon, tue le contexte.
            circular_list_destoroyah(context_stack_pop(c_s));
        // Extra : début d'une procédure.
        } else if(c == ':') {
            if(!context_stack_empty(c_s)) {
                fprintf(stderr, "Vous ne pouvez définir une fonction qu'hors d'une boucle ou d'une fonction.");
            }
            c = get_next(in, c_s);
            if(c < 'a' || c > 'z') {
                fprintf(stderr, "Le nom de variable `%c` est erroné\n", c);
                continue;
            }
            // Définis la procédure en créant un contexte et en y mettant tous
            // Les trucs jusqu'au ';'.
            circular_list* procedure_context = circular_list_init();
            fill_context(procedure_context, in, c_s, ':', ';');
            procedures[c - 'a'] = procedure_context;

        // Extra : le retour d'une procédure
        } else if(c == ';') {
            context_stack_pop(c_s);
        // Extra : L'appel d'une procédure
        } else if(c == '/') {
            c = get_next(in, c_s);
            if(c < 'a' || c > 'z') {
                fprintf(stderr, "Le nom de variable `%c` est erroné\n", c);
                continue;
            }
            if(procedures[c - 'a'] == NULL){
                fprintf(stderr, "La procédure `%c` n'est pas définie.", c);
                continue;
            }
            context_stack_push(c_s, procedures[c - 'a']);
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
            c = push_number(c, s, stdin, empty_stack);
            free(empty_stack);

            stack_peek(s)->sign = negative;
            waiting = TRUE;

        // Extra : echo
        } else if(c == '"') {

            char last_char = c;
            c = get_next(in, c_s);

            while((c != '"' || last_char == '\\') && c != EOF) {
                if(last_char != '\\' && c == '\\') {
                    last_char = c;
                    c = get_next(in, c_s);
                    continue;
                }

                printf("%c", c);
                last_char = c;
                c = get_next(in, c_s);
            }

            waiting = TRUE;
        // Extra : vide le stack
        } else if(c == '~') {
            while(!stack_empty(s)) {
                bignum_destoroyah(stack_pop(s));
            }
        // Gestion des expressions inconnues
        } else {
            fprintf(stderr, "Expression inconnue ignorée : %c", c);
            while(c != ' ' && c != '\n') {
                c = get_next(in, c_s);
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

    for(i=0; i<26; i++) {
        if(variables[i] != NULL) {
            free(variables[i]);
        }
    }

    return EXIT_SUCCESS;
}
