// Un contexte est une liste circulaire de char.
typedef struct context_cell context_cell;

struct context_cell {
    char element;
    context_cell* next;
    context_cell* previous;
};

struct context {
    context_cell* current;
    char type;
};

typedef struct context context;

char CONTEXT_LOOP = 1;
char CONTEXT_PROC = 0;

//Créé une liste circulaire vide.
context* context_init(int type) {
    context* list = malloc(sizeof(context));
    please_dont_segfault(list);
    list-> current = NULL;
    list-> type = type;
    return list;
}
//Ajoute un élément à la fin (relative) de la liste.
void context_append(context* list, char elem) {
    context_cell* cell = malloc(sizeof(context_cell));
    please_dont_segfault(cell);
    cell->element = elem;
    //Si la liste est vide, son élément courrant devient celui-ci.
    //précédent et suivant sont == à lui même.
    if(list->current == NULL) {
        list->current = cell;

        cell->previous = cell;
        cell->next = cell;
    //Si la liste n'est pas vide, on insère l'élément à la fin relative de la liste
    //C-A-D derrière l'élément courrant.
    } else {
        cell->next = list->current;
        cell->previous = list->current->previous;
        list->current->previous->next = cell;
        list->current->previous = cell;
    }
}
//Donne l'élément courrant et avance l'élément courrant.
char context_next(context* list) {
    char elem = list->current->element;
    list->current = list->current->next;
    return elem;
}
//Désalloue la mémoire de la liste.
void context_destoroyah(context* list) {
    //si la liste est vide, libérer seulement la liste.
    if(list->current == NULL) {
        free(list);
        return;
    //si la liste ne contient qu'une seule cellule.
    } else if (list->current == list->current->next) {
        free(list->current);
        free(list);
    } else {
        //set le next du dernier élément comme NULL
        list->current->previous->next = NULL;
        //Cellule actuelle
        context_cell* current = list->current;
        //Trouve le prochain
        context_cell* next = list->current->next;
        //Free les shitz tant qu'on est pas au dernier elem.
        while(current != NULL){
            next = current->next;
            free(current);
            current = next;
        }
        free(list);
    }
}
