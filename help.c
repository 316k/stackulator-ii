#include <string.h>
#define FALSE 0
#define TRUE 1

#define please_dont_segfault(stuff) \
    if(stuff == NULL) { \
        printf("FIN DE LA MÉMOIRE. FUYEZ !! FUYEZ PAUVRES FOUS !! (%s, ligne %d)\n", __func__, __LINE__); \
        exit(-1); \
    }

#define MIN(a, b) (a <= b ? a : b)
#define MAX(a, b) (a >= b ? a : b)
