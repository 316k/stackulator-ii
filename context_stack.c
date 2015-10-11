// Requires circular_list*.c to be included first

typedef struct context_stack_cell context_stack_cell;

struct context_stack_cell {
    circular_list* element;
    context_stack_cell* next_cell;
};

struct context_stack {
    context_stack_cell* top;
    long len;
};

typedef struct context_stack context_stack;

long context_stack_len(context_stack* s) {
    return s->len;
}

void context_stack_push(context_stack* s, circular_list* element){
    context_stack_cell* new_cell = malloc(sizeof(context_stack_cell));

    please_dont_segfault(new_cell);

    new_cell->element = element;
    new_cell->next_cell = s->top;
    s->top = new_cell;
    s->len++;
}

circular_list* context_stack_pop(context_stack* s) {
    context_stack_cell* popped = s->top;
    s->top = popped->next_cell;
    circular_list* value = popped->element;

    free(popped); //like a bird.

    s->len--;

    return value;
}

circular_list* context_stack_peek(context_stack* s) {
    return s->top->element;
}

int context_stack_empty(context_stack* s) {
    return s->top == NULL;
}

context_stack* context_stack_init() {
    context_stack* s = malloc(sizeof(context_stack));

    please_dont_segfault(s);

    s->top = NULL;
    s->len = 0;

    return s;
}
