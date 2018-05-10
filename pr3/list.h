#include "stable.h"

typedef struct Node{

 char *key;
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
 struct Lista *pos[];

}stable_s;

Lista* lista_create();

void lista_destroy(Lista *lista);

void lista_insert(Lista *lista, char *key, EntryData val);

void lista_print(Lista *lista);
