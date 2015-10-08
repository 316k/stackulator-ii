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
    int refs;
};

/**
 * Init un bignum (positif, sans digits)
 */
bignum* bignum_init() {
    bignum* num = malloc(sizeof(bignum));
    please_dont_segfault(num);
    num->first = NULL;
    num->refs = 1;
    num->sign = 0;
    return num;
}

/**
 * Init un bigdigit (à zéro, sans élément suivant)
 */
bigdigit* bigdigit_init() {
    bigdigit* digit = malloc(sizeof(bigdigit));

    please_dont_segfault(digit);

    digit->value = 0;
    digit->next = NULL;

    return digit;
}

/**
 * free() le bignum et tous les bigdigits associés
 * Voir https://ja.wikipedia.org/wiki/ザ・デストロイヤー pour plus de détails
 */
void bignum_destoroyah(bignum* num) {
    num->refs--;

    if(num->refs == 0) {
        // Vive les bignums libres !
        bigdigit* current = num->first;
        bigdigit* next = NULL;

        while(current != NULL) {
            next = current->next;
            free(current);
            current = next;
        }

        free(num);
    }
}

/*
 * Donne la taille du bignum (le nombre de digits, sans le signe '-')
 */
int bignum_len(bignum num) {
    long len = 0;
    bigdigit** ptr_addr = &num.first;

    while(*ptr_addr != NULL) {
        len++;
        ptr_addr = &(*ptr_addr)->next;
    }

    return len;
}

/**
 * Détruit les trailing zéros
 */
void bignum_clean(bignum* num) {
    int len = bignum_len(*num), relevant_size = 0, i;
    bigdigit* digit_addr = num->first;
    bigdigit* current = NULL;
    bigdigit* next = NULL;
    bigdigit* prev = NULL;

    for(i = 0; i < len; i++) {
        relevant_size = digit_addr->value ? i : relevant_size;
        digit_addr = digit_addr->next;
    }

    relevant_size++;

    digit_addr = num->first;

    for(i = 0; i < relevant_size; i++) {
        prev = digit_addr;
        digit_addr = digit_addr->next;
    }

    current = digit_addr;

    for(i = relevant_size; i < len; i++) {
        next = current->next;
        free(current);
        current = next;
    }
    prev->next = NULL;
}

/**
 * Construit un bignum depuis une string
 */
bignum* bignum_fromstr(char str[]) {
    int i;

    bignum* num = bignum_init();
    num->sign = str[0] == '-';

    bigdigit* digit = NULL;
    bigdigit** next = &num->first;

    for(i = strlen(str); i > num->sign; i--) {
        digit = bigdigit_init();

        digit->value = str[i - 1] - '0';

        *next = digit;
        next = &digit->next;
    }

    *next = NULL;

    bignum_clean(num); // Lazy way

    return num;
}

/**
 * Donne la représentation en char* d'un bignum
 */
char* bignum_tostr(bignum num) {
    // Assume que num a été `bignum_cleané`

    int len = bignum_len(num), i;
    bigdigit** digit_addr = &num.first;

    // Traite le '0' seul
    if((*digit_addr)->next == NULL && (*digit_addr)->value == 0) {
        return "0";
    }

    char* out = malloc(sizeof(char) * (len + 1 + num.sign));

    please_dont_segfault(out);

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

/**
 * Construit un bignum avec les digits de num inversés
 */
bignum* bignum_rev(bignum num) {
    bignum* rev = bignum_init();

    bigdigit* rev_digit = NULL;
    bigdigit** digit_addr = &num.first;
    bigdigit* prev_addr = NULL;

    // Construit le nouveau bignum à l'envers
    while(*digit_addr != NULL) {

        rev_digit = bigdigit_init();

        please_dont_segfault(rev_digit);

        rev_digit->next = prev_addr;
        rev_digit->value = (*digit_addr)->value;

        prev_addr = rev_digit;
        digit_addr = &(*digit_addr)->next;
    }

    rev->sign = num.sign;
    rev->first = prev_addr;
    rev->refs = 1;

    // Attention : les trailing zéros sont importants dans le contexte d'un bignum inversé

    return rev;
}

/**
 * Donne le booléen `abs(a) > abs(b)`.
 * Utilisé pour l'addition.
 */
char bignum_absgt(bignum a, bignum b) {
    // Assume que a et b ont été `bignum_cleanés`

    int size_a = bignum_len(a), size_b = bignum_len(b), i;

    a.sign = b.sign = 0;

    if(size_a != size_b) {
        return size_a > size_b;
    }

    bignum* ar = bignum_rev(a);
    bignum* br = bignum_rev(b);

    bigdigit** ptr_addr_a = &ar->first;
    bigdigit** ptr_addr_b = &br->first;

    for(i = 0; i < size_a; i++) {
        if((*ptr_addr_a)->value > (*ptr_addr_b)->value) {

            bignum_destoroyah(ar);
            bignum_destoroyah(br);

            return TRUE;
        }
        ptr_addr_a = &(*ptr_addr_a)->next;
        ptr_addr_b = &(*ptr_addr_b)->next;
    }

    bignum_destoroyah(ar);
    bignum_destoroyah(br);

    return FALSE;
}

/**
 * Donne le booléen `a > b`.
 */
char bignum_gt(bignum a, bignum b) {

    if(a.sign != b.sign) {
        return a.sign == BIGNUM_POSITIVE;
    }

    if(a.sign == BIGNUM_POSITIVE) {
        bignum_absgt(a, b);
    }

    return bignum_absgt(b, a);
}

/**
 * Donne le booléen `a == b`
 */
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
    int size_a = bignum_len(a), size_b = bignum_len(b), max_size = MAX(size_a, size_b);

    bignum* sum = bignum_init();

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
        digit = bigdigit_init();

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

    *next = NULL;
    bignum_clean(sum);

    return sum;
}

