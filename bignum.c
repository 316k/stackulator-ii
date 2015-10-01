// Note : little-endian numbers
struct bigchiffre {
    char valeur;
    struct bigchiffre* next;
};
typedef struct bigchiffre bigchiffre;

struct bignum {
    char sign;
    struct bigchiffre chiffre;
};
typedef struct bignum bignum;

