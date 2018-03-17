#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void write(char **words, int spaces[], int word_count) {
	int i, j;
	for (i = 0; i < word_count; i++) {
		printf("%s", words[i]);
		/* printa os espaços necessarios */
		for (j = 0; j < spaces[i]; j++)
			printf(" ");
	}
	printf("\n");
}


int getExcess(int *word_sizes, int c, int n) {
	int N, i;
	N = c;
	for (i = 0; i < n; i++)
		N -= word_sizes[i];
	N -= n-1;
	N /= n-1;
	printf("excess:%d\n", N);
	return N;
}


void **readFile(char *filename, int col) {
	int i, j, k, cur_line, word_count, excess;
	char c;
    char prev;
	char buf[30];
	int *word_sizes;
    int *spaces;
	int max_words;
	char **words; /* buffer para as linhas */
	FILE *file;

	/* abre o arquivo */
	file = fopen(filename, "r");

	if (!file) {
		printf("ERROR!INVALID FILE!\n");
		return NULL;
	}

	max_words = 40; /* maximo de palavras numa linha */

	spaces = malloc(col*sizeof(int));
	word_sizes = malloc(max_words*sizeof(int));
	words = malloc(sizeof(char**));
	for (i = 0; i < max_words; i++){
		word_sizes[i] = 0;
		words[i] = malloc(30*sizeof(char));
		strcpy(words[i], "");
	}

	prev = '\0';
    cur_line = 0;
	word_count = 0;
	for (k = 0; k < col; k++)
		spaces[k] = 1;


	for (i = j = 0; c != EOF; j++) {
		c = getc(file);
        if (c != ' ' && c != '\n') {
            buf[i] = c;
			word_sizes[word_count]++;
			i++;
        }
        else {
			buf[i] = '\0'; /* fecha o buffer como string */
			printf("%d\n", word_count);
			strcpy(words[word_count], buf);
            word_count++;
			buf[0] = '\0'; /* reseta o buffer */
			i = 0;

            if (j > col) { /* se ultrapassou o limite de colunas */
				word_sizes[0] = word_sizes[word_count]; /* salva o tamanho da ultima? */
                excess = getExcess(word_sizes, col, word_count - 1); /* o quanto excedeu o limite */
                for (k = 0; k < excess; k++) {
                    spaces[(word_count - k - 2) % (word_count - 2)]++; /* errado */
                }
				spaces[word_count - 1] = 0;

				/* escreve as parada */
                write(words, spaces, word_count);

				/* reseta td */
				for (k = 0; k < col; k++)
                	spaces[k] = 1;
				for (k = 0; k < word_count; k++) {
					word_sizes[k] = 0;
					strcpy(words[k], "");
				}
				j = 0;
                word_count = 0;
				cur_line++;

            }
            if (c == prev && prev == '\n') { /* fim do paragrafo, save meeeee */
				continue;
            }
        }
        prev = c;
	}
	fclose(file);
	for (i = 0; i < max_words; i++)
		free(words[i]);
	free(words);
	free(spaces);
	return 0;
}

int main(int argc, char **argv) {
	readFile(argv[1], atoi(argv[2]));
	return 0;
}









