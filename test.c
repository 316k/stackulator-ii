#include <string.h>
#define ASSERT(expr) test_all_works = test_all_works && assert(expr, __func__, __LINE__)

// Oui oui, c'est une variable globale. Vous avez un problème avec ça ?
char test_all_works = TRUE;

char assert(char exp, const char* func, int line) {
    if(exp != 1) {
        printf("** %s ** : Assertion failed in test.c line %d\n", func, line);
    }

    return exp;
}

void test_bignum_tostr() {
    bignum num;
    bigdigit d1, d2, d3, d0;

    d3.value = 3;
    d3.next = NULL;
    d2.value = 2;
    d2.next = &d3;
    d1.value = 1;
    d1.next = &d2;

    num.sign = 0;
    num.first = &d1;

    ASSERT(strcmp(bignum_tostr(num), "121") != 0);
    ASSERT(strcmp(bignum_tostr(num), "321") == 0);

    num.sign = BIGNUM_NEGATIVE;

    ASSERT(strcmp(bignum_tostr(num), "-321") == 0);

    d0.next = NULL;
    d0.value = 0;
    num.first = &d0;
    ASSERT(strcmp(bignum_tostr(num), "0") == 0);
}

void test_bignum_fromstr() {
    bignum* num = bignum_fromstr("12345");

    ASSERT(strcmp(bignum_tostr(*num), "12345") == 0);

    num = bignum_fromstr("-337");
    ASSERT(strcmp(bignum_tostr(*num), "-337") == 0);

    bignum* a = bignum_fromstr("5");
    ASSERT(strcmp(bignum_tostr(*a), "5") == 0);
}

void test_bignum_clean() {
    bignum* a = bignum_fromstr("0000012345");
    bignum* b = bignum_fromstr("0012345");
    bignum* c = bignum_fromstr("-0012345");
    bignum* d = bignum_fromstr("-12345");

    bignum_clean(a);
    bignum_clean(b);
    ASSERT(strcmp(bignum_tostr(*a), bignum_tostr(*b)) == 0);
    bignum_clean(c);
    bignum_clean(d);

    ASSERT(strcmp(bignum_tostr(*c), bignum_tostr(*d)) == 0);

    bignum* e = bignum_fromstr("000000000000000000000");
    bignum* f = bignum_fromstr("-0");
    bignum_clean(e);
    bignum_clean(f);
    ASSERT(strcmp(bignum_tostr(*e), bignum_tostr(*bignum_fromstr("0"))) == 0);
    ASSERT(strcmp(bignum_tostr(*f), bignum_tostr(*bignum_fromstr("0"))) == 0);
}

void test_bignum_destoroyah() {
    bignum* d = bignum_fromstr("-123325");
    bignum_destoroyah(d);
    // Si ça segfault, bah faut croire que le test passe pas
}

void test_bignum_rev() {
    bignum* a = bignum_fromstr("12345");
    bignum* ar = bignum_fromstr("54321");
    bignum* b = bignum_fromstr("-12345");
    bignum* br = bignum_fromstr("-54321");

    ASSERT(strcmp(bignum_tostr(*bignum_rev(*a)), bignum_tostr(*ar)) == 0);
    ASSERT(strcmp(bignum_tostr(*bignum_rev(*b)), bignum_tostr(*br)) == 0);
}

void test_bignum_gt() {

    bignum* h = bignum_fromstr("-10");
    bignum* i = bignum_fromstr("-20");
    bignum* j = bignum_fromstr("10");
    bignum* k = bignum_fromstr("20");
    bignum* z = bignum_fromstr("0");

    // bignum_absgt tests
    ASSERT(bignum_absgt(*i, *h));
    ASSERT(bignum_absgt(*i, *j));
    ASSERT(bignum_absgt(*h, *z));
    ASSERT(bignum_absgt(*k, *z));

    ASSERT(!bignum_absgt(*h, *i));
    ASSERT(!bignum_absgt(*j, *i));
    ASSERT(!bignum_absgt(*z, *h));
    ASSERT(!bignum_absgt(*z, *k));

}

