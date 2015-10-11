// Circular list of chars
typedef struct circular_list_cell circular_list_cell;

struct circular_list_cell {
    char element;
    circular_list_cell* next;
    circular_list_cell* previous;
};

struct circular_list {
    circular_list_cell* current;
};

typedef struct circular_list circular_list;

//Créé une liste circulaire vide.
circular_list* circular_list_init() {
    circular_list* list = malloc(sizeof(circular_list));
    please_dont_segfault(list);
    list-> current = NULL;
    return list;
}
//Ajoute un élément à la fin (relative) de la liste.
void circular_list_append(circular_list* list, char elem){
    circular_list_cell* cell = malloc(sizeof(circular_list_cell));
    please_dont_segfault(cell);
    cell->element = elem;
    //Si la liste est vide, son élément courrant devient celui-ci.
    //précédent et suivant sont == à lui même.
    if(list->current == NULL){
        list->current = cell;
        
        cell->previous = cell;
        cell->next = cell;
    //Si la liste n'est pas vide, on insère l'élément à la fin relative de la liste
    //C-A-D derrière l'élément courrant.
    } else {
        cell->next = list->current;
        list->current->previous->next = cell;
    }
}
//Donne l'élément courrant et avance l'élément courrant.
char circular_list_next(circular_list* list){
    char elem = list->current->element;
    list->current = list->current->next;
    return elem;
}
//Désalloue la mémoire de la liste.
void circular_list_destoroyah(circular_list* list){
    //si la liste est vide, libérer seulement la liste.
    if(list->current == NULL){
        free(list);
        return;
    //si la liste ne contient qu'une seule cellule.
    } else if (list->current == list->current->next){
        free(list->current);
        free(list);
    } else{
        //set le next du dernier élément comme NULL
        list->current->previous->next = NULL;
        //Cellule actuelle
        circular_list_cell* current = list->current;
        //Trouve le prochain
        circular_list_cell* next = list->current->next;
        //Free les shitz tant qu'on est pas au dernier elem.
        while(next != NULL){
            free(current);
            current = next;
            next = current->next;
        }
    }
}
