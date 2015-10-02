#include <stdlib.h>


// Note : le truc est en little-endian
typedef struct bigdigit bigdigit;
struct bigdigit {
    char valeur;
    bigdigit* next;
};

typedef struct bignum bignum;
struct bignum {
    char sign; // 0 => +, 1 => -
    bigdigit first;
};

bignum*
bignum_fromstr(char str[]) {
   bignum* num = malloc(sizeof(bignum));
   bigdigit* digit = NULL;
   int i;

   for(i=0; i< strlen(str); i++) {
       digit = malloc(sizeof(bigdigit));
       free(digit);
   }

   return num;
}

void bignum_add(bignum* dest, bignum* add) {

}

