#include "parser.h"
#include "optable.h"
#include <stdio.h>
#include <stdlib.h>

int parse(const char *s, SymbolTable alias_table, Instruction **instr, const char **errptr){

	int k = 0; //indice do começo da palavra
	int i = 0; //indice que percorre s / indice do fim da palavra
	int l = 0; //tamanho da palavra

	char *label = NULL;

	while(s[i]!=' ' || s[i]!='\n' || s[i]!=EOF) i++; //acha o fim da palabra

	if(s[i]=='\n' || s[i]=='*' || s[i]==EOF){} //Fazer o que aqui?

	l = i-k;



	char t_label[l]; //"test label"
	for(int j = 0; j<l; j++, k++) t_label[j] = s[k]; //copia a palavra

	Operator op = optable_find(t_label);	//se null, não é um OP, portanto label? Outros casos?

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
		while(s[i]!=' ' || s[i]!='\n' || s[i]!=EOF) i++; // acha o fim da palavra

		l = i-k;
		char t_op[l];

		for(int j = 0; j<l; j++, k++) t_op[j] = s[k];

		op = optable_find(t_op);

		if(op==NULL){} //Manda uns erros legais aqui

	}

	//Deem uma olhada no optable.c
	//Basicamente cada operator tem um vertor com o que ele precisa.
	//Se não precisar de argumentos, ele fala OP_NONE, assim:

	Operand opds[3]; //prepara os operandos

	for(int z = 0; z<3; z++){

		if(op->opd_types[z]==OP_NONE) break; //ele não precisa mais de argumentos

		while(s[k]==' ') k++; //acha o inicio do argumento
		i = k;
		while(s[i]!=',' || s[i]!=' ' || s[i]!='\n' || s[i]!=EOF) i++; // acha o fim do argumento

		l = i-k;

		char *t_opd[l];
		for(int j = 0; j<l; j++, k++) t_opd[j] = s[k];

		//opds[z] = operand_create o formato correto... Alguem sabe como fazer isso?
		// 			eu pensei que teria que dar algum switch case no opd_types of no t_opd

	}

	Instruction new = {label,op,opds};	//Cria a instruction nova
	new->next = instr;					//Faz ela apontar para a instrução anterior
	instr = new;						//Agora ela está no topo


	// A gente precisa dar frees aqui etc?

	return 1;
}

