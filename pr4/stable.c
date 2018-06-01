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
    return chv%m;
}

SymbolTable stable_create(int m) {

    SymbolTable ST = malloc(sizeof(stable_s));
    ST->pos = malloc(m * sizeof(Lista));
    for (int i = 0; i < m; i++)
        ST->pos[i] = lista_create();

    ST->n = 0;
    ST->m = m;

    return ST;
}

void stable_destroy(SymbolTable table) {

    for (int i = table->m-1; i > -1; i--)
        lista_destroy(table->pos[i]);

    free(table->pos);
    free(table);
}

InsertionResult stable_insert(SymbolTable table, const char *key) {
    InsertionResult result;
    int idx = hash(key, table->m);

    // começa com val 1 caso seja uma nova palavra
    EntryData val;
    val.i = 1;

    result = lista_insert(table->pos[idx], key, val);

    // se for nova, atualiza o tamanho da ST
    if (result.new != 0)
        table->n++;

    return result;
}

EntryData *stable_find(SymbolTable table, const char *key) {
    EntryData *data;
    int idx = hash(key, table->m);

    // se nao tiver nenhuma chave com esse hash, retorna null
    if (table->pos[idx]->root == NULL) return NULL;

    Node *p = table->pos[idx]->root;

    // percorre a lista com o hash da chave
    while (p != NULL) {
        if (strcmp(key, p->key) == 0) {
            data = &p->val;
            return data;
        }
        p = p->next;
    }
    // retorna null se nao está lá
    return NULL;
}

int stable_visit(SymbolTable table, int (*visit)(const char *key, EntryData *data)) {

    visit = print_node;

    for (int i = 0; i < table->m; i++) {

        if (table->pos[i]->root != NULL) {
            Node *p = table->pos[i]->root;

            while (p != NULL) {
                EntryData *d = &p->val;
                if (visit(p->key, d) == 0)
                    return  EXIT_FAILURE;
                p = p->next;
            }
        }
    }
    return EXIT_SUCCESS;
}
