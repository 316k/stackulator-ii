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

bigdigit** bignum_nextdigit_addr(bignum* num) {
    bigdigit** ptr_addr = NULL;
    
    ptr_addr = &num->first;
    
    while(*ptr_addr != NULL) {
        ptr_addr = &(*ptr_addr)->next;
    }
    
    return ptr_addr;
}

bignum* bignum_fromstr(char str[]) {
    int i;
    // New bignum
    bignum* num = malloc(sizeof(bignum));
    bigdigit* digit = NULL;
    
    bigdigit** next = &num->first;

    for(i = strlen(str) - 1;  i >= 0; i--) {
        digit = malloc(sizeof(bigdigit));
        digit->value = str[i-1] - '0';
        
        *next = digit;
        next = &digit->next;
    }

    return num;
}

long bignum_len(bignum* num) {
    long len = 0;
    
    return len;
}

char* bignum_tostr(char str[]) {
    return "nopes";
}

void bignum_add(bignum* dest, bignum* operande) {

}

void bignum_sub(bignum* dest, bignum* operande) {

}

void bignum_mul(bignum* dest, bignum* operande) {

}
