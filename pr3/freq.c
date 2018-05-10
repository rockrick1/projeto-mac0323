#include "stable.h"

int main(int argc, char **argv) {
    SymbolTable ST = stable_create();

    EntryData d;

    d.i = 2;
    lista_insert(ST[0], "abc", d);
    lista_print(ST[0]);
    stable_destroy(ST);
    return 0;
}