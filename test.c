#include <string.h>

char assert(char exp, int line) {
    if(exp != 1) {
        printf("** Assertion failed ** in test.c line %d\n", line);
    }

    return exp != 1;
}

void test_strpad() {
    assert(strcmp(strpad("1230000", '0', 3), "1230000") == 0, __LINE__);
    assert(strcmp(strpad("123", '0', 5), "00123") == 0, __LINE__);
}

void test_bignum_nextdigit_addr() {
    bignum num;

    num.sign = 0;
    num.first = NULL;

    bigdigit* digit = *bignum_nextdigit_addr(&num);

    assert(num.first == digit, __LINE__);

    bigdigit newdigit;

    newdigit.value = 4;
    newdigit.next = NULL;

    num.first = &newdigit;

    assert(bignum_nextdigit_addr(&num) == &newdigit.next, __LINE__);
}

void test_bignum_tostr() {
    bignum num;
    bigdigit d1, d2, d3;

    d3.value = 3;
    d3.next = NULL;
    d2.value = 2;
    d2.next = &d3;
    d1.value = 1;
    d1.next = &d2;

    num.sign = 0;
    num.first = &d1;

    assert(strcmp(bignum_tostr(&num), "121") != 0, __LINE__);
    assert(strcmp(bignum_tostr(&num), "321") == 0, __LINE__);

    num.sign = BIGNUM_NEGATIVE;
    assert(strcmp(bignum_tostr(&num), "-321") == 0, __LINE__);
}

void test_bignum_fromstr() {
    bignum* num = bignum_fromstr("12345");

    assert(strcmp(bignum_tostr(num), "12345") == 0, __LINE__);

    num = bignum_fromstr("-337");
    assert(strcmp(bignum_tostr(num), "-337") == 0, __LINE__);
}

void test_bignum_gt() {
    bignum* a = bignum_fromstr("12345");
    bignum* b = bignum_fromstr("123");
    bignum* c = bignum_fromstr("123");
    bignum* d = bignum_fromstr("123456");

    assert(bignum_gt(a, b), __LINE__);
    assert(!bignum_gt(b, a), __LINE__);
    assert(!bignum_gt(b, c), __LINE__);
    assert(!bignum_gt(c, b), __LINE__);
    assert(!bignum_gt(a, a), __LINE__);
    assert(bignum_gt(d, a), __LINE__);

    bignum* e = bignum_fromstr("-123");
    bignum* f = bignum_fromstr("123456");
    bignum* g = bignum_fromstr("-123456");

    assert(!bignum_gt(e, f), __LINE__);
    assert(bignum_gt(f, e), __LINE__);
    assert(bignum_gt(f, g), __LINE__);

    bignum* h = bignum_fromstr("-10");
    bignum* i = bignum_fromstr("-20");
    bignum* j = bignum_fromstr("10");
    bignum* k = bignum_fromstr("20");
    bignum* z = bignum_fromstr("0");

    assert(bignum_gt(h, i), __LINE__);
    assert(bignum_gt(j, i), __LINE__);
    assert(!bignum_gt(j, k), __LINE__);
    assert(bignum_gt(k, z), __LINE__);
    assert(!bignum_gt(h, z), __LINE__);
    assert(bignum_gt(z, i), __LINE__);

    // bignum_absgt tests
    assert(bignum_absgt(*i, *h), __LINE__);
    assert(bignum_absgt(*i, *j), __LINE__);
    assert(bignum_absgt(*h, *z), __LINE__);
    assert(bignum_absgt(*k, *z), __LINE__);
}

void test_bignum_add() {
    bignum* a = bignum_fromstr("12345");
    bignum* b = bignum_fromstr("123");
    bignum* sum = bignum_fromstr("12468");

    assert(bignum_eq(bignum_add(a, b), sum), __LINE__);


    sum->sign = 1;
    bignum* c = bignum_fromstr("-12345");
    bignum* d = bignum_fromstr("-123");

    assert(bignum_eq(bignum_add(c, d), bignum_fromstr("-12468")), __LINE__);
    assert(!bignum_eq(bignum_add(c, d), bignum_fromstr("12468")), __LINE__);

    bignum* e = bignum_fromstr("-10");
    bignum* f = bignum_fromstr("5");
    bignum* g = bignum_fromstr("-5");

    assert(bignum_eq(bignum_add(e, f), bignum_fromstr("-5")), __LINE__);
    assert(bignum_eq(bignum_add(f, e), bignum_fromstr("-5")), __LINE__);
    assert(bignum_eq(bignum_add(f, g), bignum_fromstr("0")), __LINE__);
}

void test_stack_push() {
    stack *s = new_stack();

    push(s, 8);
    assert(s->top->element == 8 , __LINE__);

    push(s, 7);
    assert(s->top->element == 7, __LINE__);
}

void test_stack_peek(){
    stack *s = new_stack();

    push(s, 8);
    assert(s->top->element == 8, __LINE__);
}

void test_stack_pop(){
    stack *s = new_stack();

    push(s,8);
    push(s,9);
    push(s,34);
    push(s,-34);

    assert(pop(s) == -34 , __LINE__);
    assert(pop(s) == 34 , __LINE__);
    assert(pop(s) == 9 , __LINE__);
    assert(pop(s) == 8 , __LINE__);
}

void test_stack_empty(){
    stack *s = new_stack();

    assert(empty(s), __LINE__);

    push(s,-34);
    pop(s);

    assert(empty(s), __LINE__);
}

void test_all() {

    // help.c
    test_strpad();
    // bignum.c
    test_bignum_nextdigit_addr();
    test_bignum_tostr();
    test_bignum_fromstr();
    test_bignum_gt();
    test_bignum_add();
    // stack.c
    test_stack_push();
    test_stack_peek();
    test_stack_pop();
    test_stack_empty();
}
