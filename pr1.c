#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* recebe as palavras a serem escritas, o vetor de espaços a serem inseridos
// entre elas e o numero de palavras, e escreve uma linha */
void write(char **words, int *spaces, int word_count) {
	int i, j;
	for (i = 0; i < word_count; i++) {
		printf("%s", words[i]);
		/* printa os espaços necessarios */
		for (j = 0; j < spaces[i]; j++)
			printf(" ");
	}
	printf("\n");
}


/* le um arquivo e printa o texto justificado em col colunas */
void **readFile(char *filename, int col) {
	int i, j, k, p;
	int word_count; /* quantidade de palavras na linha ate agora. reseta toda linha */
	int excess; /* espaços em excesso no final da iteração */
	char c;
    char prev;
	char *buf; /* buffer para palavra */
	int cur_word_size; /* vetor com tamanho das palavras da linha para usar na formula */
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
	words = malloc(sizeof(char**));
	buf = malloc(50*sizeof(char));
	for (i = 0; i < max_words; i++){
		words[i] = malloc(30*sizeof(char));
		strcpy(words[i], "");
	}

	prev = '\0'; /* esse char define o final de uma string!!! */
	word_count = 0;
	cur_word_size = 0;
	for (k = 0; k < col; k++)
		spaces[k] = 1;


	for (i = j = 0; c != EOF; j++) {
		c = getc(file);
		/* se nao for espaço nem newline, escreve no buffer */
        if (c != ' ' && c != '\n') {
            buf[i] = c;
			cur_word_size++;
			i++;
        }
		/* se nao for, faz varias coisas */
        else {
			buf[i] = '\0';

			/* se ultrapassou o limite de colunas */
			if (j > col) {
				/* define os espaços a serem printados nessa linha */
                excess = cur_word_size - (j - col) + 1; /* quantos espaços sobraram */
				p = word_count - 2;
                for (k = 0; k < excess; k++) {
                    spaces[p]++;
					p--;
					if (p < 0)
						p = word_count - 2;
                }
				spaces[word_count - 1] = 0;
				/***************************************************/

				/* escreve as parada */
                write(words, spaces, word_count);

				/* reseta td */
				for (k = 0; k < col; k++)
                	spaces[k] = 1;
				for (k = 0; k < word_count; k++) {
					strcpy(words[k], "");
				}
				strcpy(words[0], buf);
				j = cur_word_size;
                word_count = 1;
            }

			else {
				strcpy(words[word_count], buf); /* da segfault aqui uma hora e nao sei porque D: */
				printf("%s\n", buf);
				word_count++;
			}

            if (c == prev && prev == '\n') { /* fim do paragrafo, sem ideias ainda */
				continue;
            }
			buf[0] = '\0'; /* reseta o buffer */
			i = 0;
			cur_word_size = 0;
        }
        prev = c;
	}
	/* destroi tudo */
	for (i = 0; i < max_words; i++)
		free(words[i]);
	free(words);
	free(spaces);
	free(buf);
	fclose(file);
	return 0;
}

int main(int argc, char **argv) {
	readFile(argv[1], atoi(argv[2]));
	return 0;
}









