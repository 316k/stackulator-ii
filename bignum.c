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

void destroya_bignum(bignum* num) {
    // Vive les bignums libres !
    bigdigit** current = &num->first;
    bigdigit** next = NULL;

    while(current != NULL) {
        next = &(*current)->next;
        free(*current);
        current = next;
    }
}

/**
 * Détruit les trailing zéros
 */
void bignum_clean(bignum* num) {
    int len = bignum_len(*num), relevant_size = 0, i;
    bigdigit** digit_addr = &num->first;
    bigdigit** current = NULL;
    bigdigit** next = NULL;
    bigdigit** prev = NULL;

    for(i = 0; i < len; i++) {
        relevant_size = (*digit_addr)->value ? i : relevant_size;
        digit_addr = &(*digit_addr)->next;
    }

    relevant_size++;

    digit_addr = &num->first;

    for(i = 0; i<relevant_size; i++) {
        prev = digit_addr;
        digit_addr = &(*digit_addr)->next;
    }

    current = digit_addr;

    for(i = relevant_size; i < len; i++) {
        next = &(*current)->next;
        free(*current);
        current = next;
    }
    (*prev)->next = NULL;
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

    bignum_clean(num); // Lazy way

    return num;
}

/*
 * Donne la taille du bignum (le nombre de digits, sans le signe '-')
 */
int bignum_len(bignum num) {
    long len = 0;
    bigdigit** ptr_addr = NULL;

    ptr_addr = &num.first;

    while(*ptr_addr != NULL) {
        len++;
        ptr_addr = &(*ptr_addr)->next;
    }

    return len;
}

char* bignum_tostr(bignum num) {
    // Assume que num a été `bignum_cleané`

    int len = bignum_len(num), i;
    bigdigit** digit_addr = &num.first;

    // Traite le '0' seul
    if((*digit_addr)->next == NULL && (*digit_addr)->value == 0) {
        return "0";
    }

    char* out = malloc(sizeof(char) * (len + 1 + num.sign));

    if(num.sign) {
        out[0] = '-';
    }

    digit_addr = &num.first;

    for(i = len + num.sign; i > num.sign; i--) {
        out[i-1] = (*digit_addr)->value + '0';
        digit_addr = &(*digit_addr)->next;
    }

    out[len + num.sign] = '\0';

    return out;
}

char bignum_absgt(bignum a, bignum b) {
    // Assume que num a été `bignum_cleané`

    int size_a = bignum_len(a), size_b = bignum_len(b), i;

    a.sign = b.sign = 0;

    if(size_a != size_b) {
        return size_a > size_b;
    }

    bigdigit** ptr_addr_a = &a.first;
    bigdigit** ptr_addr_b = &b.first;
    
    for(i = 0; i < size_a; i++) {
        if((*ptr_addr_a)->value != (*ptr_addr_b)->value) {
            return FALSE;
        }
        ptr_addr_a = &(*ptr_addr_a)->next;
        ptr_addr_b = &(*ptr_addr_b)->next;
    }

    return 0;
}

// Returns a == b
char bignum_eq(bignum a, bignum b) {
    // Assume que num a été `bignum_cleané`
    int size_a = bignum_len(a), size_b = bignum_len(b), i;
    
    if(size_a != size_b || a.sign != b.sign) {
        return FALSE;
    }
    
    bigdigit** ptr_addr_a = &a.first;
    bigdigit** ptr_addr_b = &b.first;
    
    for(i = 0; i < size_a; i++) {
        if((*ptr_addr_a)->value != (*ptr_addr_b)->value) {
            return FALSE;
        }
        ptr_addr_a = &(*ptr_addr_a)->next;
        ptr_addr_b = &(*ptr_addr_b)->next;
    }
    
    return TRUE;
}

bignum* bignum_add(bignum a, bignum b) {
    char carry = 0;

    /* Indique l'opération à  faire en valeur absolue
       1 si les deux nombres sont additionnés en valeur absolue
       -1 s'il s'agit d'une soustraction
    */
    char op = 1;

    int i = 0;
    int size_a = bignum_len(a), size_b = bignum_len(b), max_size = max(size_a, size_b);

    bignum* sum = malloc(sizeof(bignum));
    bigdigit* digit = NULL;
    bigdigit** da = &a.first;
    bigdigit** db = &b.first;
    bigdigit** next = &sum->first;

    if(a.sign == b.sign) {
        sum->sign = a.sign;
    } else {
        // Assure que |a| >= |b| par la suite
        if(bignum_absgt(b, a))
            return bignum_add(b, a);

        sum->sign = a.sign;
        op = -1;
    }

    for(i = 0; i < max_size + 1; i++) {
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

        carry = op * (digit->value >= 10 || digit->value < 0);
        digit->value = (digit->value + 10) % 10;
        *next = digit;
        next = &digit->next;
    }

    bignum_clean(sum);

    return sum;
}

bignum* bignum_sub(bignum a, bignum b) {
    b.sign = !b.sign;
    return bignum_add(a, b);
}

bignum* bignum_mul(bignum a, bignum b) {
    bignum* prod = bignum_fromstr("0");
    bignum* zero = bignum_fromstr("0");
    bignum* dec = bignum_fromstr("-1");
    
    while(!bignum_eq(b, *zero)) {
        b = *bignum_add(b, *dec); // Decrement b
        prod = bignum_add(*prod, a);
    }
    
    //destroya_bignum(zero);
    //destroya_bignum(dec);
    
    return prod;
}
