void assert(char exp, int line) {
    if(exp != 1) {
        printf("\n** Assertion failed ** : see line %d\n", line);
    }
}

void test_bignum_nextdigit() {
    bignum num;
    
    num.sign = 0;
    num.first = NULL;
    
    bigdigit* digit = bignum_nextdigit(&num);
    
    assert(num.first == digit, __LINE__);
    
    bigdigit newdigit;
    
    newdigit.value = 4;
    newdigit.next = NULL;
    
    num.first = &newdigit;
    
    printf("%x %x", &newdigit, &num.first, bignum_nextdigit(&num));
    assert(bignum_nextdigit(&num)->value == newdigit.value, __LINE__);
}

