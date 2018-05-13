#include "list.h"
#include "stable.h"
#include "string.h"
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

int main(int argc, char **argv) {

    FILE *file = fopen(argv[1], "r");

    SymbolTable ST = stable_create(53);

    char c;
    char prev = ' '; //pra ver se não está em vários espaços
    int w = 0; //onde estamos no buffer
    char *buffer = malloc(182*sizeof(char)); //buffer pra jogar palavras

    while (1) {
        c = getc(file);
        if (c == EOF && !isspace(prev)) {
            //se for eof e tiver uma palavra antes, coloca a palavra na st e termina
            buffer[w] = '\0';
            w++;
            char *word = malloc(w*sizeof(char));
            strncpy(word, buffer, w);
            stable_insert(ST, word);

            //reseta tudo
            for (int i = 0; i < w; i++)
                buffer[i] = '\0';
            w = 0;

            break;
        }
        if (c == EOF) break; //safety measures
        else if (!isspace(c)) {
            //se c não é espaço, joga ele no buffer
            buffer[w] = c;
            w++;
        }
        else if (isspace(c) && !isspace(prev)) {
            //se c é espaço e antes dele acabou uma palavra, joga a palavra na ST e reseta tudo
            buffer[w] = '\0';
            w++;
            char *word = malloc(w*sizeof(char));
            strncpy(word, buffer, w);
            stable_insert(ST, word);

            //reseta tudo
            for (int i = 0; i < w; i++)
                buffer[i] = '\0';
            w = 0;
        }
        prev = c;
    }

    free(buffer);
    fclose(file);


    //Coloca toda a lista em a

    const char *a[ST->n];
    int b = 0;
    for(int i = 0; i<ST->m; i++){
        lista_getall(ST->pos[i],a,b);
        b += ST->pos[i]->size;
    }

    printf("\n");

    //acha o valor maximo de espaços

    int max = -1;

    for (int i = 0; i < ST->n; i++) {
    	if ((int)(strlen(a[i])) > max)
            max = strlen(a[i]);
    }

    int j;
    int diff;


    //Ordena por insertion sort

    for(int i = 0; i<ST->n; i++){

        j = i;

        while(j>0){
            diff = strcmp(a[j],a[j-1]);
            if(diff>0)
                break;
            else{
                const char *t = a[j];
                a[j] = a[j-1];
                a[j-1] = t;
                j--;
            }
        }
    }


    //Imprime tudo com o numero de espaços bonito

    for(int i = 0; i<ST->n; i++){
        diff = max - (int)(strlen(a[i]));
        printf("%s ", a[i]);
        for(int j = 0; j<diff; j++) {
            printf(" ");
        }
        printf("%d\n", stable_find(ST,a[i])->i);
    }

    printf("\n");
    stable_destroy(ST);

    return 0;
}
