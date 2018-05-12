#include "stable.h"
#include <stdio.h>

typedef struct Node{

 const char *key;
 EntryData val;
 struct Node *next;

}Node;

typedef struct Lista{

 int size;
 Node *root;

}Lista;

typedef struct stable_s{

 int n;
 int m;
 struct Lista **pos;

}stable_s;

Lista* lista_create();

void lista_destroy(Lista *lista);

InsertionResult lista_insert(Lista *lista, const char *key, EntryData val);

void lista_print(Lista *lista);
