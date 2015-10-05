#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "help.c"
#include "bignum.c"
#include "stack.c"
#include "test.c"

int main(int argc, char* argv[]) {
    test_all();
    
    return EXIT_SUCCESS;
}
