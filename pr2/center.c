#include <stdio.h>
#include <ctype.h>
#include "buffer.h"

int main(int argc, char **argv) {
    // argv[1] = arquivo de entrada
    // argv[2] = arquivo de saida
    // argv[3] = numero de colunas
	Buffer *b = buffer_create(sizeof(char));
	for (int i = 40; i < 200; i++)
		buffer_push_char(b, 'A');
	buffer_push_char(b, '!');
	buffer_push_char(b, '6');
	buffer_push_char(b, 'h');
	// buffer_reset(b);
	buffer_push_char(b, 'h');
	char *data = (char*)b->data;

	printf("size:%zu\n%s\n", b->buffer_size, data);
	printf("3rd pos: %c\n", data[2]);
	buffer_destroy(b);
	return 0;
}