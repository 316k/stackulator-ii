#include <string.h>
#define FALSE 0
#define TRUE 1

#define please_dont_segfault(stuff) \
    if(stuff == NULL) { \
        printf("FIN DE MÉMOIRE. FUYEZ !! FUYEZ PAUVRES FOUS !! (%s at line %d)\n", __func__, __LINE__); \
        exit(-1); \
    }

#define MIN(a, b) (a <= b ? a : b)
#define MAX(a, b) (a >= b ? a : b)
