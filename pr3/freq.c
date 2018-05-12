#include "list.h"
#include "stable.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    SymbolTable ST = stable_create(17);

    // EntryData d;
    // d.i = 12;

	stable_insert(ST, "0");
    stable_insert(ST, "9");
    stable_insert(ST, "abc");
    stable_insert(ST, "89wrtu9b");
    stable_insert(ST, "89wrb");
    // lista_print(ST->pos[0]);
    // lista_insert(ST->pos[0], "abc", d);
    // lista_print(ST->pos[0]);
    char *a[ST->n];
    int b = 0;
    for(int i = 0; i<ST->m; i++){
        lista_getall(ST->pos[i],a,b);
        b += ST->pos[i]->size;
    }

    printf("\n");
    int max = -1;

    for(int i = 0; i<ST->n; i++){
        if((int)(strlen(a[i]))>max)
            max = strlen(a[i]);
    }

    //ALGUEM ORDENA AQUI PLS

    int diff = 0;

    for(int i = 0; i<ST->n; i++){
        diff = max - (int)(strlen(a[i]));
        printf("%s ", a[i]);
        for(int j = 0; j<diff; j++)
            printf(" ");
        printf("%d\n", 10);//<------------------------ get aqui
    }

    printf("\n");
    // lista_insert(ST->pos[0], "oof", d);
    // lista_print(ST->pos[0]);
    printf("kek\n");
    stable_destroy(ST);

    return 0;
}
