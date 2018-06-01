#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "buffer.h"
#include "error.h"

int main(int argc, char **argv) {
    FILE *input = fopen(argv[1], "r");

    Buffer *B = buffer_create(sizeof(char));
    SymbolTable alias_table = stable_create();
    Instruction **instr = NULL;

    // passa por todas as linhas do codigo
    for (int line = 1;; line++) {
        // le a linha de codigo e poe no buffer
        read_line(input, B);
        char *data = (char*)B->data;

        const char **errptr; // owo whats dis??

        if (parse(data, alias_table, instr, errptr) == 0) break;
    }

    // libera as instruções
    Instruction *destroy, *next;
    for (destroy = *instr; next != NULL; destroy = next) {
        next = destroy->next;
        instr_destroy(destroy);
        destroy = next;
    }

    buffer_destroy(B);
    stable_destroy(alias_table);
    return 0;
}