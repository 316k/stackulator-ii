#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "help.c"
#include "bignum.c"
#include "stack.c"
#include "test.c"


#define input_isnt(truc) \
    (c = getchar()) != truc

int main(int argc, char* argv[]) {
    test_all();

    char c = ' ';

    while((c = getchar()) != EOF) {
        if(c >= '1' && c <= '9') {
            printf("Nombre entré : %c", c);
            while(c != ' ' && c != '\n') {
                c = getchar();
                printf("%c", c);
            }
            printf("\n");
        } else if(c == ' ' || c == '\n') {
        } else {
            printf("Impossible de comprendre %c", c);
            while(c != ' ' && c != '\n') {
                c = getchar();
                printf("%c", c);
            }
            printf("\n");
        }
    }

    return EXIT_SUCCESS;
}
