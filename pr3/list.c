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

	printf("Destroir, preparando\n");

	Node *ant;
	ant = lista->root;

	printf("Destroir, preparado\n\n");

	while (lista->root != NULL){
		printf("iterando root =  %s\n",lista->root->key);
		ant = lista->root;
		lista->root = lista->root->next;
		free(ant);
		printf("completado mais uma\n\n");
	}
	}

	printf("liberando\n");
	// ISSo ESXPLODE
	free(lista);
	printf("liberado\n\n");

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

void lista_print(Lista *lista){
	Node *next = NULL;
	next = lista->root;
	for(int i = 0; i<lista->size; i++){
		printf("%s %d\n", next->key, next->val);
		next = next->next;
	}
}
