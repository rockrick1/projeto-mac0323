#include "parser.h"
#include "optable.h"
#include "error.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int parse(const char *s, SymbolTable alias_table, Instruction **instr, const char **errptr){

	//test printf("\nEntrei no parse!\n");

	//Assumo que o buffer tira todos os espaços antes do começo da palavra

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
		//test printf("Achei uma linha sem codigo, vou voltar.\n");
		return 1;
	}

	l = i-k;

	char label[l + 1];

	//Copia a palavra se ela existir
	if(l>0){
		//test printf("Preparando para copiar a palavra com %d chars.\n",l);

		for (int c = 0; c < l; c++, k++) label[c] = s[k];
		label[l] = '\0';

		//test printf("Palavra copiada com sucesso, tamanho %d .\n",l);
	}
	else{
		//test printf("Encontrei uma palavra vazia.\n");
	}


	// coloca i e k no inicio da proxima palavra
	while (isspace(s[i]) && s[i] != '\0') i++;
	k = i;

	op = optable_find(label);

	//Se op != null, na verdade encontramos um Operator
	if(l>0 && op == NULL) {

		//test printf("label: %s\n",label);

	}
	else { //é um OP então, olha que legal
		//test printf("Acredito que a palavra seja um operator.\n");

		// não sei se precisa de algo aqui TBH
		// seria para deixar um o op guardado,
		// mas acho que da para usar o op que a gente checa o null

		//talvez desalocar o tlabel ou algo assim?

	}

	if(label !=  NULL) { //então temos que procurar o op
		//test printf("\nVou começar a procurar o operator!\n");

		while(isspace(s[k])) k++; //acha o inicio da palavra
		//test printf("Achei o começo da palavra.\n");
		i = k;
		// acha o fim da palavra
		while(!isspace(s[i]) && s[i] != '\0') i++;
		//test printf("Achei o fim da palavra.\n");
		/*
		if(s[i]=='\n' || s[i]=='*' || s[i]==EOF){
			errptr = &s[k];							Acho que o teste abaixo já resolve esse caso,
			set_error_msg("Missing Operator."); 	mas achei melhor deixar ele aqui por enquanto.
			return 0; 								Vai que.
		}
		*/

		l = i-k;
		

		//test printf("Preparando para copiar a palavra com %d chars.\n",l);
		char t_op[l + 1];
		for (int c = 0; c < l; c++, k++) t_op[c] = s[k];
		t_op[l] = '\0';
		//test printf("operator: %s\n", t_op);
		//test printf("Palavra copiada com sucesso.\n");

		op = optable_find(t_op);

		if(op == NULL){
			*errptr = &s[k];
			set_error_msg("Invalid operator."); //Basicamente se ele não char um op até aqui,
			return 0; 							//Assumimos que falta o op.
		}

	}

	//test printf("\nIndo pegar os operands.\n");

	//Deem uma olhada no optable.c
	//Basicamente cada operator tem um vertor com o que ele precisa.
	//Se não precisar de argumentos, ele fala OP_NONE, assim:

	// identifica quantos operandos sao necessarios
	int operands = 3;
	if (op->opd_types[2] == OP_NONE){ operands = 2; opds[2] = NULL; }
	if (op->opd_types[1] == OP_NONE){ operands = 1; opds[1] = NULL; }
	if (op->opd_types[0] == OP_NONE){ operands = 0; opds[0] = NULL; }

	for(int z = 0; z < operands; z++){

		//test printf("\n\nVou procurar o operand %d!\n",z);

		k = i;
		while(isspace(s[k])) k++; //acha o inicio do argumento
		//test printf("Achei o inicio do operand.\n");
		i = k;
		// acha o fim do argumento
		while(!isspace(s[i]) && s[i] != ',' && s[i] != '\0' && s[i] != EOF) i++;

		l = i-k;
		//test printf("Achei o fim do operand, de tamanho %d.\n",l);

		if(op->opd_types[z] == OP_NONE && l != 0){ //ele não precisa mais de argumentos, mas achou um
			*errptr = &s[k];
			set_error_msg("Wrong number of operands.");
			return 0;
		}

		//test printf("Vou começar a copiar esse operand.\n");

		char t_opd[l + 1];
		for (int c = 0; c < l; c++, k++) t_opd[c] = s[k];
		t_opd[l] = '\0';
		//test printf("opd: %s\n", t_opd);

		//test printf("Operand copiado com sucesso.\n");


		//test printf("Hora de verificar que tipo de Operand é esse!\n");

		if(t_opd[0] == '$') {
			//test printf("Parece que ele é um registrador!\n");

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
				//test printf("Vou calcular o numero do registrador.\n");
				int num = 0;
				for(int j = 1; j<l; j++){
					//test printf("somando: %c\n",t_opd[j]);
					if(!isdigit(t_opd[j])){ //evitar algo do tipo "$1a"
						*errptr = &s[k];
						set_error_msg("Invalid register number.");
						return 0;
					}
					num = num*10 + (t_opd[j] - '0'); //se fosse float, essa operação teria menos erro ;D
				}
				//test printf("Calculei: %d\n",num);
				opds[z] = operand_create_register((unsigned char)num);
			}
			else {
				*errptr = &s[k];
				set_error_msg("Wrong operand type: Expected %hhx, got REGISTER.",op->opd_types[z]);
				return 0;
			}
		}

		else if(isdigit(t_opd[0]) || ( t_opd[0] == '#' && l>1 ) ){ // então deve ser um numero, né?
			//test printf("Parece que ele é um numero!\n");
			if(op->opd_types[z] & NUMBER_TYPE){
				//test printf("Vou calcular o numero.\n");
				int num = 0;
				int j = 0;
				if(t_opd[0] == '#') j = 1;
				for(; j<l; j++){
					//test printf("somando: %c\n",t_opd[j]);
					if(!isdigit(t_opd[j])){ //evitar algo do tipo "$1a"
						*errptr = &s[k];
						set_error_msg("Invalid register number.");
						return 0;
					}
					num = num*10 + (t_opd[j] - '0'); //se fosse float, essa operação teria menos erro ;D
				}
				//test printf("Calculei: %d\n",num);
				opds[z] = operand_create_number((octa)num);
			}
			else{
				*errptr = &s[k];
				set_error_msg("Wrong operand type: Expected %hhx, got NUMBER_TYPE.",op->opd_types[z]);
				return 0;
			}
		}


		else { //é uma string, fudeu
			//test printf("Ele deve ser uma string ou um label.\n");
			EntryData *test = stable_find(alias_table, t_opd);

			if(test == NULL){ // então é uma string
				int num = -1;

				if(l>1 && l<4 && t_opd[0] == 'r'){ //nenhum registrador especial
												  //com menos de 2 e mais de 4 chars
					if(l == 3 && strcmp(t_opd,"rSP")){
						num = 253;
					}
					else if(strcmp(t_opd,"rA")){
						num = 255;
					}
					else if(strcmp(t_opd,"rR")){
						num = 254;
					}
					else if(strcmp(t_opd,"rX")){
						num = 252;
					}
					else if(strcmp(t_opd,"rY")){
						num = 251;
					}
					else if(strcmp(t_opd,"rZ")){
						num = 250;
					}
				}

				//test printf("Parece que é uma string ou registrador de valor %d!\n",num);
				if((num == -1 && op->opd_types[z] & STRING) || (num != -1 &&  op->opd_types[z] & REGISTER)){
					if(num == -1)
						opds[z] = operand_create_string(t_opd);
					else
						opds[z] = operand_create_register((octa)num);
				}
				else {
					*errptr = &s[k];
					if(num == -1)
						set_error_msg("Wrong operand type: Expected %hhx, got STRING.",op->opd_types[z]);
					else
						set_error_msg("Wrong operand type: Expected %hhx, got REGISTER.",op->opd_types[z]);
					return 0;
				}
			}
			else{ // então é um label
				//test printf("Parece que é um label de tipo %x \n",test->opd->type);
				if(op->opd_types[z] & test->opd->type) { //verifica para o que a label aponta de verdade
					//cria novo operador caso a caso do label
					if(test->opd->type & REGISTER)
						opds[z] = operand_create_register(test->opd->value.num);
					else if(test->opd->type & NUMBER_TYPE){
						opds[z] = operand_create_number(test->opd->value.num);
					}
					else
						opds[z] = operand_create_label(t_opd);
				}
				else {
					*errptr = &s[k];
					set_error_msg("Wrong operand type: Expected %hhx, got LABEL.",op->opd_types[z]);
					return 0;
				}
			}
		}

		i++;
	}

	//test printf("\nAchei todos os operands.\n");

	//test printf("Vou criar a nova instrução.\n");
	Instruction *new = instr_create(label,op,opds);	//Cria a instruction nova
	//test printf("Vou fazer ela apontar para o começo da lista.\n");
	new->next = *instr;								//Faz ela apontar para a instrução anterior
	//test printf("Vou colocar ela no começo da lista.\n");
	*instr = new;									//Agora ela está no topo
	//test printf("Pronto! Tudo parece ter sido feito!\n");




	// A gente precisa dar frees aqui etc?


	return 1;
}

