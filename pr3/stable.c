#include <stdio.h>
#include <stdlib.h>
#include "stable.h"
#include "list.h"

// pega uma string e converte ela para um inteiro,
// unsigned pra caber caso a string seja mto grande
static unsigned int convert(char* s) {
    unsigned int k = 0;
    for(int i = 0; s[i] != '\0'; i++) {
        // base 179, porque é um número primo completamente arbitrario
        k = k * 179 + s[i];
    }
    return k;
}

// hash, até agora com um m qualquer que ela recebe,
// esperançosamente presumindo que ele é primo
static int hash(char* chave, int m) {
    unsigned int chv = convert(chave);
    return chv%m;
}

SymbolTable stable_create(int n, int m) {

    SymbolTable ST = malloc(n * sizeof(stable_s)); // como faz isso pls halp
    for (int i = 0; i < n; i++)
        ST->pos[i] = lista_create();
    

    ST->n = n;
    ST->m = m;

    return ST;
}

void stable_destroy(SymbolTable table) {

    for (int i = table->n-1; i > -1; i--)
        lista_destroy(table->pos[i]);

    free(table);
}

InsertionResult stable_insert(SymbolTable table, const char *key) {
    InsertionResult result;
    // faz a inserçao de vdd
    return result;
}

EntryData *stable_find(SymbolTable table, const char *key) {
    EntryData *data;
    // procura de vdd
    // se nao achou, return NULL;
    return data;
}

int stable_visit(SymbolTable table, int (*visit)(const char *key, EntryData *data)) {
    return 0;
}
