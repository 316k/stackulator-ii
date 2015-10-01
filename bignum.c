// Note : little-endian numbers
struct bignum {
    short sign,
    bigchiffre chiffre
};

struct bigchiffre {
    short valeur,
    bigchiffre* next
};

typedef struct bignum bignum;

