#include "stable.h"

typedef struct{

 char *key;
 EntryData val;
 struct Node *next;

}Node;

typedef struct{

 int size;
 Node *root;
	
}Lista;

Lista* lista_create();

void lista_destroy(Lista *lista);

void lista_insert(Lista *lista, char *key, EntryData val);

void lista_print(Lista *lista);