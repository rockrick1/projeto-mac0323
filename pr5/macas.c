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
    int pushed = 0;
    for(int line = 1; ttop!=NULL; line++, ttop = ttop->next){

    	Operator *op = ttop->op;
        char* name = op->name;


        if(!strcmp(op->name,"IS")){
            continue;
        }

    	if(strcmp(op->name,"EXTERN") && strcmp(op->name,"STR")
            && strcmp(op->name,"PUSH") && strcmp(op->name,"CALL") && strcmp(op->name,"RET")){

            if(pushed>0){
                fprintf(output,"31fdfd%02x\n",pushed);
                line++;
                pushed = 0;
            }

    		if(!strcmp(name,"LDB")){
    			if(ttop->opds[2]->type == REGISTER){
    				fprintf(output, "00");
    			}
    			else{
    				fprintf(output, "01");
    			}
    			
    			fprintf(output, "%02x",(int)ttop->opds[0]->value.num);
    			fprintf(output, "%02x",(int)ttop->opds[1]->value.num);
				

    			if(ttop->opds[2]->type == LABEL){
    				fprintf(output, "%02x",stable_find(alias_table,ttop->opds[2]->value.label)->i);			
    			}
    			else{
    				fprintf(output, "%02x",(int)ttop->opds[2]->value.num);
    			}
    			

    		}
    		else if(!strcmp(name,"LDW")){
    			if(ttop->opds[2]->type == REGISTER){
    				fprintf(output, "02");
    			}
    			else{
    				fprintf(output, "03");
    			}

    			fprintf(output, "%02x",(int)ttop->opds[0]->value.num);
    			fprintf(output, "%02x",(int)ttop->opds[1]->value.num);
				

    			if(ttop->opds[2]->type == LABEL){
    				fprintf(output, "%02x",stable_find(alias_table,ttop->opds[2]->value.label)->i);			
    			}
    			else{
    				fprintf(output, "%02x",(int)ttop->opds[2]->value.num);
    			}
    		}
    		else if(!strcmp(name,"LDT")){
    			if(ttop->opds[2]->type == REGISTER){
    				fprintf(output, "04");
    			}
    			else{
    				fprintf(output, "05");
    			}

    			fprintf(output, "%02x",(int)ttop->opds[0]->value.num);
    			fprintf(output, "%02x",(int)ttop->opds[1]->value.num);
				

    			if(ttop->opds[2]->type == LABEL){
    				fprintf(output, "%02x",stable_find(alias_table,ttop->opds[2]->value.label)->i);			
    			}
    			else{
    				fprintf(output, "%02x",(int)ttop->opds[2]->value.num);
    			}
    		}
    		else if(!strcmp(name,"LDO")){
    			if(ttop->opds[2]->type == REGISTER){
    				fprintf(output, "06");
    			}
    			else{
    				fprintf(output, "07");
    			}

    			fprintf(output, "%02x",(int)ttop->opds[0]->value.num);
    			fprintf(output, "%02x",(int)ttop->opds[1]->value.num);
				

    			if(ttop->opds[2]->type == LABEL){
    				fprintf(output, "%02x",stable_find(alias_table,ttop->opds[2]->value.label)->i);			
    			}
    			else{
    				fprintf(output, "%02x",(int)ttop->opds[2]->value.num);
    			}
    		}
    		else if(!strcmp(name,"LDBU")){
    			if(ttop->opds[2]->type == REGISTER){
    				fprintf(output, "08");
    			}
    			else{
    				fprintf(output, "09");
    			}

    			fprintf(output, "%02x",(int)ttop->opds[0]->value.num);
    			fprintf(output, "%02x",(int)ttop->opds[1]->value.num);
				

    			if(ttop->opds[2]->type == LABEL){
    				fprintf(output, "%02x",stable_find(alias_table,ttop->opds[2]->value.label)->i);			
    			}
    			else{
    				fprintf(output, "%02x",(int)ttop->opds[2]->value.num);
    			}
    		}
    		else if(!strcmp(name,"LDWU")){
    			if(ttop->opds[2]->type == REGISTER){
    				fprintf(output, "0a");
    			}
    			else{
    				fprintf(output, "0b");
    			}

    			fprintf(output, "%02x",(int)ttop->opds[0]->value.num);
    			fprintf(output, "%02x",(int)ttop->opds[1]->value.num);
				

    			if(ttop->opds[2]->type == LABEL){
    				fprintf(output, "%02x",stable_find(alias_table,ttop->opds[2]->value.label)->i);			
    			}
    			else{
    				fprintf(output, "%02x",(int)ttop->opds[2]->value.num);
    			}
    		}
    		else if(!strcmp(name,"LDTU")){
    			if(ttop->opds[2]->type == REGISTER){
    				fprintf(output, "0c");
    			}
    			else{
    				fprintf(output, "0d");
    			}

    			fprintf(output, "%02x",(int)ttop->opds[0]->value.num);
    			fprintf(output, "%02x",(int)ttop->opds[1]->value.num);
				

    			if(ttop->opds[2]->type == LABEL){
    				fprintf(output, "%02x",stable_find(alias_table,ttop->opds[2]->value.label)->i);			
    			}
    			else{
    				fprintf(output, "%02x",(int)ttop->opds[2]->value.num);
    			}
    		}
    		else if(!strcmp(name,"LDOU")){
    			if(ttop->opds[2]->type == REGISTER){
    				fprintf(output, "0e");
    			}
    			else{
    				fprintf(output, "0f");
    			}

    			fprintf(output, "%02x",(int)ttop->opds[0]->value.num);
    			fprintf(output, "%02x",(int)ttop->opds[1]->value.num);
				

    			if(ttop->opds[2]->type == LABEL){
    				fprintf(output, "%02x",stable_find(alias_table,ttop->opds[2]->value.label)->i);			
    			}
    			else{
    				fprintf(output, "%02x",(int)ttop->opds[2]->value.num);
    			}
    		}
    		else if(!strcmp(name,"STB")){
    			if(ttop->opds[2]->type == REGISTER){
    				fprintf(output, "10");
    			}
    			else{
    				fprintf(output, "11");
    			}

    			fprintf(output, "%02x",(int)ttop->opds[0]->value.num);
    			fprintf(output, "%02x",(int)ttop->opds[1]->value.num);
				

    			if(ttop->opds[2]->type == LABEL){
    				fprintf(output, "%02x",stable_find(alias_table,ttop->opds[2]->value.label)->i);			
    			}
    			else{
    				fprintf(output, "%02x",(int)ttop->opds[2]->value.num);
    			}
    		}
    		else if(!strcmp(name,"STW")){
    			if(ttop->opds[2]->type == REGISTER){
    				fprintf(output, "12");
    			}
    			else{
    				fprintf(output, "13");
    			}

    			fprintf(output, "%02x",(int)ttop->opds[0]->value.num);
    			fprintf(output, "%02x",(int)ttop->opds[1]->value.num);
				

    			if(ttop->opds[2]->type == LABEL){
    				fprintf(output, "%02x",stable_find(alias_table,ttop->opds[2]->value.label)->i);			
    			}
    			else{
    				fprintf(output, "%02x",(int)ttop->opds[2]->value.num);
    			}
    		}
    		else if(!strcmp(name,"STT")){
    			if(ttop->opds[2]->type == REGISTER){
    				fprintf(output, "14");
    			}
    			else{
    				fprintf(output, "15");
    			}

    			fprintf(output, "%02x",(int)ttop->opds[0]->value.num);
    			fprintf(output, "%02x",(int)ttop->opds[1]->value.num);
				

    			if(ttop->opds[2]->type == LABEL){
    				fprintf(output, "%02x",stable_find(alias_table,ttop->opds[2]->value.label)->i);			
    			}
    			else{
    				fprintf(output, "%02x",(int)ttop->opds[2]->value.num);
    			}
    		}
    		else if(!strcmp(name,"STO")){
    			if(ttop->opds[2]->type == REGISTER){
    				fprintf(output, "16");
    			}
    			else{
    				fprintf(output, "17");
    			}

    			fprintf(output, "%02x",(int)ttop->opds[0]->value.num);
    			fprintf(output, "%02x",(int)ttop->opds[1]->value.num);
				

    			if(ttop->opds[2]->type == LABEL){
    				fprintf(output, "%02x",stable_find(alias_table,ttop->opds[2]->value.label)->i);			
    			}
    			else{
    				fprintf(output, "%02x",(int)ttop->opds[2]->value.num);
    			}
    		}
    		else if(!strcmp(name,"STBU")){
    			if(ttop->opds[2]->type == REGISTER){
    				fprintf(output, "18");
    			}
    			else{
    				fprintf(output, "19");
    			}

    			fprintf(output, "%02x",(int)ttop->opds[0]->value.num);
    			fprintf(output, "%02x",(int)ttop->opds[1]->value.num);
				

    			if(ttop->opds[2]->type == LABEL){
    				fprintf(output, "%02x",stable_find(alias_table,ttop->opds[2]->value.label)->i);			
    			}
    			else{
    				fprintf(output, "%02x",(int)ttop->opds[2]->value.num);
    			}
    		}
    		else if(!strcmp(name,"STWU")){
    			if(ttop->opds[2]->type == REGISTER){
    				fprintf(output, "1a");
    			}
    			else{
    				fprintf(output, "1b");
    			}

    			fprintf(output, "%02x",(int)ttop->opds[0]->value.num);
    			fprintf(output, "%02x",(int)ttop->opds[1]->value.num);
				

    			if(ttop->opds[2]->type == LABEL){
    				fprintf(output, "%02x",stable_find(alias_table,ttop->opds[2]->value.label)->i);			
    			}
    			else{
    				fprintf(output, "%02x",(int)ttop->opds[2]->value.num);
    			}
    		}
    		else if(!strcmp(name,"STTU")){
    			if(ttop->opds[2]->type == REGISTER){
    				fprintf(output, "1c");
    			}
    			else{
    				fprintf(output, "1d");
    			}

    			fprintf(output, "%02x",(int)ttop->opds[0]->value.num);
    			fprintf(output, "%02x",(int)ttop->opds[1]->value.num);
				

    			if(ttop->opds[2]->type == LABEL){
    				fprintf(output, "%02x",stable_find(alias_table,ttop->opds[2]->value.label)->i);			
    			}
    			else{
    				fprintf(output, "%02x",(int)ttop->opds[2]->value.num);
    			}
    		}
    		else if(!strcmp(name,"STOU")){
    			if(ttop->opds[2]->type == REGISTER){
    				fprintf(output, "1e");
    			}
    			else{
    				fprintf(output, "1f");
    			}

    			fprintf(output, "%02x",(int)ttop->opds[0]->value.num);
    			fprintf(output, "%02x",(int)ttop->opds[1]->value.num);
				

    			if(ttop->opds[2]->type == LABEL){
    				fprintf(output, "%02x",stable_find(alias_table,ttop->opds[2]->value.label)->i);			
    			}
    			else{
    				fprintf(output, "%02x",(int)ttop->opds[2]->value.num);
    			}
    		}
    		else if(!strcmp(name,"ADD")){
    			if(ttop->opds[2]->type == REGISTER){
    				fprintf(output, "20");
    			}
    			else{
    				fprintf(output, "21");
    			}

    			fprintf(output, "%02x",(int)ttop->opds[0]->value.num);
    			fprintf(output, "%02x",(int)ttop->opds[1]->value.num);
				

    			if(ttop->opds[2]->type == LABEL){
    				fprintf(output, "%02x",stable_find(alias_table,ttop->opds[2]->value.label)->i);			
    			}
    			else{
    				fprintf(output, "%02x",(int)ttop->opds[2]->value.num);
    			}
    		}
    		else if(!strcmp(name,"SUB")){
    			if(ttop->opds[2]->type == REGISTER){
    				fprintf(output, "22");
    			}
    			else{
    				fprintf(output, "23");
    			}

    			fprintf(output, "%02x",(int)ttop->opds[0]->value.num);
    			fprintf(output, "%02x",(int)ttop->opds[1]->value.num);
				

    			if(ttop->opds[2]->type == LABEL){
    				fprintf(output, "%02x",stable_find(alias_table,ttop->opds[2]->value.label)->i);			
    			}
    			else{
    				fprintf(output, "%02x",(int)ttop->opds[2]->value.num);
    			}
    		}
    		else if(!strcmp(name,"MUL")){
    			if(ttop->opds[2]->type == REGISTER){
    				fprintf(output, "24");
    			}
    			else{
    				fprintf(output, "25");
    			}

    			fprintf(output, "%02x",(int)ttop->opds[0]->value.num);
    			fprintf(output, "%02x",(int)ttop->opds[1]->value.num);
				

    			if(ttop->opds[2]->type == LABEL){
    				fprintf(output, "%02x",stable_find(alias_table,ttop->opds[2]->value.label)->i);			
    			}
    			else{
    				fprintf(output, "%02x",(int)ttop->opds[2]->value.num);
    			}
    		}
    		else if(!strcmp(name,"DIV")){
    			if(ttop->opds[2]->type == REGISTER){
    				fprintf(output, "26");
    			}
    			else{
    				fprintf(output, "27");
    			}

    			fprintf(output, "%02x",(int)ttop->opds[0]->value.num);
    			fprintf(output, "%02x",(int)ttop->opds[1]->value.num);
				

    			if(ttop->opds[2]->type == LABEL){
    				fprintf(output, "%02x",stable_find(alias_table,ttop->opds[2]->value.label)->i);			
    			}
    			else{
    				fprintf(output, "%02x",(int)ttop->opds[2]->value.num);
    			}
    		}
    		else if(!strcmp(name,"CMP")){
    			if(ttop->opds[2]->type == REGISTER){
    				fprintf(output, "28");
    			}
    			else{
    				fprintf(output, "29");
    			}

    			fprintf(output, "%02x",(int)ttop->opds[0]->value.num);
    			fprintf(output, "%02x",(int)ttop->opds[1]->value.num);
				

    			if(ttop->opds[2]->type == LABEL){
    				fprintf(output, "%02x",stable_find(alias_table,ttop->opds[2]->value.label)->i);			
    			}
    			else{
    				fprintf(output, "%02x",(int)ttop->opds[2]->value.num);
    			}
    		}
    		else if(!strcmp(name,"SL")){
    			if(ttop->opds[2]->type == REGISTER){
    				fprintf(output, "2a");
    			}
    			else{
    				fprintf(output, "2b");
    			}

    			fprintf(output, "%02x",(int)ttop->opds[0]->value.num);
    			fprintf(output, "%02x",(int)ttop->opds[1]->value.num);
				

    			if(ttop->opds[2]->type == LABEL){
    				fprintf(output, "%02x",stable_find(alias_table,ttop->opds[2]->value.label)->i);			
    			}
    			else{
    				fprintf(output, "%02x",(int)ttop->opds[2]->value.num);
    			}
    		}
    		else if(!strcmp(name,"SR")){
    			if(ttop->opds[2]->type == REGISTER){
    				fprintf(output, "2c");
    			}
    			else{
    				fprintf(output, "2d");
    			}

    			fprintf(output, "%02x",(int)ttop->opds[0]->value.num);
    			fprintf(output, "%02x",(int)ttop->opds[1]->value.num);
				

    			if(ttop->opds[2]->type == LABEL){
    				fprintf(output, "%02x",stable_find(alias_table,ttop->opds[2]->value.label)->i);			
    			}
    			else{
    				fprintf(output, "%02x",(int)ttop->opds[2]->value.num);
    			}
    		}
    		else if(!strcmp(name,"NEG")){
    			if(ttop->opds[2]->type == REGISTER){
    				fprintf(output, "2e");
    			}
    			else{
    				fprintf(output, "2f");
    			}

    			fprintf(output, "%02x",(int)ttop->opds[0]->value.num);
    			fprintf(output, "%02x",(int)ttop->opds[1]->value.num);
				

    			if(ttop->opds[2]->type == LABEL){
    				fprintf(output, "%02x",stable_find(alias_table,ttop->opds[2]->value.label)->i);			
    			}
    			else{
    				fprintf(output, "%02x",(int)ttop->opds[2]->value.num);
    			}
    		}
    		else if(!strcmp(name,"ADDU")){
    			if(ttop->opds[2]->type == REGISTER){
    				fprintf(output, "30");
    			}
    			else{
    				fprintf(output, "31");
    			}

    			fprintf(output, "%02x",(int)ttop->opds[0]->value.num);
    			fprintf(output, "%02x",(int)ttop->opds[1]->value.num);
				

    			if(ttop->opds[2]->type == LABEL){
    				fprintf(output, "%02x",stable_find(alias_table,ttop->opds[2]->value.label)->i);			
    			}
    			else{
    				fprintf(output, "%02x",(int)ttop->opds[2]->value.num);
    			}
    		}
    		else if(!strcmp(name,"SUBU")){
    			if(ttop->opds[2]->type == REGISTER){
    				fprintf(output, "32");
    			}
    			else{
    				fprintf(output, "33");
    			}

    			fprintf(output, "%02x",(int)ttop->opds[0]->value.num);
    			fprintf(output, "%02x",(int)ttop->opds[1]->value.num);
				

    			if(ttop->opds[2]->type == LABEL){
    				fprintf(output, "%02x",stable_find(alias_table,ttop->opds[2]->value.label)->i);			
    			}
    			else{
    				fprintf(output, "%02x",(int)ttop->opds[2]->value.num);
    			}
    		}
    		else if(!strcmp(name,"MULU")){
    			if(ttop->opds[2]->type == REGISTER){
    				fprintf(output, "34");
    			}
    			else{
    				fprintf(output, "35");
    			}

    			fprintf(output, "%02x",(int)ttop->opds[0]->value.num);
    			fprintf(output, "%02x",(int)ttop->opds[1]->value.num);
				

    			if(ttop->opds[2]->type == LABEL){
    				fprintf(output, "%02x",stable_find(alias_table,ttop->opds[2]->value.label)->i);			
    			}
    			else{
    				fprintf(output, "%02x",(int)ttop->opds[2]->value.num);
    			}
    		}
    		else if(!strcmp(name,"DIVU")){
    			if(ttop->opds[2]->type == REGISTER){
    				fprintf(output, "36");
    			}
    			else{
    				fprintf(output, "37");
    			}

    			fprintf(output, "%02x",(int)ttop->opds[0]->value.num);
    			fprintf(output, "%02x",(int)ttop->opds[1]->value.num);
				

    			if(ttop->opds[2]->type == LABEL){
    				fprintf(output, "%02x",stable_find(alias_table,ttop->opds[2]->value.label)->i);			
    			}
    			else{
    				fprintf(output, "%02x",(int)ttop->opds[2]->value.num);
    			}
    		}
    		else if(!strcmp(name,"CMPU")){
    			if(ttop->opds[2]->type == REGISTER){
    				fprintf(output, "38");
    			}
    			else{
    				fprintf(output, "39");
    			}

    			fprintf(output, "%02x",(int)ttop->opds[0]->value.num);
    			fprintf(output, "%02x",(int)ttop->opds[1]->value.num);
				

    			if(ttop->opds[2]->type == LABEL){
    				fprintf(output, "%02x",stable_find(alias_table,ttop->opds[2]->value.label)->i);			
    			}
    			else{
    				fprintf(output, "%02x",(int)ttop->opds[2]->value.num);
    			}
    		}
    		else if(!strcmp(name,"SLU")){
    			if(ttop->opds[2]->type == REGISTER){
    				fprintf(output, "3a");
    			}
    			else{
    				fprintf(output, "3b");
    			}

    			fprintf(output, "%02x",(int)ttop->opds[0]->value.num);
    			fprintf(output, "%02x",(int)ttop->opds[1]->value.num);
				

    			if(ttop->opds[2]->type == LABEL){
    				fprintf(output, "%02x",stable_find(alias_table,ttop->opds[2]->value.label)->i);			
    			}
    			else{
    				fprintf(output, "%02x",(int)ttop->opds[2]->value.num);
    			}
    		}
    		else if(!strcmp(name,"SRU")){
    			if(ttop->opds[2]->type == REGISTER){
    				fprintf(output, "3c");
    			}
    			else{
    				fprintf(output, "3d");
    			}

    			fprintf(output, "%02x",(int)ttop->opds[0]->value.num);
    			fprintf(output, "%02x",(int)ttop->opds[1]->value.num);
				

    			if(ttop->opds[2]->type == LABEL){
    				fprintf(output, "%02x",stable_find(alias_table,ttop->opds[2]->value.label)->i);			
    			}
    			else{
    				fprintf(output, "%02x",(int)ttop->opds[2]->value.num);
    			}
    		}
    		else if(!strcmp(name,"NEGU")){
    			if(ttop->opds[2]->type == REGISTER){
    				fprintf(output, "3e");
    			}
    			else{
    				fprintf(output, "3f");
    			}

    			fprintf(output, "%02x",(int)ttop->opds[0]->value.num);
    			fprintf(output, "%02x",(int)ttop->opds[1]->value.num);
				

    			if(ttop->opds[2]->type == LABEL){
    				fprintf(output, "%02x",stable_find(alias_table,ttop->opds[2]->value.label)->i);			
    			}
    			else{
    				fprintf(output, "%02x",(int)ttop->opds[2]->value.num);
    			}
    		}
    		else if(!strcmp(name,"AND")){
    			if(ttop->opds[2]->type == REGISTER){
    				fprintf(output, "40");
    			}
    			else{
    				fprintf(output, "41");
    			}

    			fprintf(output, "%02x",(int)ttop->opds[0]->value.num);
    			fprintf(output, "%02x",(int)ttop->opds[1]->value.num);
				

    			if(ttop->opds[2]->type == LABEL){
    				fprintf(output, "%02x",stable_find(alias_table,ttop->opds[2]->value.label)->i);			
    			}
    			else{
    				fprintf(output, "%02x",(int)ttop->opds[2]->value.num);
    			}
    		}
    		else if(!strcmp(name,"OR")){
    			if(ttop->opds[2]->type == REGISTER){
    				fprintf(output, "42");
    			}
    			else{
    				fprintf(output, "43");
    			}

    			fprintf(output, "%02x",(int)ttop->opds[0]->value.num);
    			fprintf(output, "%02x",(int)ttop->opds[1]->value.num);
				

    			if(ttop->opds[2]->type == LABEL){
    				fprintf(output, "%02x",stable_find(alias_table,ttop->opds[2]->value.label)->i);			
    			}
    			else{
    				fprintf(output, "%02x",(int)ttop->opds[2]->value.num);
    			}
    		}
    		else if(!strcmp(name,"XOR")){
    			if(ttop->opds[2]->type == REGISTER){
    				fprintf(output, "44");
    			}
    			else{
    				fprintf(output, "45");
    			}

    			fprintf(output, "%02x",(int)ttop->opds[0]->value.num);
    			fprintf(output, "%02x",(int)ttop->opds[1]->value.num);
				

    			if(ttop->opds[2]->type == LABEL){
    				fprintf(output, "%02x",stable_find(alias_table,ttop->opds[2]->value.label)->i);			
    			}
    			else{
    				fprintf(output, "%02x",(int)ttop->opds[2]->value.num);
    			}
    		}
    		else if(!strcmp(name,"NXOR")){
    			if(ttop->opds[2]->type == REGISTER){
    				fprintf(output, "46");
    			}
    			else{
    				fprintf(output, "47");
    			}

    			fprintf(output, "%02x",(int)ttop->opds[0]->value.num);
    			fprintf(output, "%02x",(int)ttop->opds[1]->value.num);
				

    			if(ttop->opds[2]->type == LABEL){
    				fprintf(output, "%02x",stable_find(alias_table,ttop->opds[2]->value.label)->i);			
    			}
    			else{
    				fprintf(output, "%02x",(int)ttop->opds[2]->value.num);
    			}
    		}
    		else if(!strcmp(name,"JMP")){
    			int i;
    			if(ttop->opds[0]->type == LABEL){
    				i = stable_find(label_table,ttop->opds[0]->value.label)->i - line;		
    			}
    			else{
    				i = ttop->opds[0]->value.num - line;
    			}

    			if(i > 0){
    				fprintf(output, "48");
    			}
    			else{
    				fprintf(output, "49");
    				i = -i;
    			}
    			fprintf(output, "%06x",i);
    		}
    		else if(!strcmp(name,"JZ")){
    			int i;
    			if(ttop->opds[1]->type == LABEL){
    				i = stable_find(label_table,ttop->opds[1]->value.label)->i - line;		
    			}
    			else{
    				i = ttop->opds[1]->value.num - line;
    			}

    			if(i > 0){
    				fprintf(output, "4a");
    			}
    			else{
    				fprintf(output, "4b");
    				i = -i;
    			}
    			fprintf(output, "%02x",(int)ttop->opds[0]->value.num);
    			fprintf(output, "%04x",i);
    		}
    		else if(!strcmp(name,"JNZ")){
    			int i;
    			if(ttop->opds[1]->type == LABEL){
    				i = stable_find(label_table,ttop->opds[1]->value.label)->i - line;		
    			}
    			else{
    				i = ttop->opds[1]->value.num - line;
    			}

    			if(i > 0){
    				fprintf(output, "4c");
    			}
    			else{
    				fprintf(output, "4d");
    				i = -i;
    			}
    			fprintf(output, "%02x",(int)ttop->opds[0]->value.num);
    			fprintf(output, "%04x",i);
    		}
    		else if(!strcmp(name,"JP")){
    			int i;
    			if(ttop->opds[1]->type == LABEL){
    				i = stable_find(label_table,ttop->opds[1]->value.label)->i - line;		
    			}
    			else{
    				i = ttop->opds[1]->value.num - line;
    			}

    			if(i > 0){
    				fprintf(output, "4e");
    			}
    			else{
    				fprintf(output, "4f");
    				i = -i;
    			}
    			fprintf(output, "%02x",(int)ttop->opds[0]->value.num);
    			fprintf(output, "%04x",i);
    		}
    		else if(!strcmp(name,"JN")){
    			int i;
    			if(ttop->opds[1]->type == LABEL){
    				i = stable_find(label_table,ttop->opds[1]->value.label)->i - line;		
    			}
    			else{
    				i = ttop->opds[1]->value.num - line;
    			}

    			if(i > 0){
    				fprintf(output, "50");
    			}
    			else{
    				fprintf(output, "51");
    				i = -i;
    			}
    			fprintf(output, "%02x",(int)ttop->opds[0]->value.num);
    			fprintf(output, "%04x",i);
    		}
    		else if(!strcmp(name,"JNN")){
    			int i;
    			if(ttop->opds[1]->type == LABEL){
    				i = stable_find(label_table,ttop->opds[1]->value.label)->i - line;		
    			}
    			else{
    				i = ttop->opds[1]->value.num - line;
    			}

    			if(i > 0){
    				fprintf(output, "52");
    			}
    			else{
    				fprintf(output, "53");
    				i = -i;
    			}
    			fprintf(output, "%02x",(int)ttop->opds[0]->value.num);
    			fprintf(output, "%04x",i);
    		}
    		else if(!strcmp(name,"JNP")){
    			int i;
    			if(ttop->opds[1]->type == LABEL){
    				i = stable_find(label_table,ttop->opds[1]->value.label)->i - line;		
    			}
    			else{
    				i = ttop->opds[1]->value.num - line;
    			}

    			if(i > 0){
    				fprintf(output, "54");
    			}
    			else{
    				fprintf(output, "55");
    				i = -i;
    			}
    			fprintf(output, "%02x",(int)ttop->opds[0]->value.num);
    			fprintf(output, "%04x",i);
    		}
    		else if(!strcmp(name,"GO")){
    			int i;
    			if(ttop->opds[1]->type == LABEL){
    				i = stable_find(label_table,ttop->opds[1]->value.label)->i - line;		
    			}
    			else{
    				i = ttop->opds[1]->value.num - line;
    			}

    			if(i > 0){
    				fprintf(output, "56");
    			}
    			else{
    				fprintf(output, "57");
    				i = -i;
    			}
    			fprintf(output, "%02x",(int)ttop->opds[0]->value.num);
    			fprintf(output, "%04x",i);
    		}
    		else if(!strcmp(name,"GETA")){
    			int i;
    			if(ttop->opds[1]->type == LABEL){
    				i = stable_find(label_table,ttop->opds[1]->value.label)->i - line;		
    			}
    			else{
    				i = ttop->opds[1]->value.num - line;
    			}

    			if(i > 0){
    				fprintf(output, "58");
    			}
    			else{
    				fprintf(output, "59");
    				i = -i;
    			}
    			fprintf(output, "%02x",(int)ttop->opds[0]->value.num);
    			fprintf(output, "%04x",i);
    		}
    		else if(!strcmp(name,"SETW")){
    			fprintf(output, "5a");
    			fprintf(output, "%02x",(int)ttop->opds[0]->value.num);
    			fprintf(output, "%04x",(int)ttop->opds[1]->value.num);
    		}
    		else if(!strcmp(name,"SAVE")){
    			fprintf(output, "5b");
    			fprintf(output, "%02x",(int)ttop->opds[0]->value.num);
    			fprintf(output, "%02x",(int)ttop->opds[1]->value.num);
    			fprintf(output, "%02x",(int)ttop->opds[2]->value.num);
    		}
    		else if(!strcmp(name,"REST")){
    			fprintf(output, "5c");
    			fprintf(output, "%02x",(int)ttop->opds[0]->value.num);
    			fprintf(output, "%02x",(int)ttop->opds[1]->value.num);
    			fprintf(output, "%02x",(int)ttop->opds[2]->value.num);
    		}
    		else if(!strcmp(name,"INT")){
    			fprintf(output, "fe");
    			fprintf(output, "%06d",(int)ttop->opds[0]->value.num);
    		}
    		else if(!strcmp(name,"NOP")){
    			fprintf(output, "ff000000");
    		}
            fprintf(output, "\n");

    	}
        else{
            if(!strcmp(name,"PUSH")){
                fprintf(output,"1f");

                if(ttop->opds[0]->type == LABEL){
                    fprintf(output,"%02x",stable_find(alias_table,ttop->opds[0]->value.label)->i);          
                }
                else{
                    fprintf(output,"%02x",(int)ttop->opds[0]->value.num);
                }
                
                fprintf(output,"fd%02x",pushed);
                pushed = pushed+8;

                fprintf(output, "\n");
                continue;
            }

            if(pushed>0){
                fprintf(output,"31fdfd%02x\n",pushed);
                line++;
                pushed = 0;
            }
            
            if(!strcmp(name,"CALL")){
                fprintf(output,"58fa0004\n1ffafd00\n31fdfd08");
                line = line + 3;

                int i;
                if(ttop->opds[0]->type == LABEL){
                    i = stable_find(label_table,ttop->opds[0]->value.label)->i - line;      
                }
                else{
                    i = ttop->opds[0]->value.num - line;
                }

                if(i > 0){
                    fprintf(output,"48");
                }
                else{
                    fprintf(output,"49");
                    i = -i;
                }
                fprintf("%06x",i);
            }
            else if(!strcmp(name,"RET")){
                int i = ((int)ttop->opds[0]->value.num) * 8;

                fprintf(output,"33fdfd%02x\n0ffafd%02x\n56fa0000",i+8,i);
                line = line + 2;
            }
            else if(!strcmp(name,"STR")){
                char* str = ttop->opds[0]->value.str;
                int c = 0;
                int cont = 1;

                while(str[c]!='\n' && str[c]!= EOF && str[c] !='\0' && str[c] != '*'){
                    fprintf(output,"%02x",str[c]);
                    cont++;

                    if(cont == 5){
                        line++;
                        fprintf(output, "\n");
                        cont = 1;
                    }

                    c++;
                }

                while(cont!=1 && cont<5){
                    fprintf(output,"00");
                    cont++;
                }
            }
            else if(!strcmp(name,"EXTERN")){
                fprintf(output, "EXTERN %s",ttop->opds[0]->value.label)
            }

            fprintf(output, "\n");

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
