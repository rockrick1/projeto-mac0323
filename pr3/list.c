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
	// Checa se a lista já contém essa chave. Se ja tiver,
	// aumenta a frequencia dela
	Node *p;
	p = lista->root;

	while (p != NULL) {
		if (strcmp(key, p->key)==0) {
			(p->val.i)++;
			return;
		}
		p = p->next;
	}

	// Se não estiver, cria uma node com a nova chave chave e o val
	// (imagino que todos entrem com val.i = 1 para inicializar a frequencia
	// de novas palavras)
	Node* new = malloc(sizeof(Node));

	new->key = key;
	new->val = val;
	new->next = lista->root;

	lista->root = new;
	lista->size++;

}

void lista_print(Lista *lista){
	Node *next = NULL;
	next = lista->root;
	for(int i = 0; i<lista->size; i++){
		printf("%s %d\n", next->key, next->val);
		next = next->next;
	}
}
