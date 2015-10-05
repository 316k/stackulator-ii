#include <stdlib.h>

#define BIGNUM_NEGATIVE 1
#define BIGNUM_POSITIVE 0

// Note : le truc est actuellement en big-endian
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

// (Already) DEPRECATED
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

    bignum* num = malloc(sizeof(bignum));
    bigdigit* digit = NULL;
    num->sign = str[0] == '-';

    bigdigit** next = &num->first;

    for(i = strlen(str); i > num->sign; i--) {
        digit = malloc(sizeof(bigdigit));
        digit->value = str[i - 1] - '0';

        *next = digit;
        next = &digit->next;
    }
    
    *next = NULL;

    return num;
}

int bignum_len(bignum* num) {
    long len = 0;
    bigdigit** ptr_addr = NULL;

    ptr_addr = &num->first;

    while(*ptr_addr != NULL) {
        len++;
        ptr_addr = &(*ptr_addr)->next;
    }

    return len;
}

char* bignum_tostr(bignum* num) {
    int len = bignum_len(num), i=0;
    char* out = malloc(sizeof(char) * (len + 1 + num->sign));

    bigdigit** digit_addr = &num->first;

    if(num->sign) {
        out[0] = '-';
    }

    for(i = len + num->sign; i > num->sign; i--) {
        out[i-1] = (*digit_addr)->value + '0';
        digit_addr = &(*digit_addr)->next;
    }

    out[len + num->sign] = '\0';

    return out;
}

// Returns a > b
char bignum_gt(bignum* a, bignum* b) {
    char size = max(bignum_len(a), bignum_len(b));
    return (a->sign == b->sign && a->sign == 1 ? -1 : 1) * strcmp(strpad(bignum_tostr(a), '0', size), strpad(bignum_tostr(b), '0', size)) > 0;
}

char bignum_absgt(bignum a, bignum b) {
    char size = max(bignum_len(&a), bignum_len(&b));
    a.sign = 0;
    b.sign = 0;
    return strcmp(strpad(bignum_tostr(&a), '0', size), strpad(bignum_tostr(&b), '0', size)) > 0;
}

// Returns a == b
char bignum_eq(bignum* a, bignum* b) {
    char size = max(bignum_len(a), bignum_len(b));
    return strcmp(strpad(bignum_tostr(a), '0', size), strpad(bignum_tostr(b), '0', size)) == 0;
}

bignum* bignum_add(bignum* a, bignum* b) {
    char carry = 0;

    /* Indique l'opération à  faire en valeur absolue
       1 si les deux nombres sont additionnés en valeur absolue
       0 s'il s'agit d'une soustraction
    */
    char op = 1;

    int i = 0;
    int size_a = bignum_len(a), size_b = bignum_len(b), max_size = max(size_a, size_b);

    bignum* sum = malloc(sizeof(bignum));
    bigdigit* digit = NULL;
    bigdigit** da = &a->first;
    bigdigit** db = &b->first;
    bigdigit** next = &sum->first;

    if(a->sign == b->sign) {
        sum->sign = a->sign;
    } else {
        // Ensures |a| >= |b| in the following lines
        if(bignum_absgt(*b, *a))
            return bignum_add(b, a);
        
        sum->sign = a->sign;
        op = -1;
    }

    for(i = 0; i < max_size; i++) {
        digit = malloc(sizeof(bigdigit));
        digit->value = carry;

        if(i < size_a) {
            digit->value += (*da)->value;
            da = &(*da)->next;
        }
        if(i < size_b) {
            digit->value += op * (*db)->value;
            db = &(*db)->next;
        }
        
        carry = op * (digit->value > 10 || digit->value < 0);
        digit->value = (digit->value + 10) % 10;
        *next = digit;
        next = &digit->next;
    }
    
    /*
     1        1 
     3281   3281
    +2816  -2816
    -----  -----
     6097   0465
    */

    return sum;
}

bignum* bignum_sub(bignum* a, bignum* b) {
    a->sign = !b->sign;
    return bignum_add(a, b);
}

void bignum_mul(bignum* a, bignum* b) {
}
