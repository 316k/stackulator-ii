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

void push(stack *the_stack ,char element){
    stack_cell new_cell = {element, the_stack->top};
    the_stack->top = &new_cell;
}

