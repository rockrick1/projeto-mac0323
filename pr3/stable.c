#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stable.h"
#include "list.h"

// pega uma string e converte ela para um inteiro,
// unsigned pra caber caso a string seja mto grande
static unsigned int convert(const char* s) {
    unsigned int k = 0;
    for(int i = 0; s[i] != '\0'; i++) {
        // base 179, porque é um número primo completamente arbitrario
        k = k * 179 + s[i];
    }
    return k;
}

// hash, até agora com um m qualquer que ela recebe,
// esperançosamente presumindo que ele é primo
static int hash(const char* chave, int m) {
    unsigned int chv = convert(chave);
    if(strcmp(chave,"0")==0)
        return 0;
    return chv%m;
}

SymbolTable stable_create(int m) {

    SymbolTable ST = malloc(sizeof(SymbolTable)); // como faz isso pls halp
    ST->pos = malloc(m * sizeof(Lista));
    for (int i = 0; i < m; i++)
        ST->pos[i] = lista_create();

    lista_print(ST->pos[0]);
    printf("uhm\n");
    stable_insert(ST, "0");

    ST->n = 0;
    ST->m = m;

    return ST;
}

void stable_destroy(SymbolTable table) {

    for (int i = table->m-1; i > -1; i--) {
        printf("%d\n", i);
        lista_destroy(table->pos[i]);
    }

    free(table->pos);
    free(table);
}

InsertionResult stable_insert(SymbolTable table, const char *key) {
    InsertionResult result;
    int idx = hash(key, table->m);
    printf("hash de %s: %d\n", key, idx);

    EntryData val;
    val.i = 1;

    result = lista_insert(table->pos[idx], key, val);

    table->n++;

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
