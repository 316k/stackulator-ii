#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "help.c"
#include "bignum.c"
#include "stack.c"
#include "test.c"

int main(int argc, char* argv[]) {
    int i;
    FILE* in = stdin;
    char c = ' ', waiting, interactive_mode = TRUE;
    stack* s = stack_init();

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
            return EXIT_FAILURE;
        }
        interactive_mode = FALSE;
    }

    if(interactive_mode == 1) {
        printf("> ");
    } else if(interactive_mode == 2) {
        printf("0:(null)> ");
    }

    while((c = getc(in)) != EOF) {
        waiting = FALSE;

        if(c >= '0' && c <= '9') {
            bignum* num = malloc(sizeof(bignum));

            please_dont_segfault(num);

            num->sign = BIGNUM_POSITIVE; // Impossible d'entrer un nombre négatif

            bigdigit* digit = NULL;
            bigdigit* prev_addr = NULL;

            // Si ça commence avec 0, on veut potentiellement garder le premier seulement
            if(c == '0') {
                digit = malloc(sizeof(bigdigit));

                digit->next = NULL;
                digit->value = 0;
                prev_addr = digit;

                while((c = getc(in)) == '0');

                // Si ça commençait avec des 0 non-significatifs, on les flush
                if(c >= '1' && c <= '9') {
                    digit->value = c - '0';
                    c = getc(in);
                }
            }

            while(c >= '0' && c <= '9') {

                digit = malloc(sizeof(bigdigit));

                please_dont_segfault(digit);

                digit->next = prev_addr;
                digit->value = c - '0';

                prev_addr = digit;

                c = getc(in);
            }

            num->first = prev_addr;

            stack_push(s, num);

            waiting = c == '\n';
        } else if(c == '+') {
            if(stack_len(s) < 2) {
                fprintf(stderr, "+ nécessite deux opérandes, taille du stack insuffisante\n");
            } else {
                bignum* a = stack_pop(s);
                bignum* b = stack_pop(s);

                bignum* num = bignum_add(*a, *b);
                stack_push(s, num);

                bignum_destoroyah(a);
                bignum_destoroyah(b);
            }
        } else if(c == '-') {
            if(stack_len(s) < 2) {
                fprintf(stderr, "- nécessite deux opérandes, taille du stack insuffisante\n");
            } else {
                bignum* a = stack_pop(s);
                bignum* b = stack_pop(s);

                bignum* num = bignum_sub(*a, *b);
                stack_push(s, num);

                bignum_destoroyah(a);
                bignum_destoroyah(b);
            }
        } else if(c == '*') {
            if(stack_len(s) < 2) {
                fprintf(stderr, "* nécessite deux opérandes, taille du stack insuffisante\n");
            } else {
                bignum* a = stack_pop(s);
                bignum* b = stack_pop(s);

                bignum* num = bignum_mul(*a, *b);
                stack_push(s, num);

                bignum_destoroyah(a);
                bignum_destoroyah(b);
            }
        } else if(c == ' ') {
            // Ignored
        } else if(c == '\n') {
            if(!stack_empty(s)) {
                printf("%s\n", bignum_tostr(*stack_peek(s)));
            } else {
                printf("Stack vide\n");
            }
            waiting = TRUE;
        } else if(c == '$') {
            // Dump stack
            stack_dump(s);
        } else if(c == '.') {
            // Pop
           if(!stack_empty(s)) {
                bignum* a = stack_pop(s);
                bignum_destoroyah(a);
            }
        } else if(c == '#') {
            // Clear stack
            while(!stack_empty(s)) {
                stack_pop(s);
            }
        } else {
            fprintf(stderr, "Symbole inconnu ignoré : %c", c);
            while(c != ' ' && c != '\n') {
                c = getc(in);
                fprintf(stderr, "%c", c);
            }
            fprintf(stderr, "\n");
            waiting = c == '\n';
        }

        if(waiting && interactive_mode == 1) {
            printf("> ");
        } else if(waiting && interactive_mode == 2) {
            if(stack_empty(s)) {
                printf("0:(null)> ");
            } else {
                printf("%ld:%s> ", stack_len(s), bignum_tostr(*stack_peek(s)));
            }
        }
    }

    return EXIT_SUCCESS;
}
