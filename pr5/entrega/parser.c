#include "parser.h"
#include "optable.h"
#include "error.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int parse(const char *s, SymbolTable alias_table, SymbolTable label_table,
          SymbolTable extern_table, Instruction **instr, const char **errptr){

	int k = 0; //indice do começo da palavra
	int i = 0; //indice que percorre s / indice do fim da palavra
	int l = 0; //tamanho da palavra

	const Operator *op;
	Operand *opds[3];

	//acha o fim da palavra
	while(!isspace(s[i])) {
		if (s[i] == '*') break;
		i++;
	}

	if (i == 0 && (s[i] == '\n' || s[i] == '*' || s[i] == EOF)) {
		//linha sem codigo, voltar
		return 1;
	}

	l = i-k;

	char label[l + 1];

	if(l>0){
		for (int c = 0; c < l; c++, k++) label[c] = s[k];
		label[l] = '\0';
	}

	// coloca i e k no inicio da proxima palavra
	while (isspace(s[i]) && s[i] != '\0') i++;
	k = i;

	op = optable_find(label);

	if(op ==  NULL) { //então temos que procurar o op

		//printf(">Não era um OP, aparentemente.\n");

		while(isspace(s[k])) k++;
		i = k;
		while(!isspace(s[i]) && s[i] != '\0') i++;

		l = i-k;	

		char t_op[l + 1];
		for (int c = 0; c < l; c++, k++) t_op[c] = s[k];
		t_op[l] = '\0';

		op = optable_find(t_op);

		if(op == NULL){
			*errptr = &s[k];
			set_error_msg("Invalid operator."); //Basicamente se ele não char um op até aqui,
			return 0; 							//Assumimos que falta o op.
		}

		//printf(">%s é o OP que encontrei.\n",t_op);

	}
	else{
		label[0] = '\0';
	}

	//Caso especial para STR

	if(!strcmp(op->name,"STR")){
		k = i;
		 //acha o inicio do texto
		while(s[k] != '\"' && s[k] != ',' && s[k] != '\0' && s[k] != EOF && s[k] != '\n') k++; 
		if(s[k] != '\"'){
			*errptr = &s[k];
			set_error_msg("Missing String start ( Did you forget the \"? )");
			return 0;
		}
		i = k+1;
		while(s[i] != '\"' && s[i] != '\0' && s[i] != EOF) i++;
		if(s[i] != '\"'){
			*errptr = &s[k];
			set_error_msg("Missing String end ( Did you forget the \" ? )");
			return 0;
		}

		k = k+1;

		l = i-k;

		char t_opd[l + 1];
		for (int c = 0; c < l; c++, k++) t_opd[c] = s[k];
		t_opd[l] = '\0';

		opds[0] = operand_create_string(t_opd);
		opds[1] = NULL;
		opds[2] = NULL;

	}
	else{
		// identifica quantos operandos sao necessarios
		int operands = 3;
		if (op->opd_types[2] == OP_NONE){ operands = 2; opds[2] = NULL; }
		if (op->opd_types[1] == OP_NONE){ operands = 1; opds[1] = NULL; }
		if (op->opd_types[0] == OP_NONE){ operands = 0; opds[0] = NULL; }

		//printf(">Precisamos de %d openrads\n",operands);

		for(int z = 0; z < operands; z++){

			//printf(">Indo encontrar o openrand %d\n",z);

			k = i;
			while(isspace(s[k])) k++; //acha o inicio do argumento
			i = k;
			while(!isspace(s[i]) && s[i] != ',' && s[i] != '\0' && s[i] != EOF) i++;

			l = i-k;

			if(op->opd_types[z] == OP_NONE && l != 0){ //ele não precisa mais de argumentos, mas achou um
				*errptr = &s[k];
				set_error_msg("Wrong number of operands.");
				return 0;
			}

			char t_opd[l + 1];
			for (int c = 0; c < l; c++, k++) t_opd[c] = s[k];
			t_opd[l] = '\0';

			//printf(">Achei %s\n", t_opd);

			if(t_opd[0] == '$') {

				// aqui fazemos uma operação bitwise para comparar os possiveis
				// tipos do operando com os aceitados pelo operador
				//
				// creditos para essa pagina:
				// https://paca.ime.usp.br/mod/forum/discuss.php?d=38234

				if(op->opd_types[z] & REGISTER) {
					//Isso signigica que o cara só mandou um '$'
					if(l < 2) {
						*errptr = &s[k];
						set_error_msg("Missing register number.");
						return 0;
					}
					//calcular o numero do registrador
					int num = 0;
					for(int j = 1; j<l; j++){
						if(!isdigit(t_opd[j])){ //evitar algo do tipo "$1a"
							*errptr = &s[k];
							set_error_msg("Invalid register number.");
							return 0;
						}
						num = num*10 + (t_opd[j] - '0');
					}
					//printf(">É um registrador de num %d\n",num);
					opds[z] = operand_create_register((unsigned char)num);
				}
				else {
					*errptr = &s[k];
					set_error_msg("Wrong operand type: Expected %hhx, got REGISTER.",op->opd_types[z]);
					return 0;
				}
			}

			else if(isdigit(t_opd[0]) || ( t_opd[0] == '#' && l>1 ) ){ // então deve ser um numero, né?

				if(op->opd_types[z] & NUMBER_TYPE){
					int num = 0;
					int j = 0;
					if(t_opd[0] == '#') j = 1;
					for(; j<l; j++){
						if(!isdigit(t_opd[j])){ //evitar algo do tipo "$1a"
							*errptr = &s[k];
							set_error_msg("Invalid register number.");
							return 0;
						}
						num = num*10 + (t_opd[j] - '0');
					}
					opds[z] = operand_create_number((octa)num);
				}
				else{
					*errptr = &s[k];
					set_error_msg("Wrong operand type: Expected %hhx, got NUMBER_TYPE.",op->opd_types[z]);
					return 0;
				}
			}

			else { //é o que então?
				EntryData *test = stable_find(alias_table, t_opd);

				if(test == NULL){ // então não é um label - registrador

					test = stable_find(label_table, t_opd);

					if(test == NULL){
						if(op->opd_types[z] & STRING){
							opds[z] = operand_create_string(t_opd);
						}
						else {
							*errptr = &s[k];
							set_error_msg("Wrong operand type: Expected %hhx, got STRING.",op->opd_types[z]);
							return 0;
						}
					}
					else{
						if(op->opd_types[z] & LABEL){
							opds[z] = operand_create_label(t_opd);
						}
						else {
							*errptr = &s[k];
							set_error_msg("Wrong operand type: Expected %hhx, got LABEL.",op->opd_types[z]);
							return 0;
						}
						if(!strcmp(op->name,"EXTERN")){
							//printf(">Vou adicionar um label extern\n");
							stable_insert(extern_table, t_opd);
	    					stable_find(extern_table, t_opd)->i = test->i;
						}
					}
				}
				else{ // é um label-registrador
					if(op->opd_types[z] & REGISTER) { //verifica para o que a label aponta de verdade
							opds[z] = operand_create_register(test->opd->value.num);
					}
					else {
						*errptr = &s[k];
						set_error_msg("Wrong operand type: Expected %hhx, got REGISTER.",op->opd_types[z]);
						return 0;
					}
				}
			}

			i++;
		}
	}

	Instruction *new = instr_create(label,op,opds);	//Cria a instruction nova
	new->next = *instr;								//Faz ela apontar para a instrução anterior
	*instr = new;									//Agora ela está no topo

	return 1;
}

