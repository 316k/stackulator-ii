#include <string.h>
#define SWP(x,y) (x^=y, y^=x, x^=y)
#define FALSE 0
#define TRUE 1

#define please_dont_segfault(stuff) \
    if(stuff == NULL) { \
        printf("FIN DE MÉMOIRE. FUYEZ !! FUYEZ PAUVRES FOUS !! (%s at line %d)\n", __func__, __LINE__); \
        exit(-1); \
    }

#define MIN(a, b) a <= b ? a : b
#define MAX(a, b) a >= b ? a : b

// see https://stackoverflow.com/questions/198199/how-do-you-reverse-a-string-in-place-in-c-or-c
void strrev(char *p) {
    char *q = p;
    while(q && *q) ++q; /* find eos */
    for(--q; p < q; ++p, --q)
        SWP(*p, *q);
}
