#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_WORD_SIZE 100

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


/* le um arquivo e printa o texto justificado em col colunas */
void **readFile(char *filename, int col) {
	int i, j, k, p;
	int word_count; /* quantidade de palavras na linha ate agora. reseta toda linha */
	int excess; /* espaços em excesso no final da iteração */
	char c;
    char prev;
	char *buf; /* buffer para palavra */
	int cur_word_size; /* vetor com tamanho das palavras da linha para usar na formula */
    int spaces[col/2]; /* vetor com espaços a serem inseridos entre cada palavra.
	 			/// tamanho do vetor é num de palavras - 1*/
	char **words; /* buffer para as linhas */

	FILE *file;

	/* abre o arquivo */
	file = fopen(filename, "r");

	if (!file) {
		printf("ERROR!INVALID FILE!\n");
		return NULL;
	}

	/* aloca tudo */
	/* col/2 é o maximo numero de palavras que podemos ter em uma linha */
	words = malloc((col/2)*sizeof(char*));
	buf = malloc(MAX_WORD_SIZE*sizeof(char));
	for (i = 0; i < col/2; i++){
		words[i] = malloc(MAX_WORD_SIZE*sizeof(char));
		spaces[i] = 1;
		strcpy(words[i], "");
	}

	prev = '\0';
	word_count = 0;
	cur_word_size = 0;

	for (i = j = 0; c != EOF; j++) {
		c = getc(file);
		/* se nao for espaço nem newline, escreve no buffer */
        if (c != ' ' && c != '\n' && c != EOF && c != '\r') {
            buf[i] = c;
			cur_word_size++;
			i++;
        }
		/* se nao for, faz varias coisas */
        else {
			buf[i] = '\0'; /* fecha o buffer como string */

			if (c == prev && prev == '\n') { /* fim do paragrafo, sem ideias ainda */
				continue;
			}
			
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

				/* reseta spaces e words */
				for (k = 0; k < col/2; k++)
                	spaces[k] = 1;
				for (k = 0; k < word_count; k++) {
					strcpy(words[k], "");
				}
				/* copia a palavra que excedeu o limite */
				strcpy(words[0], buf);
				j = cur_word_size;
				if (j == 0) word_count = 0;
				else word_count = 1;
            }

			else {
				strcpy(words[word_count], buf);
				word_count++;
			}

			buf[0] = '\0'; /* reseta o buffer */
			i = 0;
			cur_word_size = 0;
        }
        prev = c;
	}
	/* escreve a ultima linha i guess */
	for (i = 0; i < word_count - 1; i++)
		spaces[i] = 1;
	spaces[word_count - 1] = 0;
	write(words, spaces, word_count);


	/* destroi tudo */
	for (i = 0; i < col/2; i++)
		free(words[i]);
	free(words);
	free(buf);
	fclose(file);
	return 0;
}

int main(int argc, char **argv) {
	readFile(argv[1], atoi(argv[2]));
	return 0;
}









