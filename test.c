#include <string.h>
#define ASSERT(expr) assert(expr, __func__, __LINE__)


char assert(char exp, const char* func, int line) {
    if(exp != 1) {
        printf("** %s ** : Assertion failed in test.c line %d\n", func, line);
    }

    return exp != 1;
}

void test_strpad() {
    ASSERT(strcmp(strpad("1230000", '0', 3), "1230000") == 0);
    ASSERT(strcmp(strpad("123", '0', 5), "00123") == 0);
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
}

void test_bignum_mul() {

    bignum* a = bignum_fromstr("1");
    bignum* b = bignum_fromstr("2");
    bignum* c = bignum_fromstr("3");
    bignum* d = bignum_fromstr("6");
    bignum* e = bignum_fromstr("124908901283901283019283");
    // bignum* f = bignum_fromstr("1498906815406815396231396");

    ASSERT(bignum_eq(*bignum_mul(*a, *a), *a));

    ASSERT(bignum_eq(*bignum_mul(*a, *b), *b));
    ASSERT(bignum_eq(*bignum_mul(*a, *d), *d));
    ASSERT(bignum_eq(*bignum_mul(*b, *c), *d));
    printf("%s", bignum_tostr(*bignum_mul(*d, *e)));
    /*
    ASSERT(bignum_eq(*bignum_mul(*d, *e), *f));
    */
}

void test_stack_push() {
    stack *s = new_stack();

    push(s, 8);
    ASSERT(s->top->element == 8 );

    push(s, 7);
    ASSERT(s->top->element == 7);
}

void test_stack_peek(){
    stack *s = new_stack();

    push(s, 8);
    ASSERT(s->top->element == 8);
}

void test_stack_pop(){
    stack *s = new_stack();

    push(s,8);
    push(s,9);
    push(s,34);
    push(s,-34);

    ASSERT(pop(s) == -34 );
    ASSERT(pop(s) == 34 );
    ASSERT(pop(s) == 9 );
    ASSERT(pop(s) == 8 );
}

void test_stack_empty(){
    stack *s = new_stack();

    ASSERT(empty(s));

    push(s,-34);
    pop(s);

    ASSERT(empty(s));
}

void test_all() {
    // help.c
    test_strpad();
    // bignum.c
    test_bignum_tostr();
    test_bignum_fromstr();
    test_bignum_clean();
    test_bignum_gt();
    test_bignum_add();
    /*
	test_bignum_mul();
	*/
    // stack.c
    test_stack_push();
    test_stack_peek();
    test_stack_pop();
    test_stack_empty();
}
