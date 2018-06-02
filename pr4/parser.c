#include "parser.h"
#include "optable.h"
#include "error.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int parse(const char *s, SymbolTable alias_table, Instruction **instr, const char **errptr){

	printf("\nEntrei no parse!\n");

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
		printf("Achei uma linha sem codigo, vou voltar.\n");
		return 1; 	//acho que nesse caso seria um paragrafo,
					//uma linha vazia, só um comentario, então a gente ignora?
	}

	l = i-k;

	printf("Preparando para copiar a palavra com %d chars.\n",l);
	char t_label[l + 1]; //"test label"
	strncpy(t_label, s + k, l);
	t_label[l] = '\0';
	// printf("palavra: %s\n", t_label);
	printf("Palavra copiada com sucesso.\n");

	// // remove a parte da label da linha, para mais facilidade na hora de
	// // fazer o parse no resto
	// int j;
	// for (j = i; isspace(s[j]); j++);
	// s = s + j;
	// printf("%s\n", s);

	// coloca i e k no inicio da proxima palavra
	while (isspace(s[i]) && s[i] != '\0') i++;
	k = i;


	char label[l + 1];

	//se null, não é um OP, portanto label? Outros casos?
	//E se o cara tiver escrito o comando errado?
	//Acho que poderiamos contar quantas palavras temos na linha,
	//se tiver 3, temos um label, caso contrario não.
	op = optable_find(t_label);

	if(op == NULL) {		//deve ser um label então
		printf("Acredito que a palavra seja um label.\n");
		strcpy(label, t_label);

		//ve se já não colocaram essa label
		printf("label: %s\n",label);
		EntryData *test = stable_find(alias_table,label);

		if(test != NULL) {
			*errptr = &s[k];
			set_error_msg("This label already exists");

			return 0;
		}

	}
	else { //é um OP então, olha que legal
		printf("Acredito que a palavra seja um operator.\n");

		// não sei se precisa de algo aqui TBH
		// seria para deixar um o op guardado,
		// mas acho que da para usar o op que a gente checa o null

		//talvez desalocar o tlabel ou algo assim?

	}

	if(label !=  NULL) { //então temos que procurar o op
		printf("\nVou começar a procurar o operator!\n");

		while(isspace(s[k])) k++; //acha o inicio da palavra
		printf("Achei o começo da palavra.\n");
		i = k;
		// acha o fim da palavra
		while(!isspace(s[i]) && s[i] != '\0') i++;
		printf("Achei o fim da palavra.\n");
		/*
		if(s[i]=='\n' || s[i]=='*' || s[i]==EOF){
			errptr = &s[k];							Acho que o teste abaixo já resolve esse caso,
			set_error_msg("Missing Operator."); 	mas achei melhor deixar ele aqui por enquanto.
			return 0; 								Vai que.
		}
		*/

		l = i-k;

		printf("Preparando para copiar a palavra com %d chars.\n",l);
		char t_op[l + 1];
		strncpy(t_op, s + k, l);
		t_op[l] = '\0';
		printf("operator: %s\n", t_op);
		printf("Palavra copiada com sucesso.\n");

		op = optable_find(t_op);

		if(op == NULL){
			*errptr = &s[k];
			set_error_msg("Invalid operator."); //Basicamente se ele não char um op até aqui,
			return 0; 							//Assumimos que falta o op.
		}

	}

	printf("\nIndo pegar os operands.\n");

	//Deem uma olhada no optable.c
	//Basicamente cada operator tem um vertor com o que ele precisa.
	//Se não precisar de argumentos, ele fala OP_NONE, assim:

	// identifica quantos oerandos sao necessarios
	int operands = 3;
	if (op->opd_types[2] == OP_NONE) operands = 2;
	if (op->opd_types[1] == OP_NONE) operands = 1;
	if (op->opd_types[0] == OP_NONE) operands = 0;

	for(int z = 0; z < operands; z++){

		printf("\n\nVou procurar o operand %d!\n",z);

		printf("Achei o inicio do operand.\n");
		k = i;
		while(isspace(s[k])) k++; //acha o inicio do argumento
		i = k;
		// acha o fim do argumento
		while(!isspace(s[i]) && s[i] != ',' && s[i] != '\0' && s[i] != EOF) i++;

		l = i-k;
		printf("Achei o fim do operand, de tamanho %d.\n",l);

		if(op->opd_types[z] == OP_NONE && l != 0){ //ele não precisa mais de argumentos, mas achou um
			*errptr = &s[k];
			set_error_msg("Wrong number of operands.");
			return 0;
		}

		printf("Vou começar a copiar esse operand.\n");
		// strncpy chora com const char :japanese_goblin:
		const char *t_opd[l + 1];
		strncpy((char*)t_opd, s + k, l);
		t_opd[l] = '\0';
		printf("opd: %s\n", (char*)t_opd);
		// for(int j = 0; j < l; j++, k++) t_opd[j] = &s[k];
		printf("Operand copiado com sucesso.\n");


		printf("Hora de verificar que tipo de Operand é esse!\n");
		// fiz mais um opd pq const char é chato pra porra
		char tt_opd[l + 1];
		strcpy(tt_opd, (char*)t_opd);
		if(tt_opd[0] == '$') {
			printf("Parece que ele é um registrador!\n");

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
				opds[z] = operand_create_register(
					(unsigned char)strtol(t_opd[1], (char**)t_opd[l-1], 10));
			}
			else {
				*errptr = &s[k];
				set_error_msg("Wrong operand type: Expected %hhx, got REGISTER.",op->opd_types[z]);
				return 0;
			}
		}


		else if(isdigit(t_opd[0])){ // então deve ser um numero, né?
			printf("Parece que ele é um numero!\n");
			if(op->opd_types[z] == NUMBER_TYPE){
				opds[z] = operand_create_number(strtoll(t_opd[1], (char**)t_opd[l-1], 10));
			}
			else{
				*errptr = &s[k];
				set_error_msg("Wrong operand type: Expected %hhx, got NUMBER_TYPE.",op->opd_types[z]);
				return 0;
			}
		}


		else { //é uma string, fudeu
			printf("Ele deve ser uma string ou um label.\n");
			EntryData *test = stable_find(alias_table, (char*)t_opd);

			if(test == NULL){ // então é uma string
				printf("Parece que é uma string!\n");
				if(op->opd_types[z] == STRING){
					opds[z] = operand_create_string((char*)t_opd);
				}
				else {
					*errptr = &s[k];
					set_error_msg("Wrong operand type: Expected %hhx, got STRING.",op->opd_types[z]);
					return 0;
				}
			}
			else{ // então é um label
				printf("Parece que é um label!\n");
				if(op->opd_types[z] == LABEL) {
					opds[z] = operand_create_label((char*)t_opd);
				}
				else {
					*errptr = &s[k];
					set_error_msg("Wrong operand type: Expected %hhx, got STRING.",op->opd_types[z]);
					return 0;
				}
			}
		}

		i++;
	}

	printf("\nAchei todos os operands.\n");

	printf("Vou criar a nova instrução.\n");
	Instruction *new = instr_create(label,op,opds);	//Cria a instruction nova
	printf("Vou fazer ela apontar para o começo da lista.\n");
	new->next = *instr;								//Faz ela apontar para a instrução anterior
	printf("Vou colocar ela no começo da lista.\n");
	*instr = new;									//Agora ela está no topo
	printf("Pronto! Tudo parece ter sido feito!\n");


	// A gente precisa dar frees aqui etc?


	return 1;
}

