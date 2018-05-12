#include "list.h"
#include "stable.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    SymbolTable ST = stable_create(17);

	stable_insert(ST, "0");
    stable_insert(ST, "9");
    stable_insert(ST, "abc");
    stable_insert(ST, "aab");
    stable_insert(ST, "zxc");
    stable_insert(ST, "89wrtu9b");
    stable_insert(ST, "89wrb");

    //Coloca toda a lista em a

    char *a[ST->n];
    int b = 0;
    for(int i = 0; i<ST->m; i++){
        lista_getall(ST->pos[i],a,b);
        b += ST->pos[i]->size;
    }

    printf("\n");
    int max = -1;

    //Acha o valor maximo

    for(int i = 0; i<ST->n; i++){
        if((int)(strlen(a[i]))>max)
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
                char *t = a[j];
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
        for(int j = 0; j<diff; j++)
            printf(" ");
        printf("%d\n", stable_find(ST,a[i])->i);
    }

    printf("\n");
    stable_destroy(ST);

    return 0;
}
