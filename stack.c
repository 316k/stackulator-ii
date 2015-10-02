// Requires bignum.c to be included first

typedef struct {
    char element;
    char *next_element;
} stack_cell;

typedef struct {
    stack_cell *top;
} stack;

void push(stack *the_stack ,char element){
    stack_cell new_cell = {element, the_stack->top};
    the_stack->top = &new_cell;
}

int main(){
    return EXIT_SUCCESS;
}
