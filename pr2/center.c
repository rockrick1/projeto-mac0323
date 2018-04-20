#include <stdio.h>
#include <ctype.h>
#include "buffer.h"

int main(int argc, char **argv) {
    // argv[1] = arquivo de entrada
    // argv[2] = arquivo de saida
    // argv[3] = numero de colunas
	FILE *input = fopen(argv[1], "r");
	FILE *output = fopen(argv[2], "w");

	int col = atoi(argv[3]);
	int count, chars, cur_line = 1;

	Buffer *B = buffer_create(sizeof(char));
	Buffer *aux = buffer_create(sizeof(char));

	char *data, *aux_data;


	while (1) {
		// chars sera o numero de chars lidos em cada linha
		// Se retornar 0, siginifica que encontrou uma linha cujo primeiro
		// char é EOF
		chars = read_line(input, B);
		if (chars != 0) {
			data = (char*)B->data;
			// printf("line %d: %s\n",cur_line, data);
			count = 0;
			// conta quantos chars tem a partir do primeiro nao-space
			// e guarda eles num buffer auxiliar
			for (int i = 0; i < chars; i++) {
				// se ja estiver contando, continua
				if (count > 0) {
					count++;
					buffer_push_char(aux, data[i]);
				}
				// começa a contar quando achar um nao-space
				else if (count == 0 && !isspace(data[i])) {
					count = 1;
					buffer_push_char(aux, data[i]);
				}
			}
			aux_data = (char*)aux->data;
			// se ultrapassou col, só escreve a linha normalmente e xinga
			if (count >= col) {
				fprintf(stderr,"center: %s: line %d: line too long.\n",argv[1],cur_line);
				fprintf(output, "%s", aux_data);
			}
			// se for uma linha em branco, escreve um newline
			else if (count == 0)
				fprintf(output, "\n");
			// se nao ultrapassou, calcula os espaços e escreve a linha
			// logo após
			else {
				// será o numero de espaços que colocaremos no começo
				int spaces = (col - count)/2;
				for (int i = 0; i < spaces; i++)
					fprintf(output, " ");
				fprintf(output, "%s", aux_data);
				// printf("line %d: count %d: spaces %d\n", cur_line, count, spaces);
			}

			cur_line++;
			buffer_reset(aux);
		}
		// chegou no fim do arquivo
		else break;
	}

	buffer_destroy(B);
	buffer_destroy(aux);
	fclose(input);
	fclose(output);
	return 0;
}