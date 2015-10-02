char assert(char exp, int line) {
    if(exp != 1) {
        printf("\n** Assertion failed ** : see line %d\n", line);
    }

    return exp != 1;
}

void test_bignum_nextdigit() {
    bignum num;
    
    num.sign = 0;
    num.first = NULL;
    
    bigdigit* digit = *bignum_nextdigit(&num);
    
    assert(num.first == digit, __LINE__);
    
    bigdigit newdigit;
    
    newdigit.value = 4;
    newdigit.next = NULL;
    
    num.first = &newdigit;
    
    assert(bignum_nextdigit(&num) == &newdigit.next, __LINE__);
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
    stack *s = new_stack
}
