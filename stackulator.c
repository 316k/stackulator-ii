#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "help.c"
#include "bignum.c"
#include "stack.c"
#include "test.c"

int main(int argc, char* argv[]) {
    test_all();
    int i;
    char c = ' ';
    char waiting;
    char interactive_mode = TRUE;

    // Parse les arguments
    for(i = 1; i < argc && argv[i][0] == '-'; i++) {
        if(strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--silent") == 0) {
            interactive_mode = FALSE;
        } else if(strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            printf("usage : stackulator [options]\n");
            printf("stackulator is a postfix calculator evaluates stdin into stdout\n");
            printf("options :\n");
            printf("    -s, --silent : don't show '>' prompt character\n");
            printf("    -h, --help : displays help\n");
            return EXIT_SUCCESS;
        }
    }

    if(interactive_mode) {
        printf("> ");
    }

    while((c = getchar()) != EOF) {
        waiting = FALSE;

        if(c >= '1' && c <= '9') {
            printf("Nombre entré : %c", c);
            while(c != ' ' && c != '\n') {
                c = getchar();
                printf("%c", c);
            }
            printf("\n");

        } else if(c == ' ' || c == '\n') {
            // Ignore
            waiting = c == '\n';

        } else {
            fprintf(stderr, "Symbole inconnu ignoré : (%d) %c", c, c);
            while(c != ' ' && c != '\n') {
                c = getchar();
                fprintf(stderr, "(%d) %c", c, c);
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
