#include "list.h"
#include <stdio.h>

Lista* lista_create(){

	Lista* lista = malloc(sizeof(Lista));

	lista->root = NULL;
	lista->size = 0;

	return lista;

}

void lista_destroy(Lista *lista){

	Node *ant, *p;
	ant = lista->root;
	p = ant;

	while (p != NULL){
		ant = p;
		p = p->next;
		free(ant);
	}

	free(lista);

}

void lista_insert(Lista *lista, char *key, EntryData val){

	Node* new = malloc(sizeof(Node));

	new->key = key;
	new->val = val;
	new->next = lista->root;

	lista->root = new;
	lista->size++;

}

void lista_print(Lista *lista){
	Node* next;
	next = lista->root;
	for(int i = 0; i<lista->size; i++){
		printf("%s %d\n", next->key, next->val);
		next = next->next;
	}
}

int main(){

	Lista* l = lista_create();

	EntryData d;
	
	d.i = 2;
	lista_insert(l,"abc",d);

	d.i = 10;
	lista_insert(l,"def",d);

	lista_print(l);

	lista_destroy(l);

}