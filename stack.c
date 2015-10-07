// Requires bignum*.c to be included first

typedef struct stack_cell stack_cell;

struct stack_cell {
    bignum* element;
    stack_cell *next_cell;
};

struct stack {
    stack_cell *top;
};

typedef struct stack stack;

void stack_push(stack *stack ,bignum* element){
    stack_cell *new_cell = malloc(sizeof(stack_cell));
    new_cell->element = element;
    new_cell->next_cell = stack->top;
    stack->top = new_cell;
}

bignum* stack_pop(stack *stack) {
    stack_cell *popped = stack->top;
    stack->top = popped->next_cell;
    bignum* value = popped->element;
    free(popped); //like a bird.
    return value;
}

bignum* stack_peek(stack *stack) {
    return stack->top->element;
}

int stack_empty(stack *stack) {
    return stack->top == NULL;
}

stack* stack_init() {
    stack *stack = malloc(sizeof(stack));
    stack->top = NULL;
    return stack;
}