bignum* bignum_sub(bignum a, bignum b) {
    b.sign = !b.sign;
    return bignum_add(a, b);
}

bignum* bignum_dumb_mul(bignum a, bignum b) {
    // Optimisation
    if(bignum_absgt(b, a))
        return bignum_dumb_mul(b, a);

    bignum* prod = bignum_fromstr("0");
    bignum* zero = bignum_fromstr("0");
    bignum* dec = bignum_fromstr("-1");

    char sign = a.sign != b.sign;
    b.sign = 0;
    a.sign = 0;

    while(!bignum_eq(b, *zero)) {
        b = *bignum_add(b, *dec); // Decrement b
        prod = bignum_add(*prod, a);
    }

    bignum_destoroyah(zero);
    bignum_destoroyah(dec);

    prod->sign = sign;

    return prod;
}

/* Splitte un bignum en deux à la moitié de la longueur. Il faut passer deux
  pointeurs Vers ce qui va contenir high et low.
*/
void bignum_split(int split_index ,bignum a, bignum* high, bignum* low) {

    high->sign = a.sign;
    low->sign = a.sign;

    bigdigit* new_digit = NULL;
    bigdigit* prev_new_digit = NULL;
    bigdigit* current_digit = a.first;
    int i = 1;
    while(current_digit != NULL){
        //Copies the current digit.
        new_digit = malloc(sizeof(bignum));
        please_dont_segfault(new_digit);
        new_digit->next = NULL;
        new_digit->value = current_digit->value;
        //if its not the start of high and a previous digit exists, its next is 
        //the new one.
        if(i != split_index+1 && prev_new_digit != NULL){
            prev_new_digit->next = new_digit;
        }
        if(i == 1) {
            low->first = new_digit;
        } else if(i == split_index+1) {
            prev_new_digit = NULL;
            high->first = new_digit;
        }
        prev_new_digit = new_digit;
        current_digit = current_digit->next;
        i++;
    }
}

bignum* bignum_copy(bignum* num) {

    bignum* copy = bignum_init();
    copy->sign = num->sign;

    bigdigit* new_digit = NULL;
    bigdigit* prev_new_digit = NULL;
    bigdigit* current_digit = num->first;
    
    while(current_digit != NULL){
        //Copies the current digit.
        new_digit = malloc(sizeof(bignum));
        please_dont_segfault(new_digit);
        new_digit->next = NULL;
        new_digit->value = current_digit->value;

        if(prev_new_digit != NULL){
            prev_new_digit->next = new_digit;
        }
        prev_new_digit = new_digit;
        current_digit = current_digit->next;
    }
    return copy;
}

/* Arithmetic base 10 left shift on a bignum.*/
void bignum_shift_left(bignum* out, int shamt) {
    bigdigit* zero = NULL;
    bigdigit* old_first = NULL;
    int i;
    for(i=0; i<shamt; i++) {
        zero = malloc(sizeof(bigdigit));
        old_first = out->first;
        zero->next = old_first;
        out->first = zero;
    }
}

/**
 * Algo de karatsuba pour la multiplication de grands entiers
 */
bignum* bignum_mul(bignum a, bignum b) {
    //compute the size of both nums
    int len_a = bignum_len(a);
    int len_b = bignum_len(b);
    //if any of the num is <10, multiply normally
    if( len_a < 2 || bignum_len(b) < 2) {
        return bignum_dumb_mul(a,b);
    }
    int max_middle = MAX(len_a, len_b);
    bignum* high_a = bignum_init();
    bignum* high_b = bignum_init();
    bignum* low_a = bignum_init();
    bignum* low_b = bignum_init();
    bignum_split(max_middle, a, high_a, low_a);
    bignum_split(max_middle, b, high_b, low_b);

    bignum* z0 = bignum_mul(*low_a, *low_b);
    //This is painful to code : (z2*10^(2*m2))+((z1-z2-z0)*10^(m2))+(z0)
    bignum* z1 = bignum_mul(*bignum_add(*low_a, *low_b), *bignum_add(*high_a, *high_b));
    bignum* z2 = bignum_mul(*high_a, *high_b);

    //Saletés de pointeurs, c'est la guerre des étoiles
    bignum* the_res_menace = bignum_copy(z2);
    bignum_shift_left(the_res_menace, 2*max_middle);
    bignum* attack_of_the_res = bignum_sub(*z1, *z2);
    bignum* a_new_res = bignum_sub(*attack_of_the_res, *z0);
    bignum_shift_left(a_new_res, max_middle);
    bignum* res_strikes_back = bignum_add(*a_new_res, *z0);
    bignum* of_the_res = bignum_add(*res_strikes_back, *the_res_menace);

    //Free everything.
    bignum_destoroyah(high_a);
    bignum_destoroyah(high_b);
    bignum_destoroyah(low_a);
    bignum_destoroyah(low_b);
    bignum_destoroyah(z0);
    bignum_destoroyah(z1);
    bignum_destoroyah(z2);
    bignum_destoroyah(the_res_menace);
    bignum_destoroyah(attack_of_the_res);
    bignum_destoroyah(a_new_res);
    bignum_destoroyah(res_strikes_back);

    return of_the_res;
}
