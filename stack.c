// Requires bignum.c to be included first

typedef struct stack_cell stack_cell;
typedef struct smallnum smallnum;

struct smallnum {
    char element;
};

struct stack_cell {
    smallnum element;
    stack_cell *next_cell;
};

struct stack {
    stack_cell *top;
};

typedef struct stack stack;

void push(stack *stack ,smallnum element){
    stack_cell *new_cell = malloc(sizeof(stack_cell));
    new_cell->element = element;
    new_cell->next_cell = stack->top;
    stack->top = new_cell;
}

smallnum pop(stack *stack) {
    stack_cell *popped = stack->top;
    stack->top = popped->next_cell;
    smallnum value = popped->element;
    free(popped); //like a bird.
    return value;
}

smallnum peek(stack *stack) {
    return stack->top->element;
}

int empty(stack *stack) {
    return stack->top == NULL;
}

stack* new_stack() {
    stack *stack = malloc(sizeof(stack));
    stack->top = NULL;
    return stack;
}
