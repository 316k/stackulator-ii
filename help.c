#include <string.h>
#define SWP(x,y) (x^=y, y^=x, x^=y)
#define FALSE 0
#define TRUE 1

#define please_dont_segfault(stuff) \
    if(stuff == NULL) { \
        printf("FIN DE MÉMOIRE. FIN DE TOUTE. FUYEZ !! FUYEZ PAUVRES FOUS !! (%s at line %d)\n", __func__, __LINE__); \
        exit(-1); \
    }



char min(int a, int b) {
    return a <= b ? a : b;
}

char max(int a, int b) {
    return a >= b ? a : b;
}

char* strpad(char str[], char pad, int size) {

    if(strlen(str) > size)
        return str;

    char* out = malloc(sizeof(char) * size + 1);

    please_dont_segfault(out);


    int i;
    int pad_size = size - strlen(str);

    for(i = 0; i < size + 1; i++) {
        if(i < pad_size) {
            out[i] = pad;
        } else {
            out[i] = str[i - pad_size];
        }
    }

    out[size + 1] = '\0';
    return out;
}

// see https://stackoverflow.com/questions/198199/how-do-you-reverse-a-string-in-place-in-c-or-c
void strrev(char *p) {
    char *q = p;
    while(q && *q) ++q; /* find eos */
    for(--q; p < q; ++p, --q)
        SWP(*p, *q);
}
