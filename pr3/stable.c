#include <stdio.h>
#include <stdlib.h>
#include "stable.h"

static unsigned int convert(char* s) {
    unsigned int k = 0;
    for(int i = 0; s[i] != '\0'; i++) {
        k = k * 179 + s[i];
    }
    return k;
}

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