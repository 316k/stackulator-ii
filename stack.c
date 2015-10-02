// Requires bignum.c to be included first

typedef struct stack_cell stack_cell;

struct stack_cell {
    char element;
    stack_cell *next_element;
};

struct stack {
    stack_cell *top;
};

typedef struct stack stack;

void push(stack *stack ,char element){
    stack_cell *new_cell = malloc(sizeof(stack_cell));
    new_cell->element = element;
    new_cell->next_element = stack->top;
    stack->top = new_cell;
}

char pop(stack *stack) {
    stack_cell *popped = stack->top;
    stack->top = popped->next_element;
    char value = popped->element;
    free(popped); //like a bird.
    return value;
}

char peek(stack *stack) {
    return stack->top->element;
}

char empty(stack *stack) {
    return stack->top == NULL;
}

stack* new_stack() {
    stack *stack = malloc(sizeof(stack));
    stack->top = NULL;
    return stack;
}
