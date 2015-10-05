#include <string.h>

char max(int a, int b) {
    return a >= b ? a : b;
}

char* strpad(char str[], char pad, int size) {

    if(strlen(str) > size)
        return str;

    char* out = malloc(sizeof(char) * size + 1);
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