void test_bignum_add() {
    bignum* a = bignum_fromstr("12345");
    bignum* b = bignum_fromstr("123");
    bignum* sum = bignum_fromstr("12468");

    ASSERT(bignum_eq(*bignum_add(*a, *b), *sum));
    sum->sign = 1;
    bignum* c = bignum_fromstr("-12345");
    bignum* d = bignum_fromstr("-123");

    ASSERT(bignum_eq(*bignum_add(*c, *d), *bignum_fromstr("-12468")));
    ASSERT(!bignum_eq(*bignum_add(*c, *d), *bignum_fromstr("12468")));

    bignum* e = bignum_fromstr("-10");
    bignum* f = bignum_fromstr("5");
    bignum* g = bignum_fromstr("-5");

    ASSERT(bignum_eq(*bignum_add(*e, *f), *bignum_fromstr("-5")));
    ASSERT(bignum_eq(*bignum_add(*f, *e), *bignum_fromstr("-5")));
    ASSERT(bignum_eq(*bignum_add(*f, *g), *bignum_fromstr("0")));

    bignum* h = bignum_fromstr("99");
    bignum* i = bignum_fromstr("1");

    ASSERT(bignum_eq(*bignum_add(*h, *i), *bignum_fromstr("100")));

    bignum* j = bignum_fromstr("124908901283901283019283124908901283901283019283");
    bignum* k = bignum_fromstr("838972389238961265356241838972389238961265356241");

    ASSERT(bignum_eq(*bignum_add(*j, *k), *bignum_fromstr("963881290522862548375524963881290522862548375524")));

    bignum* l = bignum_fromstr("0");

    ASSERT(bignum_eq(*bignum_add(*k, *l), *k));

    ASSERT(bignum_eq(*bignum_add(*bignum_fromstr("0"), *bignum_fromstr("0")), *bignum_fromstr("0")));

    bignum* m = bignum_fromstr("000001");
    bignum* n = bignum_fromstr("000002");
    ASSERT(bignum_eq(*bignum_add(*n, *m), *bignum_fromstr("3")));
}

void test_bignum_mul() {

    bignum* a = bignum_fromstr("1");
    bignum* b = bignum_fromstr("2");
    bignum* c = bignum_fromstr("3");
    bignum* d = bignum_fromstr("6");
    bignum* e = bignum_fromstr("1287632");
    bignum* f = bignum_fromstr("7725792");
    bignum* k = bignum_fromstr("9947977004544");

    // Trivial
    ASSERT(bignum_eq(*bignum_mul(a, a), *a));

    // Simples
    ASSERT(bignum_eq(*bignum_mul(a, b), *b));
    ASSERT(bignum_eq(*bignum_mul(a, d), *d));
    ASSERT(bignum_eq(*bignum_mul(b, c), *d));
    // Gros nombres
    ASSERT(bignum_eq(*bignum_mul(d, e), *f));
    // Gros Gros nombres
    // ASSERT(bignum_eq(*bignum_mul(*e, *f), *k));
    // Nombres négatifs
    bignum* g = bignum_fromstr("-2");
    bignum* h = bignum_fromstr("-3");
    bignum* i = bignum_fromstr("-6");

    ASSERT(bignum_eq(*bignum_mul(g, c), *i));
    ASSERT(bignum_eq(*bignum_mul(c, g), *i));
    ASSERT(bignum_eq(*bignum_mul(g, h), *d));
    ASSERT(bignum_eq(*bignum_mul(h, g), *d));
}

void test_bignum_split() {
    bignum* a = bignum_fromstr("6666888");
    bignum* b = bignum_fromstr("6666");
    bignum* c = bignum_fromstr("888");

    bignum* high = bignum_init();
    bignum* low = bignum_init();

    bignum_split(3, *a, high, low);

    ASSERT(bignum_eq(*high, *b));
    ASSERT(bignum_eq(*low, *c));

    bignum* d = bignum_fromstr("-6666888");
    bignum* e = bignum_fromstr("-6666");
    bignum* f = bignum_fromstr("-888");

    high = bignum_init();
    low = bignum_init();

    bignum_split(3, *d, high, low);

    ASSERT(bignum_eq(*high, *e));
    ASSERT(bignum_eq(*low, *f));


    bignum* g = bignum_fromstr("668");
    bignum* h = bignum_fromstr("66");
    bignum* i = bignum_fromstr("8");

    high = bignum_init();
    low = bignum_init();

    bignum_split(3, *g, high, low);

    ASSERT(bignum_eq(*high, *h));
    ASSERT(bignum_eq(*low, *i));
}

