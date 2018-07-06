#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "parser.h"
#include "optable.h"
#include "opcodes.h"
#include "buffer.h"
#include "error.h"
#include "asm.h"

int assemble(const char *filename, FILE *input, FILE *output) {

    set_prog_name(filename);

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
                    set_error_msg("Invalid label name in line %d",pos);
                }

                for(int j = 1; j<l; j++){
                    if( !isalpha(word[j]) && word[0] != '_' && !isdigit(word[j]) ){
                        errptr = &line[s];
                        set_error_msg("Invalid label name in line %d",pos);
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
                         set_error_msg("Label already exists in line %d",pos);
                    }

                    //Procurar o registrador do IS

                    s = e+1;

                    while(isspace(line[s])){
                        if(line[s]=='*' || line[s] == EOF ||line[s] == '\n') break;
                        s++;
                    }

                    if(line[s]!='$'){
                        errptr = &line[s];
                        set_error_msg("Missing register in line %d",pos);
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
                            set_error_msg("Invalid register number in line %d",pos);
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
                        die("Defining label before EXTERN in line %d",pos);
                    }

                    InsertionResult result;
                    result = stable_insert(label_table, word);

                    if(result.new == 0){
                        errptr = &line[s];
                        die("Label already exists in line %d",pos);
                    }

                    stable_find(label_table, word)->i = pos;
                }
            }
        }

        pos++;
    }

    printf("Pegar as instruções\n");

    // passa por todas as linhas do codigo
    for (int line = 1;; line++) {

        if(read_line(input, B) == 0)
        	break;

        printf("\nLer linha %d\n",line);

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

    //inverte a lista ligada
    Instruction *tnext, *ttop, *temp;
    tnext = instr;
    while(tnext != NULL) {      
        temp = tnext;
        tnext = tnext->next;
        temp->next = ttop;
        ttop = temp;
    }

    instr = ttop;

    for(int line = 1; ttop!=NULL; line++){

    	Operator *op = ttop->op;

    	if(strcmp(op->name,"IS") && strcmp(op->name,"EXTERN")){
    		
    		char* name = op->name;

    		if(!strcmp(name,"LDB")){
    			if(ttop->opds[2] == REGISTER){
    				printf("00");
    			}
    			else{
    				printf("01");
    			}
    		}
    		else if(!strcmp(name,"LDW")){
    			if(ttop->opds[2] == REGISTER){
    				printf("02");
    			}
    			else{
    				printf("03");
    			}
    		}
    		else if(!strcmp(name,"LDT")){
    			if(ttop->opds[2] == REGISTER){
    				printf("04");
    			}
    			else{
    				printf("05");
    			}
    		}
    		else if(!strcmp(name,"LDO")){
    			if(ttop->opds[2] == REGISTER){
    				printf("06");
    			}
    			else{
    				printf("07");
    			}
    		}
    		else if(!strcmp(name,"LDBU")){
    			if(ttop->opds[2] == REGISTER){
    				printf("08");
    			}
    			else{
    				printf("09");
    			}
    		}
    		else if(!strcmp(name,"LDWU")){
    			if(ttop->opds[2] == REGISTER){
    				printf("0a");
    			}
    			else{
    				printf("0b");
    			}
    		}
    		else if(!strcmp(name,"LDTU")){
    			if(ttop->opds[2] == REGISTER){
    				printf("0c");
    			}
    			else{
    				printf("0d");
    			}
    		}
    		else if(!strcmp(name,"LDOU")){
    			if(ttop->opds[2] == REGISTER){
    				printf("0e");
    			}
    			else{
    				printf("0f");
    			}
    		}
    		else if(!strcmp(name,"STB")){
    			if(ttop->opds[2] == REGISTER){
    				printf("10");
    			}
    			else{
    				printf("11");
    			}
    		}
    		else if(!strcmp(name,"STW")){
    			if(ttop->opds[2] == REGISTER){
    				printf("12");
    			}
    			else{
    				printf("13");
    			}
    		}
    		else if(!strcmp(name,"STT")){
    			if(ttop->opds[2] == REGISTER){
    				printf("14");
    			}
    			else{
    				printf("15");
    			}
    		}
    		else if(!strcmp(name,"STO")){
    			if(ttop->opds[2] == REGISTER){
    				printf("16");
    			}
    			else{
    				printf("17");
    			}
    		}
    		else if(!strcmp(name,"STBU")){
    			if(ttop->opds[2] == REGISTER){
    				printf("18");
    			}
    			else{
    				printf("19");
    			}
    		}
    		else if(!strcmp(name,"STWU")){
    			if(ttop->opds[2] == REGISTER){
    				printf("1a");
    			}
    			else{
    				printf("1b");
    			}
    		}
    		else if(!strcmp(name,"STTU")){
    			if(ttop->opds[2] == REGISTER){
    				printf("1c");
    			}
    			else{
    				printf("1d");
    			}
    		}
    		else if(!strcmp(name,"STOU")){
    			if(ttop->opds[2] == REGISTER){
    				printf("1e");
    			}
    			else{
    				printf("1f");
    			}
    		}
    		else if(!strcmp(name,"ADD")){
    			if(ttop->opds[2] == REGISTER){
    				printf("20");
    			}
    			else{
    				printf("21");
    			}
    		}
    		else if(!strcmp(name,"SUB")){
    			if(ttop->opds[2] == REGISTER){
    				printf("22");
    			}
    			else{
    				printf("23");
    			}
    		}
    		else if(!strcmp(name,"MUL")){
    			if(ttop->opds[2] == REGISTER){
    				printf("24");
    			}
    			else{
    				printf("25");
    			}
    		}
    		else if(!strcmp(name,"DIV")){
    			if(ttop->opds[2] == REGISTER){
    				printf("26");
    			}
    			else{
    				printf("27");
    			}
    		}
    		else if(!strcmp(name,"CMP")){
    			if(ttop->opds[2] == REGISTER){
    				printf("28");
    			}
    			else{
    				printf("29");
    			}
    		}
    		else if(!strcmp(name,"SL")){
    			if(ttop->opds[2] == REGISTER){
    				printf("2a");
    			}
    			else{
    				printf("2b");
    			}
    		}
    		else if(!strcmp(name,"SR")){
    			if(ttop->opds[2] == REGISTER){
    				printf("2c");
    			}
    			else{
    				printf("2d");
    			}
    		}
    		else if(!strcmp(name,"NEG")){
    			if(ttop->opds[2] == REGISTER){
    				printf("2e");
    			}
    			else{
    				printf("2f");
    			}
    		}
    		else if(!strcmp(name,"ADDU")){
    			if(ttop->opds[2] == REGISTER){
    				printf("30");
    			}
    			else{
    				printf("31");
    			}
    		}
    		else if(!strcmp(name,"SUBU")){
    			if(ttop->opds[2] == REGISTER){
    				printf("32");
    			}
    			else{
    				printf("33");
    			}
    		}
    		else if(!strcmp(name,"MULU")){
    			if(ttop->opds[2] == REGISTER){
    				printf("34");
    			}
    			else{
    				printf("35");
    			}
    		}
    		else if(!strcmp(name,"DIVU")){
    			if(ttop->opds[2] == REGISTER){
    				printf("36");
    			}
    			else{
    				printf("37");
    			}
    		}
    		else if(!strcmp(name,"CMPU")){
    			if(ttop->opds[2] == REGISTER){
    				printf("38");
    			}
    			else{
    				printf("39");
    			}
    		}
    		else if(!strcmp(name,"SLU")){
    			if(ttop->opds[2] == REGISTER){
    				printf("3a");
    			}
    			else{
    				printf("3b");
    			}
    		}
    		else if(!strcmp(name,"SRU")){
    			if(ttop->opds[2] == REGISTER){
    				printf("3c");
    			}
    			else{
    				printf("3d");
    			}
    		}
    		else if(!strcmp(name,"NEGU")){
    			if(ttop->opds[2] == REGISTER){
    				printf("3e");
    			}
    			else{
    				printf("3f");
    			}
    		}
    		else if(!strcmp(name,"AND")){
    			if(ttop->opds[2] == REGISTER){
    				printf("40");
    			}
    			else{
    				printf("41");
    			}
    		}
    		else if(!strcmp(name,"OR")){
    			if(ttop->opds[2] == REGISTER){
    				printf("42");
    			}
    			else{
    				printf("43");
    			}
    		}
    		else if(!strcmp(name,"XOR")){
    			if(ttop->opds[2] == REGISTER){
    				printf("44");
    			}
    			else{
    				printf("45");
    			}
    		}
    		else if(!strcmp(name,"NXOR")){
    			if(ttop->opds[2] == REGISTER){
    				printf("46");
    			}
    			else{
    				printf("47");
    			}
    		}
    		else if(!strcmp(name,"JMP")){
    			EntryData *test = stable_find(label_table,ttop->opds[0]->value.label);
    			int i;
    			if(test = NULL){
    				i = ttop->opds[0]->value.num - line;
    			}
    			else{
    				i = stable_find(label_table,ttop->opds[0]->value.label)->i - line;
    			}
    			if(i > 0){
    				printf("48");
    			}
    			else{
    				printf("49");
    				i = -i;
    			}
    			printf("%06x",i);
    		}
    		else if(!strcmp(name,"JZ")){
    			EntryData *test = stable_find(label_table,ttop->opds[1]->value.label);
    			int i;
    			if(test = NULL){
    				i = ttop->opds[1]->value.num - line;
    			}
    			else{
    				i = stable_find(label_table,ttop->opds[1]->value.label)->i - line;
    			}
    			if(i > 0){
    				printf("4a");
    			}
    			else{
    				printf("4b");
    				i = -i;
    			}

    			printf("%x",(int)ttop->opds[0]->value.num);
    			printf("%04x",i);
    		}
    		else if(!strcmp(name,"JNZ")){
    			printf("4c");
    		}
    		else if(!strcmp(name,"JP")){
    			printf("4e");
    		}
    		else if(!strcmp(name,"JN")){
    			printf("50");
    		}
    		else if(!strcmp(name,"JNN")){
    			printf("52");
    		}
    		else if(!strcmp(name,"JNP")){
    			printf("54");
    		}
    		else if(!strcmp(name,"GO")){
    			printf("56");
    		}
    		else if(!strcmp(name,"GETA")){
    			printf("58");
    		}
    		else if(!strcmp(name,"SETW")){
    			printf("5a");
    		}
    		else if(!strcmp(name,"SAVE")){
    			printf("5b");
    		}
    		else if(!strcmp(name,"REST")){
    			printf("5c");
    		}
    		else if(!strcmp(name,"INT")){
    			printf("fe");
    		}
    		else if(!strcmp(name,"NOP")){
    			printf("ff");
    		}


    	}

    	ttop = ttop->next;
    	printf("\n");
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
