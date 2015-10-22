#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// include help.c

#define FALSE 0
#define TRUE 1

#define please_dont_segfault(stuff) \
    if(stuff == NULL) { \
        printf("FIN DE LA MÉMOIRE. FUYEZ !! FUYEZ PAUVRES FOUS !! (%s, ligne %d)\n", __func__, __LINE__); \
        exit(-1); \
    }

#define MIN(a, b) (a <= b ? a : b)
#define MAX(a, b) (a >= b ? a : b)

// include context.c
// Un contexte est une liste circulaire de char.
typedef struct context_cell context_cell;

struct context_cell {
    char element;
    context_cell* next;
    context_cell* previous;
};

struct context {
    context_cell* current;
    char type;
};

typedef struct context context;

char CONTEXT_LOOP = 1;
char CONTEXT_PROC = 0;

// Créé une liste circulaire vide.
context* context_init(int type) {
    context* list = malloc(sizeof(context));
    please_dont_segfault(list);
    list-> current = NULL;
    list-> type = type;
    return list;
}

// Ajoute un élément à la fin (relative) de la liste.
void context_append(context* list, char elem) {
    context_cell* cell = malloc(sizeof(context_cell));
    please_dont_segfault(cell);
    cell->element = elem;
    //Si la liste est vide, son élément courrant devient celui-ci.
    //précédent et suivant sont == à lui même.
    if(list->current == NULL) {
        list->current = cell;

        cell->previous = cell;
        cell->next = cell;
    //Si la liste n'est pas vide, on insère l'élément à la fin relative de la liste
    //C-A-D derrière l'élément courrant.
    } else {
        cell->next = list->current;
        cell->previous = list->current->previous;
        list->current->previous->next = cell;
        list->current->previous = cell;
    }
}

// Donne l'élément courrant et avance l'élément courrant.
char context_next(context* list) {
    char elem = list->current->element;
    list->current = list->current->next;
    return elem;
}

// Désalloue la mémoire de la liste.
void context_destoroyah(context* list) {
    // si la liste est vide, libérer seulement la liste.
    if(list->current == NULL) {
        free(list);
        return;
    // si la liste ne contient qu'une seule cellule.
    } else if (list->current == list->current->next) {
        free(list->current);
        free(list);
    } else {
        // set le next du dernier élément comme NULL
        list->current->previous->next = NULL;
        // Cellule actuelle
        context_cell* current = list->current;
        // Trouve le prochain
        context_cell* next = list->current->next;
        // Free les shitz tant qu'on est pas au dernier élément
        while(current != NULL){
            next = current->next;
            free(current);
            current = next;
        }
        free(list);
    }
}

// include context_stack.c
typedef struct context_stack_cell context_stack_cell;

struct context_stack_cell {
    context* element;
    context_stack_cell* next_cell;
};

struct context_stack {
    context_stack_cell* top;
    long len;
};

typedef struct context_stack context_stack;

long context_stack_len(context_stack* s) {
    return s->len;
}

void context_stack_push(context_stack* s, context* element){
    context_stack_cell* new_cell = malloc(sizeof(context_stack_cell));

    please_dont_segfault(new_cell);

    new_cell->element = element;
    new_cell->next_cell = s->top;
    s->top = new_cell;
    s->len++;
}

context* context_stack_pop(context_stack* s) {
    context_stack_cell* popped = s->top;
    s->top = popped->next_cell;
    context* value = popped->element;

    free(popped); //like a bird.

    s->len--;

    return value;
}

context* context_stack_peek(context_stack* s) {
    return s->top->element;
}

int context_stack_empty(context_stack* s) {
    return s->top == NULL;
}

context_stack* context_stack_init() {
    context_stack* s = malloc(sizeof(context_stack));

    please_dont_segfault(s);

    s->top = NULL;
    s->len = 0;

    return s;
}

// include bignum.c
#define BIGNUM_NEGATIVE 1
#define BIGNUM_POSITIVE 0

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

bignum* bignum_copy(bignum* num) {

    bignum* copy = bignum_init();
    copy->sign = num->sign;

    bigdigit* new_digit = NULL;
    bigdigit* current_digit = num->first;
    bigdigit** prev = &copy->first;

    while(current_digit != NULL) {
        // Copies the current digit.
        new_digit = bigdigit_init();
        new_digit->value = current_digit->value;

        current_digit = current_digit->next;

        *prev = new_digit;
        prev = &new_digit->next;
    }

    return copy;
}

