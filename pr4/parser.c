#include "parser.h"
#include "optable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int parse(const char *s, SymbolTable alias_table, Instruction **instr, const char **errptr){

	int k = 0; //indice do começo da palavra
	int i = 0; //indice que percorre s / indice do fim da palavra
	int l = 0; //tamanho da palavra

	char *label = NULL;

	while(s[i]!=' ' || s[i]!='\n' || s[i]!=EOF) i++; //acha o fim da palabra

	if(s[i]=='\n' || s[i]=='*' || s[i]==EOF){} //Fazer o que aqui?

	l = i-k;



	char t_label[l]; //"test label"
	// usar strcpy aqui talvez?
	for(int j = 0; j<l; j++, k++) t_label[j] = s[k]; //copia a palavra

	//se null, não é um OP, portanto label? Outros casos?
	const Operator *op = optable_find(t_label);

	if(op == NULL){		//deve ser um label então
		label = t_label;

		//Provavelmente alguma coisa aqui para checar a alias_table

	}
	else{ //é um OP então, olha que legal

		// não sei se precisa de algo aqui TBH
		// seria para deixar um o op guardado,
		// mas acho que da para usar o op que a gente checa o null

		//talvez desalocar o tlabel ou algo assim?

	}

	if(label !=  NULL){ //então temos que procurar o op

		while(s[k]==' ') k++; //acha o inicio da palavra
		i = k;
		while(s[i]!=' ' || s[i]!='\n' || s[i]!='*' || s[i]!=EOF) i++; // acha o fim da palavra

		l = i-k;
		char t_op[l];

		for(int j = 0; j<l; j++, k++) t_op[j] = s[k];

		op = optable_find(t_op);

		if(op==NULL){} //Manda uns erros legais aqui

	}

	//Deem uma olhada no optable.c
	//Basicamente cada operator tem um vertor com o que ele precisa.
	//Se não precisar de argumentos, ele fala OP_NONE, assim:

	Operand *opds[3]; //prepara os operandos

	for(int z = 0; z < 3; z++){

		while(s[k]==' ') k++; //acha o inicio do argumento
		i = k;
		while(s[i]!=',' || s[i]!=' ' || s[i]!='\n' || s[i]!='*' || s[i]!=EOF) i++; // acha o fim do argumento

		l = i-k;

		if(op->opd_types[z] == OP_NONE && l != 0){ //ele não precisa mais de argumentos, mas achou um
			//Erro legal aqui
		}

		const char *t_opd[l];
		for(int j = 0; j < l; j++, k++) t_opd[j] = s[k];

		int num = 0; //valor legal do operando ( caso necessario )

		if(t_opd[0]=='$'){
			if(op->opd_types[z]==REGISTER){
				if(l<2){
					//Erro legal aqui ( se l<2, quer dizer que ele só mandou '$' de argumento )
				}
				opds[z] = operand_create_register((unsigned char)strtol(t_opd[1],t_opd[l-1],10));
			}
			else{
				//Erro legal aqui
			}
		}


		else if(isdigit(t_opd[0])){ // então deve ser um numero, né?
			if(op->opd_types[z] == NUMBER_TYPE){
				opds[z] = operand_create_number(strtoll(t_opd[1],t_opd[l-1],10));
			}
			else{
				//Erro legal aqui
			}
		}


		else{ //é uma string, fudeu
			EntryData *test = stable_find(alias_table,t_opd);

			if(test == NULL){ // então é uma string
				if(op->opd_types[z] == STRING){
					opds[z] = operand_create_string(t_opd);
				}
				else{
				//Erro legal aqui
				}
			}
			else{ // então é um label
				if(op->opd_types[z] == LABEL){
					opds[z] = operand_create_label(t_opd);
				}
				else{
				//Erro legal aqui
				}
			}
		}

	}

	Instruction *new = instr_create(label,op,opds);	//Cria a instruction nova
	new->next = *instr;								//Faz ela apontar para a instrução anterior
	instr = &new;									//Agora ela está no topo


	// A gente precisa dar frees aqui etc?

	return 1;
}

