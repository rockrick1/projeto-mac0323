#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "parser.h"
#include "optable.h"
#include "buffer.h"
#include "error.h"
#include "asm.h"

int assemble(const char *filename, FILE *input, FILE *output) {
    FILE *getlabel = input;

    Buffer *B = buffer_create(sizeof(char));
    SymbolTable alias_table = stable_create(29);
    Instruction *instr = NULL;

    const char *errptr;

    int pos = 1; //linha que estamos
    while(read_line(getlabel, B) != 0){

        char *line = (char*)B->data;

        int s = 0;  //start
        int e = 0;  //end
        int l = 0; //lenght

        while(isspace(line[s])){
            if(line[s]=='*' || line[s] == EOF ||line[s] == '\n') break;
            s++;
        }

        e = s;

        while(!isspace(line[e])){
            if(line[e]=='*' || line[e] == EOF ||line[e] == '\n') break;
            e++;
        }

        l = e - s;

        if(l>0){

            char *word = malloc(sizeof(char)*(l+1));
            for(int i = 0; i<l; i++, s++) word[i] = line[s];
            word[l] = '\0';

            const Operator *test;
            test = optable_find(word);

            if(test == NULL){

                if(!isalpha(word[0]) && word[0] != '_'){
                    errptr = &line[s];
                    set_error_msg("Invalid label name.");
                }

                for(int j = 1; j<l; j++){
                    if( !isalpha(word[j]) && word[0] != '_' && !isdigit(word[j]) ){
                        errptr = &line[s];
                        set_error_msg("Invalid label name.");
                    }
                }

                while(isspace(line[s])){
                    if(line[s]=='*' || line[s] == EOF ||line[s] == '\n') break;
                    s++;
                }
                e = s+1;

                if(line[s] != 'I' || line[e] != 'S'){

                    InsertionResult result;
                    result = stable_insert(alias_table, word);

                    if(result.new == 0){
                         errptr = &line[s];
                         set_error_msg("This label already exists");
                    }

                    Operand *op = operand_create_label(word);
                    stable_find(alias_table, word)->opd = op;
                }
                else{

                    InsertionResult result;
                    result = stable_insert(alias_table, word);

                    if(result.new == 0){
                        errptr = &line[s];
                        set_error_msg("This label already exists");
                    }
                }
            }
        }

        pos++;
    }
    
    set_prog_name(filename);

    // passa por todas as linhas do codigo
    for (int line = 1;; line++) {

        if(read_line(input, B) == 0)
        	break;

        char *data = (char*)B->data;

        if (parse(data, alias_table, &instr, &errptr) == 0) {
            print_error_msg(NULL);
            break;
        }

        if (instr != NULL)
        {
        	Instruction *top = instr;
        	const Operator *op = top->op;

        	if (op->opcode == IS) {
                stable_find(alias_table, top->label)->opd = top->opds[0];
            }   

            printf("line     = %s", data);
            if (data[strlen(data)-1] != '\n') printf("\n");
            if (instr->label[0] == '\0')
            {
                printf("label    = n/a\n");
            }
            else
            {
                printf("label    = %s\n", top->label);
            }
            printf("operator = %s\n", op->name);
            printf("operands = ");

            //aqui vão acontecer firulas

            int count = 0;
            while (top->opds[count] != NULL && count < 3)
            {
                if (count != 0) printf(", ");
                if (instr->opds[count]->type == REGISTER)
                {
                    printf("Register(%d)", (int)instr->opds[count]->value.num);
                }
                if (instr->opds[count]->type == LABEL)
                {
                    printf("Label(\"%s\")", instr->opds[count]->value.label);
                }
                if (instr->opds[count]->type == NUMBER_TYPE)
                {
                    printf("Number(%d)", (int)instr->opds[count]->value.num);
                }
                if (instr->opds[count]->type == STRING)
                {
                    printf("String(%s)", instr->opds[count]->value.str);
                }
                count++;
            }

            printf("\n\n");
        }

    }

    //test printf("Parei de olhar para as linhas, hora de libertar!\n");

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

int main(int argc, char **argv) {

    char* file = argv[1];

    int e = 0;

    while(file[e]!='.' && file[e]!=EOF && file[e]!=' '){
        e++;
    }

    char* filename = malloc(sizeof(char)*(e+4));

    for(int i = 0; i<e; i++)
        filename[i] = file[i];

    filename[e] = '.';
    filename[e+1] = 'm';
    filename[e+2] = 'a';
    filename[e+3] = 'c';
    filename[e+4] = '\0';

    FILE *input = fopen(argv[1], "r");
    FILE *output = fopen(filename, "w");

    assemble(filename,input,output);

    fclose(input);
    fclose(output);

    return 0;
}
