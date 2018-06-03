#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "buffer.h"
#include "error.h"

int main(int argc, char **argv) {
    FILE *input = fopen(argv[1], "r");

    printf("Dei file input\n");
    Buffer *B = buffer_create(sizeof(char));
    printf("Criei o buffer\n");
    SymbolTable alias_table = stable_create(29);
    printf("Criei a table\n");
    Instruction *instr = NULL;
    printf("Criei a Instruction\n");

    set_prog_name("parse_test");

    // passa por todas as linhas do codigo
    for (int line = 1;; line++) {

    	printf("\nDentro do for, linha %d\n",line);
        // le a linha de codigo e poe no buffer
        if(read_line(input, B) == 0)
        	break;

        char *data = (char*)B->data;

        const char *errptr;

        if (parse(data, alias_table, &instr, &errptr) == 0) {
            printf("deu ruim, to vazando\n");
            print_error_msg(NULL);
            break;
        }

        if (instr != NULL) {
            Instruction *top = instr;
            const Operator *op = top->op;

            // lida com o IS, se for o caso
            if (op->opcode == IS) {
                // se ja estiver na tabela, mete o loco
                if (stable_find(alias_table, top->label))
                    printf("erro poarr\n");
                // senao, insere e altera o valor pra opds[0] (hopefully)
                else {
                    InsertionResult result;
                    result = stable_insert(alias_table, top->label);
                    stable_find(alias_table, top->label)->opd = top->opds[0];
                }
            }
            // Operand opd = *top->opds[0];
        }
    }

    // libera as instruções
    Instruction *destroy, *next;
    for (destroy = instr; next != NULL; destroy = next) {
        next = destroy->next;
        instr_destroy(destroy);
        destroy = next;
    }

    buffer_destroy(B);
    stable_destroy(alias_table);
    return 0;
}
