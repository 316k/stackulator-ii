#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "help.c"
#include "bignum.c"
#include "stack.c"
#include "test.c"

int main(int argc, char* argv[]) {
    int i;
    char c = ' ', waiting, interactive_mode = TRUE;
    stack* s = stack_init();

    // Parse les arguments
    for(i = 1; i < argc && argv[i][0] == '-'; i++) {
        if(strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--silent") == 0) {
            interactive_mode = FALSE;
        } else if(strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            printf("usage : stackulator [options]\n");
            printf("stackulator is a postfix calculator evaluates stdin into stdout\n");
            printf("Enter prints the top of the stack, supported operations are + - *\n");
            printf("options :\n");
            printf("    -s, --silent : don't show '>' prompt character\n");
            // printf("    -e, --ed : `ed` mode\n");
            printf("    -h, --help : displays help\n");
            return EXIT_SUCCESS;
        } else if(strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--test") == 0) {
            test_all();
            return EXIT_SUCCESS;
        }
    }

    if(interactive_mode) {
        printf("> ");
    }

    while((c = getchar()) != EOF) {
        waiting = FALSE;

        if(c >= '0' && c <= '9') {
            bignum* num = malloc(sizeof(bignum));

            please_dont_segfault(num);

            num->sign = BIGNUM_POSITIVE; // Impossible d'entrer un nombre négatif

            bigdigit* digit = NULL;
            bigdigit* prev_addr = NULL;

            while(c != ' ' && c != '\n') {

                digit = malloc(sizeof(bigdigit));

                please_dont_segfault(digit);

                digit->next = prev_addr;
                digit->value = c - '0';

                prev_addr = digit;

                c = getchar();
            }

            num->first = prev_addr;

            bignum_clean(num);
            
            // printf("%s", bignum_tostr()

            stack_push(s, num);

            waiting = c == '\n';
        } else if(c == '+') {
            if(stack_len(s) < 2) {
                fprintf(stderr, "Pas assez de nombres sur le stack\n");
            } else {
                bignum* a = stack_pop(s);
                bignum* b = stack_pop(s);

                bignum* num = bignum_add(*a, *b);
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
        } else {
            fprintf(stderr, "Symbole inconnu ignoré : %c", c);
            while(c != ' ' && c != '\n') {
                c = getchar();
                fprintf(stderr, "%c", c);
            }
            fprintf(stderr, "\n");
            waiting = c == '\n';
        }

        if(waiting && interactive_mode) {
            printf("> ");
        }
    }

    return EXIT_SUCCESS;
}
