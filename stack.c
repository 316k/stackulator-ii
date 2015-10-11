// Requires bignum*.c to be included first

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

void stack_dump(stack* s) {
    int i;
    stack_cell* cell = s->top;

    printf("-- Stack dump --\n");
    for(i = 0; i < stack_len(s); i++) {
        printf("%ld: %s\n", stack_len(s) - i, bignum_tostr(*cell->element));
        cell = cell->next_cell;
    }
}
