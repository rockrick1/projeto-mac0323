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
    FILE *getlabel = fopen(filename, "r");

    Buffer *B = buffer_create(sizeof(char));
    SymbolTable alias_table = stable_create(29);
    SymbolTable label_table = stable_create(29);
    SymbolTable extern_table = stable_create(29);
    Instruction *instr = NULL;


    //Inserir os registradores especiais
    InsertionResult result;
    Operand *op;

    result = stable_insert(alias_table, "rSP");
    op = operand_create_register((unsigned char)253);
    stable_find(alias_table, "rSP")->opd = op;

    result = stable_insert(alias_table, "rA");
    op = operand_create_register((unsigned char)255);
    stable_find(alias_table, "rA")->opd = op;

    result = stable_insert(alias_table, "rR");
    op = operand_create_register((unsigned char)254);
    stable_find(alias_table, "rR")->opd = op;

    result = stable_insert(alias_table, "rX");
    op = operand_create_register((unsigned char)252);
    stable_find(alias_table, "rX")->opd = op;

    result = stable_insert(alias_table, "rY");
    op = operand_create_register((unsigned char)251);
    stable_find(alias_table, "rY")->opd = op;

    result = stable_insert(alias_table, "rZ");
    op = operand_create_register((unsigned char)250);
    stable_find(alias_table, "rZ")->opd = op;

    const char *errptr;

    printf("Indo pegar os labels\n");

    int pos = 1; //linha que estamos
    while(read_line(getlabel, B) != 0){

        printf("linhas %d\n",pos);

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

                if(line[s] == 'I' && line[e] == 'S'){

                    InsertionResult result;
                    result = stable_insert(alias_table, word);

                    if(result.new == 0){
                         errptr = &line[s];
                         set_error_msg("This label already exists");
                    }

                    //Procurar o registrador do IS

                    s = e+1;

                    while(isspace(line[s])){
                        if(line[s]=='*' || line[s] == EOF ||line[s] == '\n') break;
                        s++;
                    }

                    if(line[s]!='$'){
                        errptr = &line[s];
                        set_error_msg("Missing Register");
                    }

                    s = s+1;
                    e = s;

                    while(!isspace(line[e])){
                        if(line[e]=='*' || line[e] == EOF ||line[e] == '\n') break;
                        e++;
                    }

                    int num = 0;

                    for(int j = s; j<e; j++){
                        if(!isdigit(line[j])){ //evitar algo do tipo "$1a"
                            errptr = &line[s];
                            set_error_msg("Invalid register number.");
                        }
                        num = num*10 + (line[j] - '0');
                    }

                    Operand *op = operand_create_register((unsigned char)num);
                    stable_find(alias_table, word)->opd = op;

                }
                else{

                    while(!isspace(line[e])){
                        if(line[e]=='*' || line[e] == EOF ||line[e] == '\n') break;
                        e++;
                    }

                    //Verfica se não existe um EXTERN depois do Label

                    l = e-s;
                    int ext = l;
                    char* op = "EXTERN";

                    for(int i = 0; i<l; i++,s++){
                        if(line[s]!=op[i])
                            break;
                        ext--;
                    }

                    //Se ext = 0, todas as letras são iguais

                    if(!ext){
                        errptr = &line[s];
                        set_error_msg("Defining label before EXTERN");
                    }

                    InsertionResult result;
                    result = stable_insert(label_table, word);

                    if(result.new == 0){
                        errptr = &line[s];
                        set_error_msg("This label already exists");
                    }

                    stable_find(label_table, word)->i = pos;
                }
            }
        }

        pos++;
    }
    
    printf("Nome do programa\n");

    set_prog_name(filename);

    printf("Pegar as instruções\n");

    // passa por todas as linhas do codigo
    for (int line = 1;; line++) {

        printf("\nLer linha %d\n",line);

        if(read_line(input, B) == 0)
        	break;

        char *data = (char*)B->data;

        printf("Vou mandar para o parse\n");

        if (parse(data, alias_table, label_table, extern_table, &instr, &errptr) == 0) {
            print_error_msg(NULL);
            break;
        }

        printf("Voltei do parse\n");

        if (instr != NULL)
        {
            printf("Instrução não nula\n");
        	Instruction *top = instr;
        	const Operator *op = top->op;

            printf("Vou ver se é um IS\n");
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

    assemble(argv[1],input,output);

    fclose(input);
    fclose(output);

    return 0;
}
