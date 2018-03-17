#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* recebe as palavras a serem escritas, o vetor de espaços a serem inseridos
// entre elas e o numero de palavras, e escreve uma linha */
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


/* usa a formula do enunciado (what the actual fuck) pra descobrir o numero
// de espaços em excesso */
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


/* le um arquivo e printa o texto justificado em col colunas */
void **readFile(char *filename, int col) {
	int i, j, k;
	int cur_line; /* linha atual */
	int word_count; /* quantidade de palavras na linha ate agora. reseta toda linha */
	int excess; /* espaços em excesso no final da iteração */
	char c;
    char prev;
	char buf[30]; /* buffer para palavra */
	int *word_sizes; /* vetor com tamanho das palavras da linha para usar na formula */
    int *spaces; /* vetor com espaços a serem inseridos entre cada palavra.
	 			/// tamanho do vetor é num de palavras - 1*/
	char **words; /* buffer para as linhas */

	int max_words; /* para alocação */
	FILE *file;

	/* abre o arquivo */
	file = fopen(filename, "r");

	if (!file) {
		printf("ERROR!INVALID FILE!\n");
		return NULL;
	}

	max_words = 40; /* maximo de palavras numa linha */

	/* aloca tudo */
	spaces = malloc(col*sizeof(int));
	word_sizes = malloc(max_words*sizeof(int));
	words = malloc(sizeof(char**));
	for (i = 0; i < max_words; i++){
		word_sizes[i] = 0;
		words[i] = malloc(30*sizeof(char));
		strcpy(words[i], "");
	}

	prev = '\0'; /* esse char define o final de uma string!!! */
    cur_line = 0;
	word_count = 0;
	for (k = 0; k < col; k++)
		spaces[k] = 1;


	for (i = j = 0; c != EOF; j++) {
		c = getc(file);
		/* se nao for espaço nem newline, escreve no buffer */
        if (c != ' ' && c != '\n') {
            buf[i] = c;
			word_sizes[word_count]++;
			i++;
        }
		/* se nao for, faz varias coisas */
        else {
			buf[i] = '\0'; /* fecha o buffer como string */
			printf("%d\n", word_count);
			strcpy(words[word_count], buf); /* da segfault aqui uma hora e nao sei porque D: */
            word_count++;
			buf[0] = '\0'; /* reseta o buffer */
			i = 0;

			/* se ultrapassou o limite de colunas */
            if (j > col) {
				word_sizes[0] = word_sizes[word_count]; /* salva o tamanho da ultima (not sure) */
                excess = getExcess(word_sizes, col, word_count - 1); /* o quanto excedeu o limite */
                for (k = 0; k < excess; k++) {
                    spaces[(word_count - k - 2) % (word_count - 2)]++; /* muito provavelmente errado */
                }
				spaces[word_count - 1] = 0; /* nao sei se precisa disso */

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
            if (c == prev && prev == '\n') { /* fim do paragrafo, sem ideias ainda */
				continue;
            }
        }
        prev = c;
	}
	/* destroi tudo */
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









