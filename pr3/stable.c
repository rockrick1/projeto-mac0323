#include <stdio.h>
#include <stdlib.h>
#include "stable.h"

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

SymbolTable stable_create() {
    int n = 100;
    SymbolTable ST = malloc(n * sizeof(EntryData)); // temporario(?)
    return ST;
}

void stable_destroy(SymbolTable table) {
    free(table);
}

InsertionResult stable_insert(SymbolTable table, const char *key) {
    InsertionResult result = malloc(sizeof(InsertionResult));
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

}