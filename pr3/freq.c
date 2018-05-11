#include "list.h"
#include "stable.h"
#include <stdio.h>


int main(int argc, char **argv) {
    SymbolTable ST = stable_create(10,17);

    // EntryData d;
    // d.i = 12;

    stable_insert(ST, "abc");
    stable_insert(ST, "89wrtu9b");
    stable_insert(ST, "89wrb");
    stable_insert(ST, "89wrb");
    // lista_print(ST->pos[0]);
    // lista_insert(ST->pos[0], "abc", d);
    // lista_print(ST->pos[0]);
    printf("\n");
    // lista_insert(ST->pos[0], "oof", d);
    // lista_print(ST->pos[0]);
    printf("kek\n");
    stable_destroy(ST);

    return 0;
}