/*
 * Donne la taille du bignum (le nombre de digits, sans le signe '-')
 */
int bignum_len(bignum num) {
    long len = 0;
    bigdigit* ptr_addr = num.first;

    while(ptr_addr != NULL) {
        len++;
        ptr_addr = ptr_addr->next;
    }

    return len;
}

/**
 * Donne la représentation en char* d'un bignum
 */
char* bignum_tostr(bignum num) {
    // Assume que num a été `bignum_cleané`

    int len = bignum_len(num), i;
    bigdigit* digit_addr = num.first;

    char* out = malloc(sizeof(char) * (len + 1 + num.sign));
    please_dont_segfault(out);

    if(num.sign) {
        out[0] = '-';
    }

    for(i = len + num.sign; i > num.sign; i--) {
        out[i-1] = digit_addr->value + '0';
        digit_addr = digit_addr->next;
    }

    out[len + num.sign] = '\0';

    return out;
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
 * Construit un bignum avec le code ASCII du char c
 */
bignum* bignum_fromchar(char c) {
    int i;

    bignum* num = bignum_init();
    num->sign = BIGNUM_POSITIVE;

    bigdigit* digit = NULL;
    bigdigit** next = &num->first;

    for(i = 0; c; i++) {
        digit = bigdigit_init();

        digit->value = (c % 10);
        c /= 10;

        *next = digit;
        next = &digit->next;
    }

    *next = NULL;

    bignum_clean(num);

    return num;
}

/**
 * Donne la représentation en char* d'un bignum
 */
char bignum_tochar(bignum num) {
    // Assume que num a été `bignum_cleané`

    int mul = 1;
    bigdigit* digit_addr = num.first;

    char out = 0;

    while(digit_addr != NULL) {
        out += mul * digit_addr->value;
        mul *= 10;
        digit_addr = digit_addr->next;
    }

    return out;
}

/**
 * Donne la représentation en char* d'un bignum
 */
void bignum_dump(bignum* num) {
    bigdigit* digit_addr = num->first;

    printf("--Dumping num at %p--\n", num);
    while(digit_addr != NULL) {
        printf("%d at %p (next: %p)\n", digit_addr->value, digit_addr, digit_addr->next);
        digit_addr = digit_addr->next;
    }
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

    bigdigit* ptr_a = ar->first;
    bigdigit* ptr_b = br->first;

    for(i = 0; i < size_a; i++) {
        if(ptr_a->value != ptr_b->value) {

            char ret = ptr_a->value > ptr_b->value;

            bignum_destoroyah(ar);
            bignum_destoroyah(br);

            return ret;
        }

        ptr_a = ptr_a->next;
        ptr_b = ptr_b->next;
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
        return bignum_absgt(a, b);
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
        if(bignum_absgt(b, a)) {
            bignum_destoroyah(sum);
            return bignum_add(b, a);
        }

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

    bignum* sum = bignum_add(a, b);

    return sum;
}

bignum* bignum_dumb_mul(bignum a, bignum b) {
    // Optimisation
    if(bignum_absgt(b, a))
        return bignum_dumb_mul(b, a);

    bignum* prod = bignum_fromstr("0");
    bignum* zero = bignum_fromstr("0");
    bignum* dec = bignum_fromstr("-1");

    char sign = a.sign != b.sign;

    bignum* b_copy = bignum_copy(&b);
    b_copy->sign = 0;
    bignum* add_result = NULL;
    bignum* prod_result = NULL;

    a.sign = 0;

    while(!bignum_eq(*b_copy, *zero)) {
        add_result = bignum_add(*b_copy, *dec);
        bignum_destoroyah(b_copy);
        b_copy = add_result;

        prod_result = bignum_add(*prod, a);
        bignum_destoroyah(prod);
        prod = prod_result;
    }

    bignum_destoroyah(b_copy);
    bignum_destoroyah(zero);
    bignum_destoroyah(dec);

    prod->sign = sign;

    return prod;
}

/* Splitte un bignum en deux à la moitié de la longueur. Il faut passer deux
  pointeurs vers ce qui va contenir high et low.
*/
void bignum_split(bignum a, int split_index, bignum* high, bignum* low) {

    high->sign = a.sign;
    low->sign = a.sign;

    bigdigit* new_digit = NULL;
    bigdigit* prev_new_digit = NULL;
    bigdigit* current_digit = a.first;
    int i = 1;

    while(current_digit != NULL) {
        new_digit = bigdigit_init();
        new_digit->value = current_digit->value;

        // Première fois
        if(i == 1) {
            low->first = new_digit;
        } else if(i == split_index + 1) {
            prev_new_digit = NULL;
            high->first = new_digit;
        }

        if(i != split_index+1 && prev_new_digit != NULL) {
            prev_new_digit->next = new_digit;
        }

        prev_new_digit = new_digit;
        current_digit = current_digit->next;
        i++;
    }

    if(high->first == NULL) {
        high->first = bigdigit_init();
    }

    if(low->first == NULL) {
        low->first = bigdigit_init();
    }

    bignum_clean(high);
    bignum_clean(low);
}

/* Arithmetic base 10 left shift on a bignum.*/
void bignum_shift_left(bignum* out, int shamt) {
    bigdigit* zero;
    bigdigit* old_first;
    int i;

    for(i=0; i < shamt; i++) {
        zero = bigdigit_init();

        old_first = out->first;
        zero->next = old_first;
        out->first = zero;
    }
    //clean le résultat.
    bignum_clean(out);
}
/* Arithmetic base 10 right shift on a bignum */
void bignum_shift_right(bignum* out, int shamt) {
    int i;
    int len;
    bigdigit* out_first;
    for(i=0; (i < shamt) && (len = bignum_len(*out)) > 0; i++) {
        if(len == 1){
            out->first->value = 0;
            continue;
        }
        //prend le deuxième, free le premier, met le deuxième comme premier.
        out_first = out->first;
        out->first = out_first->next;
        free(out_first);
    }

    //clean le résultat.
    bignum_clean(out);
}

/**
 * Algo de karatsuba pour la multiplication de grands entiers
 */
bignum* bignum_mul(bignum a, bignum b) {
    int len_a = bignum_len(a);
    int len_b = bignum_len(b);

    // Multiplication stupide pour les petits nombres
    if(len_a < 2 || len_b < 2) {
        return bignum_dumb_mul(a, b);
    }
    int max = MAX(len_a, len_b);
    int max_middle = max/2;

    bignum* high_a = bignum_init();
    bignum* high_b = bignum_init();
    bignum* low_a = bignum_init();
    bignum* low_b = bignum_init();

    bignum_split(a, max-max_middle, high_a, low_a);
    bignum_split(b, max-max_middle, high_b, low_b);

    bignum* z2 = bignum_mul(*high_a, *high_b);
    bignum* z0 = bignum_mul(*low_a, *low_b);

    // Je voudrais de l'operator overloading : (z2*10^(max))+((z1-z2-z0)*10^(max_middle))+(z0)
    bignum* sum_a = bignum_add(*low_a, *high_a);
    bignum* sum_b = bignum_add(*low_b, *high_b);

    bignum_destoroyah(high_a);
    bignum_destoroyah(high_b);
    bignum_destoroyah(low_a);
    bignum_destoroyah(low_b);

    // z1 = (sum_a*sum_b) - z2 - z0
    bignum* mul_of_sum = bignum_mul(*sum_a, *sum_b);
    bignum* diff_a = bignum_sub(*mul_of_sum,*z2);
    bignum* z1 = bignum_sub(*diff_a, *z0);

    bignum_destoroyah(mul_of_sum);
    bignum_destoroyah(diff_a);
    bignum_destoroyah(sum_a);
    bignum_destoroyah(sum_b);

    //arrondir pour avoir la bonne puissance de 10 dans les shifts.
    float inter = (float)max;
    inter = inter/2.0f;
    inter += 0.5f;
    max_middle = (int) inter;
    if(max%2 == 1){
        max++;
    }

    //r1 = z2*10^(max)
    bignum* r1 = bignum_copy(z2);
    bignum_shift_left(r1, max);

    //r2 = z1
    bignum* r2 = bignum_copy(z1);
    //r2 = r2*10^(max_middle)
    bignum_shift_left(r2, max_middle);
    //r3 = r2 + z0
    bignum* r3 = bignum_add(*r2, *z0);

    //bignum_destoroyah(z0);
    bignum_destoroyah(r2);
    //rf = r1+r3
    bignum* rf = bignum_add(*r1, *r3);

    bignum_destoroyah(r1);
    bignum_destoroyah(r3);

    bignum_destoroyah(z0);
    bignum_destoroyah(z1);
    bignum_destoroyah(z2);
    return rf;
}

// include stack.c
typedef struct stack_cell stack_cell;

struct stack_cell {
    bignum* element;
    stack_cell* next_cell;
};

struct stack {
    stack_cell* top;
    long len;
};

typedef struct stack stack;

long stack_len(stack* s) {
    return s->len;
}

void stack_push(stack* s, bignum* element){
    stack_cell* new_cell = malloc(sizeof(stack_cell));

    please_dont_segfault(new_cell);

    new_cell->element = element;
    new_cell->next_cell = s->top;
    s->top = new_cell;
    s->len++;
}

bignum* stack_pop(stack* s) {
    stack_cell* popped = s->top;
    s->top = popped->next_cell;
    bignum* value = popped->element;

    free(popped); //like a bird.

    s->len--;

    return value;
}

bignum* stack_peek(stack* s) {
    return s->top->element;
}

int stack_empty(stack* s) {
    return s->top == NULL;
}

stack* stack_init() {
    stack* s = malloc(sizeof(stack));

    please_dont_segfault(s);

    s->top = NULL;
    s->len = 0;

    return s;
}

void stack_reverse(stack** s) {

    stack* t = stack_init();
    stack* old_s = *s;

    while(!stack_empty(*s)) {
        stack_push(t, stack_pop(*s));
    }

    *s = t;
    free(old_s);
}

void stack_dump(stack* s) {
    int i;
    char* str;
    stack_cell* cell = s->top;

    printf("-- Stack dump --\n");
    for(i = 0; i < stack_len(s); i++) {
        str = bignum_tostr(*cell->element);
        printf("%ld: %s\n", stack_len(s) - i, str);
        free(str);
        cell = cell->next_cell;
    }
}

void prompt(char interactive_mode, stack* s) {
    if(interactive_mode == 1) {
        printf("> ");
    } else if(interactive_mode == 2) {
        if(stack_empty(s)) {
            printf("0:(nil)> ");
        } else {
            char* str = bignum_tostr(*stack_peek(s));
            printf("%ld:%s> ", stack_len(s), str);
            free(str);
        }
    }
}

/**
 * Donne la prochaine instruction à exécuter
 */
char get_next(FILE* in, context_stack* c_s, char* saved){
    /* Dans le cas ou saved est NULL, on lit l'entrée de l'utilisateur et
    il est impossible qu'il y ait de char sauvés.*/
    if(saved != NULL && *saved != -1){
        char val = *saved;
        *saved = -1;
        return val;
    }
    if(context_stack_empty(c_s)){
        return getc(in);
    }

    return context_next(context_stack_peek(c_s));
}

char push_number(char c, stack* s, FILE* in, context_stack* c_s, char* saved) {
    bignum* num = bignum_init();

    bigdigit* digit = NULL;
    bigdigit* prev_addr = NULL;

    // Si ça commence avec 0, on veut potentiellement garder le premier seulement
    if(c == '0') {
        digit = bigdigit_init();

        prev_addr = digit;

        while((c = get_next(in, c_s, saved)) == '0');

        // Si ça commençait avec des 0 non-significatifs, on les flush
        if(c >= '1' && c <= '9') {
            digit->value = c - '0';
            c = get_next(in, c_s, saved);
        }
    }

    while(c >= '0' && c <= '9') {

        digit = bigdigit_init();

        digit->next = prev_addr;
        digit->value = c - '0';

        prev_addr = digit;

        c = get_next(in, c_s, saved);
    }

    /* Si c'est un autre char que space et newline, sauve le pour la
       prochaine évaluation. Si saved est NULL, l'entrée utilisateur
       est lu et pour éviter l'injection de code, on ne sauve pas 
       le dernier char.*/
    if(c != ' ' && c != '\n' && saved != NULL) {
        *saved = c;
    }

    num->first = prev_addr;

    stack_push(s, num);

    return c;
}

char push_op(char c, stack* s, bignum* (*fct)(bignum, bignum)) {
    if(stack_len(s) < 2) {
        fprintf(stderr, "%c nécessite deux opérandes, taille de la pile insuffisante\n", c);
        return c;
    }

    bignum* a = stack_pop(s);
    bignum* b = stack_pop(s);

    bignum* num = (*fct)(*b, *a);
    stack_push(s, num);

    bignum_destoroyah(a);
    bignum_destoroyah(b);

    return c;
}

void push_shift(char c, stack* s) {
    if(stack_len(s) < 1) {
        fprintf(stderr, "%c nécessite une opérandes, taille de la pile insuffisante\n", c);
        return;
    }
    bignum* popped = stack_pop(s);
    bignum* shifted = bignum_copy(popped);
    if(c == '{') {
        bignum_shift_left(shifted, 1);
    } else {
        bignum_shift_right(shifted, 1);
    }
    stack_push(s, shifted);
    bignum_destoroyah(popped);
}

char push_test(char c, stack* s) {
    if(stack_len(s) < 2) {
        fprintf(stderr, "Une comparaison booléenne nécessite deux opérandes, taille de la pile insuffisante\n");
        return c;
    }

    bignum* a = stack_pop(s);
    bignum* b = stack_pop(s);

    bignum* num = bignum_fromstr("0");
    bignum* zero = bignum_fromstr("0");

    switch(c) {
        case '<':
            num->first->value = bignum_gt(*a, *b) && !bignum_eq(*a, *b);
            break;
        case '>':
            num->first->value = bignum_gt(*b, *a);
            break;
        case '=':
            num->first->value = bignum_eq(*a, *b);
            break;
        case '|':
            num->first->value = !bignum_eq(*a, *zero) || !bignum_eq(*b, *zero);
            break;
        case '&':
            num->first->value = !bignum_eq(*a, *zero) && !bignum_eq(*b, *zero);
            break;
        default: break;
    }

    stack_push(s, num);

    bignum_destoroyah(a);
    bignum_destoroyah(zero);
    bignum_destoroyah(b);

    return c;
}

char push_ternary(char c, stack* s) {
    if(stack_len(s) < 3) {
        fprintf(stderr, "La ternaire (?) nécessite trois opérandes, taille de la pile insuffisante\n");
        return c;
    }

    bignum* zero = bignum_fromstr("0");

    bignum* condition = stack_pop(s);
    bignum* v_false = stack_pop(s);
    bignum* v_true = stack_pop(s);

    if(!bignum_eq(*condition, *zero)) {
        stack_push(s, v_true);
        bignum_destoroyah(v_false);
    } else {
        stack_push(s, v_false);
        bignum_destoroyah(v_true);
    }

    bignum_destoroyah(zero);
    bignum_destoroyah(condition);

    return c;
}

char assign_var(char c, stack* s, bignum* variables[]) {
    if(c < 'a' || c > 'z') {
        fprintf(stderr, "Le nom de variable `%c` est erroné\n", c);
        return c;
    }

    // Si le stack est vide, on met (ou on laisse) NULL dans la variable
    if(stack_empty(s)) {
        if(variables[c - 'a']) {
            bignum_destoroyah(variables[c - 'a']);
        }

        variables[c - 'a'] = NULL;
        return c;
    }

    // Empêche d'incrémenter refs avec `100 =a =b =a`
    if(variables[c - 'a'] != stack_peek(s)) {
        variables[c - 'a'] = stack_peek(s);
        stack_peek(s)->refs++;
    }

    return c;
}

char push_var(char c, stack* s, bignum* variables[]) {
    if(variables[c - 'a'] == NULL) {
        fprintf(stderr, "La variable `%c` n'a pas été définie\n", c);
        return c;
    }

    stack_push(s, variables[c - 'a']);
    variables[c - 'a']->refs++;

    return c;
}

context* create_context(FILE* in, context_stack* c_s, char* saved, char type, char enter, char exit) {
    context* context = context_init(type);

    // Pour supporter les contextes dans des contextes (ex.: loop dans une loop)
    int enter_count = 1;
    char c;
    int quote_count = 0;
    char escape_char = 0;
    char comment = 0;
    while(enter_count) {
        c = get_next(in, c_s, saved);

        quote_count = quote_count % 2;
        //Si on est pas dans un commentaire
        if (!comment) {
            /* Si la qtée de quotes lus est paire et que c'est un symbole d'entrée.*/
            if (c == enter && !quote_count) {
                enter_count++;
            /* Si la qtée de quotes lus est paire et que c'est un symbole de sortie.*/
            } else if (c == exit && !quote_count) {
                enter_count--;
            /* Incrémente quote_count sauf quand on est dans une string et que
            ça a été escapé...*/
            } else if (c == '"' && !escape_char) {
                quote_count++;
            } else if (c == '#' && !quote_count) {
                comment = 1;
            }
            // Si on est dans une string.
            if(quote_count) {
                // et qu'on escape, on escape plus après.
                if(escape_char) {
                    escape_char--;
                // si on escape pas et qu'on a '\', on escape le prochain.
                } else if (c == '\\') {
                    escape_char++;
                }
            }
        } else if(c == '\n') {
            comment = 0;
        }

        context_append(context, c);
    }
    return context;
}

int main(int argc, char* argv[]) {
    int i, j;
    FILE* in = stdin;
    char c = ' ', waiting, interactive_mode = TRUE;
    stack* s = stack_init();
    context_stack* c_s = context_stack_init();
    // Char sauvegardé dans le cas ou un char est inputté directement après un nombre.
    char saved = -1;
    // Variables
    bignum* variables[26];

    // Procédures
    context* procedures[26];

    bignum* zero = bignum_fromstr("0");

    //Set toutes les variables et procédures à NULL
    for(i=0; i<26; i++) {
        variables[i] = NULL;
        procedures[i] = NULL;
    }

    // Parse les arguments
    for(i = 1; i < argc && argv[i][0] == '-'; i++) {
        if(strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--silent") == 0) {
            interactive_mode = FALSE;
        } else if(strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
            interactive_mode = 2; // verbose
        } else if(strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            printf("usage : stackulator [options]\n");
            printf("stackulator is a postfix stack machine that evaluates stdin into stdout\n");
            printf("options :\n");
            printf("    -s, --silent : don't show '>' prompt character\n");
            printf("    -v, --verbose : show useful stuff instead of just the '>' prompt character\n");
            printf("    -h, --help : displays help\n");
            return EXIT_SUCCESS;
        }
    }

    // Fichier en entrée
    if(i < argc) {
        in = fopen(argv[i], "r");
        if(in == NULL) {
            fprintf(stderr, "Impossible d'ouvrir le fichier %s\n", argv[i]);
            return EXIT_FAILURE;
        }
        interactive_mode = FALSE;
    }

    if(interactive_mode) {
        printf("-- Stackulator v1.0 --\n");
        printf("Entrez Ctrl+D pour quitter\n");
    }

    prompt(interactive_mode, s);

    while((c = get_next(in, c_s, &saved)) != EOF) {
        waiting = FALSE;

        // Push un nouveau nombre sur la pile
        if(c >= '0' && c <= '9') {

            c = push_number(c, s, in, c_s, &saved);
            waiting = c == '\n';

        // Addition
        } else if(c == '+') {
            push_op(c, s, bignum_add);
        // Soustraction
        } else if(c == '-') {
            push_op(c, s, bignum_sub);
        // Multiplication
        } else if(c == '*') {
            push_op(c, s, bignum_mul);
        // Extra : inversion booléenne
        } else if(c == '!') {
            if(stack_len(s) < 1) {
                fprintf(stderr, "L'inversion booléenne (!) nécessite une opérandes, taille de la pile insuffisante\n");
                continue;
            }
            bignum* num = stack_pop(s);
            bignum* val = bignum_fromstr("1");

            if(!bignum_eq(*num, *zero)) {
                val->first->value = 0;
            }

            bignum_destoroyah(num);

            stack_push(s, val);
        // Assignation & test d'égalité
        } else if(c == '=') {
            char c = get_next(in, c_s, &saved);

            // Extra : Test d'égalité
            if(c == '=') {
                push_test(c, s);
                continue;
            }

            assign_var(c, s, variables);

        // Assignation explicite de NULL dans une variable
        } else if(c == '_') {
            char c = get_next(in, c_s, &saved);

            stack* empty_stack = stack_init();
            assign_var(c, empty_stack, variables);
            free(empty_stack);

        // Push une variable sur la pile
        } else if(c >= 'a' && c <= 'z') {
            push_var(c, s, variables);
        // Ignore les espaces
        } else if(c == ' ') {
        // Extra : commentaires. Ignore tout ce qu'il y a entre # et \n
        } else if(c == '#') {
            while(c != '\n'){
                c = get_next(in, c_s, &saved);
            }
        // affiche le top du stack : ^ (ou \n en mode interactif)
        } else if(c == '^' || (c == '\n' && interactive_mode)) {

            if(!stack_empty(s)) {
                char* str = bignum_tostr(*stack_peek(s));
                printf("%s", str);

                if(interactive_mode){
                    printf("\n");
                }

                free(str);

            } else if(interactive_mode) {
                printf("Stack vide\n");
            }

            waiting = c == '\n';
        // Extra : affichage en char du bignum sur le top de la pile
        } else if(c == '/') {

            if(!stack_empty(s)) {
                char ascii_val = bignum_tochar(*stack_peek(s));
                printf("%c", ascii_val);

                if(interactive_mode){
                    printf("\n");
                }

            } else if(interactive_mode) {
                printf("Stack vide\n");
            }

            waiting = c == '\n';
        // Ignore le \n si le mode interactif est off
        } else if(c == '\n') {
        // Extra : comparateurs booléens &, |, > et <
        } else if(c == '>' || c == '<' || c == '&' || c == '|') {

            push_test(c, s);

        // Extra : opérateur ternaire
        } else if(c == '?') {
            push_ternary(c, s);
        // Extra : duplique le top du stack
        } else if(c == '@') {
            if(stack_empty(s)) {
                fprintf(stderr, "La copie nécessite au moins un élément sur la pile\n");
                continue;
            }
            stack_push(s, bignum_copy(stack_peek(s)));
        // Extra : Shifts arithmétiques.
        } else if(c == '{' || c == '}') {
            push_shift(c, s);
        // Extra : Début de loop.
        } else if(c == '[') {
            //Le contexte est créé inconditionellement car il faut faire avancer
            //le contexte dans lequel la boucle est déclarée jusqu'a la fin
            //de la boucle dans les deux cas.
            context* loop_context = create_context(in, c_s, &saved, CONTEXT_LOOP, '[', ']');
            if(stack_empty(s) || bignum_eq(*stack_peek(s), *zero) ) {
                context_destoroyah(loop_context);
                continue;
            }

            context_stack_push(c_s, loop_context);
        // Fin de boucle.
        } else if(c == ']') {
            if(context_stack_empty(c_s) || context_stack_peek(c_s)->type != CONTEXT_LOOP){
                fprintf(stderr, "Instruction de sortie de boucle hors d'une boucle.\n");
                continue;
            }
            /* Si on est dans une loopet que le top du stack est existant et 
            positif, continue comme ça */
            if(context_stack_peek(c_s)->type == CONTEXT_LOOP && !stack_empty(s) && bignum_absgt(*stack_peek(s), *zero) ) {
                continue;
            }
            // Sinon, tue le contexte.
            context_destoroyah(context_stack_pop(c_s));
        // Extra : début d'une procédure.
        } else if(c == ':') {
            if(!context_stack_empty(c_s)) {
                fprintf(stderr, "Impossible de définir une procédure depuis une boucle ou une procédure\n");
            }

            c = get_next(in, c_s, &saved);
            if(c < 'A' || c > 'Z') {
                fprintf(stderr, "Le nom de procédure `%c` est erroné\n", c);
                continue;
            }

            context* procedure_context = create_context(in, c_s, &saved, CONTEXT_PROC, ':', ';');

            if(procedures[c - 'A'] != NULL){
                context_destoroyah(procedures[c - 'A']);
            }

            procedures[c - 'A'] = procedure_context;

        // Extra : le retour d'une procédure (seulement si on est dans une proc.)
        } else if(c == ';') {
            if(context_stack_empty(c_s) || context_stack_peek(c_s)->type != CONTEXT_PROC){
                fprintf(stderr, "Instruction de sortie de procédure hors d'une définition de procédure.\n");
                continue;
            }
            context_stack_pop(c_s);
        // Extra : L'appel d'une procédure
        } else if(c >= 'A' && c <= 'Z') {
            if(procedures[c - 'A'] == NULL){
                fprintf(stderr, "La procédure `%c` n'est pas définie.\n", c);
                continue;
            }
            context_stack_push(c_s, procedures[c - 'A']);
        // Extra : dump le contenu du stack
        } else if(c == '$') {
            stack_dump(s);
        // Extra : dump le contenu des variables (adresses des bignums référencés)
        } else if(c == '%') {
            printf("-- Variables --\n");
            for(i = 0; i < 5; i++) {
                for(j = 0; j < 5; j++) {
                    printf("%c: %9p |", 'a' + i * 5 + j, variables[i * 5 + j]);
                }
                printf("\n");
            }
            printf("%c: %9p\n", 'z', variables[25]);
        // Extra : pop le top
        } else if(c == '.') {
           if(!stack_empty(s)) {
                bignum_destoroyah(stack_pop(s));
            }
        // Extra : lis un nombre depuis stdin
        } else if(c == ',') {

            c = getc(stdin);
            while(c == '\n' || c == ' ') {
                c = getc(stdin);
            }

            char negative = 0;

            if(c == '-') {
                negative = 1;
                c = getc(stdin);
            }

            while(c < '0' || c > '9') {
                c = getc(stdin);
            }

            //Si des char sont lu de l'entrée, il ne faut pas les prendres dans le contexte courrant.
            context_stack* empty_stack = context_stack_init();
            /* On passe NULL comme saved car on ne veut pas que l'utilisateur
               puisse injecter du code en mettant une opération comme dernier
               char de son entrée.
            */
            c = push_number(c, s, stdin, empty_stack, NULL);
            // Si le dernier char lu n'est pas ' ' ou '\n', on avertis l'utilisateur
            // que ce char a été ignoré.
            if(c != ' ' && c != '\n') {
                printf("Attention : les suffixes de nombres ne sont pas pris en compte ('%c' ignoré)", c);
            }
            free(empty_stack);

            stack_peek(s)->sign = negative;
            waiting = TRUE;

        // Extra : lis un char depuis l'entrée standard
        } else if(c == '`') {

            c = getc(stdin);

            bignum* input_char = bignum_fromchar(c);

            stack_push(s, input_char);
            waiting = TRUE;

        // Extra : reverse le stack
        } else if(c == '\'') {

            stack_reverse(&s);

        // Extra : echo
        } else if(c == '"') {

            char last_char = c;
            c = get_next(in, c_s, &saved);

            while((c != '"' || last_char == '\\') && c != EOF) {
                if(last_char != '\\' && c == '\\') {
                    last_char = c;
                    c = get_next(in, c_s, &saved);
                    continue;
                }

                printf("%c", c);
                last_char = c;
                c = get_next(in, c_s, &saved);
            }

            waiting = c == '\n';
        // Extra : vide le stack
        } else if(c == '~') {
            while(!stack_empty(s)) {
                bignum_destoroyah(stack_pop(s));
            }
        // Gestion des expressions inconnues
        } else {
            fprintf(stderr, "Expression inconnue ignorée : %c", c);
            while(c != ' ' && c != '\n') {
                c = get_next(in, c_s, &saved);
                fprintf(stderr, "%c", c);
            }
            fprintf(stderr, "\n");
            waiting = c == '\n';
        }

        if(waiting) {
            prompt(interactive_mode, s);
        }
    }

    while(!stack_empty(s)) {
        bignum_destoroyah(stack_pop(s));
    }

    free(s);

    while(!context_stack_empty(c_s)) {
        context_destoroyah(context_stack_pop(c_s));
    }

    free(c_s);

    for(i=0; i<26; i++) {
        if(variables[i] != NULL) {
            bignum_destoroyah(variables[i]);
        }
        if(procedures[i] != NULL) {
            context_destoroyah(procedures[i]);
        }
    }

    bignum_destoroyah(zero);
    if(in != stdin) {
        fclose(in);
    }

    return EXIT_SUCCESS;
}
