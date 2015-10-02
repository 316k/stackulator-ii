#include <stdlib.h>


// Note : le truc est en little-endian
typedef struct bigdigit bigdigit;
typedef struct bignum bignum;

struct bigdigit {
    char value;
    bigdigit* next;
};

struct bignum {
    char sign; // 0 => +, 1 => -
    bigdigit* first;
};

bigdigit*
bignum_nextdigit(bignum* num) {
    bigdigit* ptr = NULL;
    
    ptr = num->first;
    
    while(ptr != NULL) {
        ptr = ptr->next;
    }
    
    return ptr;
}

bignum*
bignum_fromstr(char str[]) {
    int i;
    // New bignum
    bignum* num = malloc(sizeof(bignum));
    bigdigit* digit = NULL;

    for(i = strlen(str) - 1;  i >= 0; i--) {
        digit = malloc(sizeof(bigdigit));
        digit->value = str[i-1] - '0';
        
        // printf("%x ", bignum_nextdigit(num));
        
        // *(bignum_nextdigit(num)) = digit;
    }

    return num;
}

bignum*
bignum_tostr(char str[]) {
    int i;
    // New bignum
    bignum* num = malloc(sizeof(bignum));
    bigdigit* digit = NULL;

    for(i = strlen(str) - 1;  i >= 0; i--) {
        digit = malloc(sizeof(bigdigit));
        digit->value = str[i-1] - '0';
        
        // printf("%x ", bignum_nextdigit(num));
        
        // *(bignum_nextdigit(num)) = digit;
    }

    return num;
}

void
bignum_add(bignum* dest, bignum* operande) {

}

void
bignum_sub(bignum* dest, bignum* operande) {

}

void
bignum_mul(bignum* dest, bignum* operande) {

}
