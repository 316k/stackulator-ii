#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bignum.c"
#include "stack.c"
#include "test.c"

int main(int argc, char* argv[]) {
    test_bignum_nextdigit_addr();
    test_stack_push();
    test_stack_peek();
    test_stack_pop();
    test_stack_empty();
    
    return EXIT_SUCCESS;
}