void test_bignum_shift_left() {
    bignum* a = bignum_fromstr("10");
    bignum* b = bignum_fromstr("10000");
    bignum_shift_left(a, 3);

    ASSERT(bignum_eq(*a, *b));
}

void test_bignum_copy() {
    bignum* a = bignum_fromstr("6666888");
    bignum* b = bignum_copy(a);

    ASSERT(bignum_eq(*a, *b));

    bignum_shift_left(a, 3);
    bignum_shift_left(b, 3);

    ASSERT(bignum_eq(*a, *b));
}

void test_stack_push() {
    stack* s = stack_init();
    bignum* a = bignum_fromstr("8");
    bignum* b = bignum_fromstr("7");

    stack_push(s, a);
    ASSERT(bignum_eq(*s->top->element, *a));

    stack_push(s, b);
    ASSERT(bignum_eq(*s->top->element, *b));
}

void test_stack_peek() {
    stack* s = stack_init();
    bignum* a = bignum_fromstr("8");
    stack_push(s, a);

    ASSERT(bignum_eq(*stack_peek(s), *a));
}

void test_stack_pop() {
    stack* s = stack_init();

    bignum* a = bignum_fromstr("8");
    bignum* b = bignum_fromstr("9");
    bignum* c = bignum_fromstr("34");
    bignum* d = bignum_fromstr("-34");

    stack_push(s, a);
    stack_push(s, b);
    stack_push(s, c);
    stack_push(s, d);

    ASSERT(bignum_eq(*stack_pop(s), *d));
    ASSERT(bignum_eq(*stack_pop(s), *c));
    ASSERT(bignum_eq(*stack_pop(s), *b));
    ASSERT(bignum_eq(*stack_pop(s), *a));
}

void test_stack_empty() {
    stack* s = stack_init();
    bignum* a = bignum_fromstr("8");

    ASSERT(stack_empty(s));

    stack_push(s, a);
    stack_pop(s);

    ASSERT(stack_empty(s));
}

void test_stack_len() {
    stack* s = stack_init();
    bignum* a = bignum_fromstr("8");

    ASSERT(stack_len(s) == 0);

    stack_push(s, a);

    ASSERT(stack_len(s) == 1);

    stack_push(s, a);

    ASSERT(stack_len(s) == 2);

    stack_pop(s);

    ASSERT(stack_len(s) == 1);

    stack_pop(s);

    ASSERT(stack_len(s) == 0);
}

void test_stress_stack() {
    stack* s = stack_init();

    int i;
    for(i=0; i<25555; i++) {
        ASSERT(stack_len(s) == i);
        char str[] = "00000";
        sprintf(str, "%3d", i);
        bignum* a = bignum_fromstr(str);
        stack_push(s, a);

    }

    for(i=0; i<25555; i++) {
        ASSERT(stack_len(s) == 25555-i);
        char str[] = "00000";
        sprintf(str, "%3d", 25554-i);
        ASSERT(bignum_eq(*stack_pop(s), *bignum_fromstr(str)));
    }
}

char test_all() {
    // bignum.c
    test_bignum_tostr();
    test_bignum_fromstr();
    test_bignum_clean();
    test_bignum_destoroyah();
    test_bignum_rev();
    test_bignum_gt();
    test_bignum_add();
	test_bignum_mul();
	test_bignum_split();
	test_bignum_copy();
	test_bignum_shift_left();
    // stack.c
    test_stack_push();
    test_stack_peek();
    test_stack_pop();
    test_stack_empty();
    test_stack_len();
    test_stress_stack();

    return test_all_works;
}
