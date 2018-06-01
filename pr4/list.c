#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

Lista* lista_create(){

	Lista* lista = malloc(sizeof(Lista));

	lista->root = NULL;
	lista->size = 0;

	return lista;

}

void lista_destroy(Lista *lista){

	if(lista->root != NULL){

		Node *ant;
		ant = lista->root;

		while (lista->root != NULL){
			ant = lista->root;
			lista->root = lista->root->next;
			free(ant);
		}
	}

	free(lista);

}

InsertionResult lista_insert(Lista *lista, const char *key, EntryData val){
	InsertionResult result;
	EntryData d;

	// Checa se a lista já contém essa chave. Se ja tiver,
	// aumenta a frequencia dela
	Node *p;
	p = lista->root;
	while (p != NULL) {
		if (strcmp(key, p->key)==0) {
			(p->val.i)++;
			result.new = 0;
			d.i = p->val.i;
			result.data = &d;
			return result;
		}
		p = p->next;
	}

	// Se não estiver, cria uma node com a nova chave chave e o val
	// (imagino que todos entrem com val.i = 1 para inicializar a frequencia
	// de novas palavras)
	Node* new = malloc(sizeof(Node));

	result.new = 1;

	d.i = 1;
	result.data = &d;

	new->key = key;
	new->val = val;
	new->next = lista->root;

	lista->root = new;
	lista->size++;

	return result;
}

int print_node(const char *key, EntryData *data) {
    // devemos considerar outras possibilidades de valores de data
    // como string e pointer
	if (key == NULL || data == NULL)
		return 0;
    printf("%s : %d\n", key, data->i);
    return 1;
}

void lista_getall(Lista *lista,const char *a[],int i){
	Node *next = NULL;
	next = lista->root;
	for(int j = 0; j<lista->size; j++, i++){
		a[i] = next->key;
		next = next->next;
	}
}
