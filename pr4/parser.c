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

	char *label = NULL;

	while(s[i]!=' ' || s[i]!='\n' || s[i]=='*'|| s[i]!=EOF) i++; //acha o fim da palabra

	if(s[i]=='\n' || s[i]=='*' || s[i]==EOF){
		printf("Achei uma linha sem codigo, vou voltar.\n");
		return 1; 	//acho que nesse caso seria um paragrafo, 
					//uma linha vazia, só um comentario, então a gente ignora?
	} 

	l = i-k;

	printf("Preparando para copiar a palavra com %d chars.\n",l);
	char t_label[l]; //"test label"					 // usar strcpy aqui talvez?
	for(int j = 0; j<l; j++, k++) t_label[j] = &s[k]; //copia a palavra
	printf("Palavra copiada com sucesso.\n");


	//se null, não é um OP, portanto label? Outros casos? 
	//E se o cara tiver escrito o comando errado?
	//Acho que poderiamos contar quantas palavras temos na linha,
	//se tiver 3, temos um label, caso contrario não.
	const Operator *op = optable_find(t_label);

	if(op == NULL){		//deve ser um label então
		printf("Acredito que a palavra seja um label.\n");
		label = t_label;

		EntryData *test = stable_find(alias_table,label); //ve se já não colocaram essa label

		if(test == NULL){
			errptr = &s[k];
			set_error_msg("Using a label that was already set."); //Alguma mensagem melhor?

			return 0;
		}

	}
	else{ //é um OP então, olha que legal
		printf("Acredito que a palavra seja um operator.\n");

		// não sei se precisa de algo aqui TBH
		// seria para deixar um o op guardado,
		// mas acho que da para usar o op que a gente checa o null

		//talvez desalocar o tlabel ou algo assim?

	}

	if(label !=  NULL){ //então temos que procurar o op
		printf("\nVou começar a procurar o operator!\n");

		while(s[k]==' ') k++; //acha o inicio da palavra
		printf("Achei o começo da palavra.\n");
		i = k;
		while(s[i]!=' ' || s[i]!='\n' || s[i]!='*' || s[i]!=EOF) i++; // acha o fim da palavra
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
		char t_op[l];
		for(int j = 0; j<l; j++, k++) t_op[j] = &s[k];
		printf("Palavra copiada com sucesso.\n");

		op = optable_find(t_op);

		if(op==NULL){
			errptr = &s[k];						
			set_error_msg("Missing Operator."); //Basicamente se ele não char um op até aqui,
			return 0; 							//Assumimos que falta o op.
		} 

	}

	printf("\nIndo pegar os operands.\n");

	//Deem uma olhada no optable.c
	//Basicamente cada operator tem um vertor com o que ele precisa.
	//Se não precisar de argumentos, ele fala OP_NONE, assim:

	Operand *opds[3]; //prepara os operandos

	for(int z = 0; z < 3; z++){

		printf("Vou procurar o operand %d!\n",z);

		printf("Achei o inicio do operand.\n");
		while(s[k]==' ') k++; //acha o inicio do argumento
		i = k;
		while(s[i]!=',' || s[i]!=' ' || s[i]!='\n' || s[i]!='*' || s[i]!=EOF) i++; // acha o fim do argumento

		l = i-k;
		printf("Achei o fim do operand, de tamanho %d.\n",l);

		if(op->opd_types[z] == OP_NONE && l != 0){ //ele não precisa mais de argumentos, mas achou um
			errptr = &s[k];						
			set_error_msg("Wrong number of operands.");
			return 0;
		}

		printf("Vou começar a copiar esse operand.\n");
		const char *t_opd[l];
		for(int j = 0; j < l; j++, k++) t_opd[j] = &s[k];
		printf("Operand copiado com sucesso.\n");


		printf("Hora de verificar que tipo de Operand é esse!\n");
		if(t_opd[0]=='$'){
			printf("Parece que ele é um registrador!\n");
			if(op->opd_types[z]==REGISTER){
				if(l<2){ 							//Isso signigica que o cara só mandou um '$'
					errptr = &s[k];						
					set_error_msg("Missing register number.");
					return 0;			
				}
				opds[z] = operand_create_register((unsigned char)strtol(t_opd[1],t_opd[l-1],10));
			}
			else{
				errptr = &s[k];						
				set_error_msg("Wrong operand type: Expected %hhx, got REGISTER.",op->opd_types[z]);
				return 0;
			}
		}


		else if(isdigit(t_opd[0])){ // então deve ser um numero, né?
			printf("Parece que ele é um numero!\n");
			if(op->opd_types[z] == NUMBER_TYPE){
				opds[z] = operand_create_number(strtoll(t_opd[1],t_opd[l-1],10));
			}
			else{
				errptr = &s[k];						
				set_error_msg("Wrong operand type: Expected %hhx, got NUMBER_TYPE.",op->opd_types[z]);
				return 0;
			}
		}


		else{ //é uma string, fudeu
			printf("Ele deve ser uma string ou um label.\n");
			EntryData *test = stable_find(alias_table,t_opd);

			if(test == NULL){ // então é uma string
				printf("Parece que é uma string!\n");
				if(op->opd_types[z] == STRING){
					opds[z] = operand_create_string(t_opd);
				}
				else{
					errptr = &s[k];						
					set_error_msg("Wrong operand type: Expected %hhx, got STRING.",op->opd_types[z]);
					return 0;
				}
			}
			else{ // então é um label
				printf("Parece que é um label!\n");
				if(op->opd_types[z] == LABEL){
					opds[z] = operand_create_label(t_opd);
				}
				else{
					errptr = &s[k];						
					set_error_msg("Wrong operand type: Expected %hhx, got STRING.",op->opd_types[z]);
					return 0;
				}
			}
		}

	}

	printf("\nAchei todos os operands.\n");

	printf("Vou criar a nova instrução.\n");
	Instruction *new = instr_create(label,op,opds);	//Cria a instruction nova
	printf("Vou fazer ela apontar para o começo da lista.\n");
	new->next = *instr;								//Faz ela apontar para a instrução anterior
	printf("Vou colocar ela no começo da lista.\n");
	instr = &new;									//Agora ela está no topo
	printf("Pronto! Tudo parece ter sido feito!\n");

	// A gente precisa dar frees aqui etc?

	return 1;
}

