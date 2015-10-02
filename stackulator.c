#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bignum.c"
#include "stack.c"
#include "test.c"

int main(int argc, char* argv[]) {
    test_bignum_nextdigit_addr();
    
    return EXIT_SUCCESS;
}
