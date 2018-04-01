		EXTERN	main

t 		IS 		$0		*literalmente tudo
mem		IS		$1		*guarda alguns endereços de memoria quando necessario
col		IS		$49		*numero de colunas pra justificar
nwords	IS		$50		*numero de palavras por linha
current	IS		$51		*quantos espaços temos sobrando na linha
wstart	IS		$52		*marca o inicio da palavra atual
wend	IS		$53		*marca o fim da palavra atual
wcount	IS		$54		*"int i" para fazer while ( i < nwords)
scount	IS		$55		*conta os espacos por linha

main	SUBU	col,rSP,16	*pega argumento
		LDOU	col,col,0

		PUSH	col 		*transforma o argumento em int
		CALL	toint
		ADDU	col,rA,0

		SETW	$100,0		*le tudo na entrada padrão e guarda em 0
		PUSH	$100
		CALL	scan

		ADDU	wstart,$100,0	*primeira palavra começa no começo
		ADDU	current,col,0	*temos col espaços sobrando

get		PUSH	wstart			*pega a proxima palavra
		CALL	scanf

		ADDU	wstart,$99,0	*guarda o começo e fim dela
		ADDU	wend,rA,0

		MUL		t,nwords,100	*copia ela lá para os 50000 da memoria
		SETW	mem,50000
		ADDU	mem,mem,t
		PUSH	mem
		PUSH	wend
		PUSH	wstart
		CALL	memcopy

		MUL		t,nwords,16		*guarda um ponteiro para o fim dela lá nos 40000
		SETW	mem,40000
		ADDU	mem,mem,t
		STW 	rA,mem,0

		SUBU 	t,wend,wstart	*tamanho da palavra
		JNP  	t,last			*se o tamanho for 0 ou negativo, o texto acabou
		SUBU	$3,col,t 		*vê se a palavra supera o col por si só
		JNP 	$3,ignora
		SUBU	current,current,t *subtrai o tamanho da palavra dos espaços que faltam
		JN		current,spaces 	  *deu certinho ou passou?
		SUBU	current,current,1 *adiciona pelo menos um espaço então
		
		ADDU	nwords,nwords,1	  *aumenta quantas palavras temos na linha
		ADDU	wstart,wend,0	  *pega a proxima palavra depois da atual

		JMP 	get

ignora	JP      nwords,spaces	*se tem alguma palavra antes, imprime elas
		PUSH	wend			*imprime a palavra e passa para a proxima
		PUSH 	wstart
		CALL	printf
		ADDU	wstart,wend,0
		JMP		newline
		
spaces	SUBU	t,nwords,scount		*Já adicionamos pelo menos um espaço por palavra?
		SUBU 	t,t,1
		JNP		t,remain

		MUL		t,scount,16			*Adiciona um espaço no fim da palavra
		SETW	mem,40000
		ADDU	t,mem,t
		LDWU	mem,t,0
		SETW	$3,32
		STB		$3,mem,0

		ADDU	mem,mem,1			*Atualiza Ponteiro
		STW 	mem,t,0
		
		ADDU 	scount,scount,1		*Colocamos o espaço em mais uma palavra
		JMP 	spaces

remain	JMP 	write				*deveria colocar o resto dos espaços

write	MUL		t,wcount,16			*Pega o fim da palavra, empilha
		SETW	mem,40000
		ADDU	mem,mem,t
		LDWU	mem,mem,0
		PUSH	mem

		MUL		t,wcount,100		*Pega o começo da palavra, empilha
		SETW	mem,50000
		ADDU	mem,mem,t
		PUSH	mem

		CALL 	printf				*Imprime ela

		ADDU	wcount,wcount,1		*Imprimimos uma palavra
		SUBU	t,nwords,wcount

		JNP		t,newline			*Se imprimimos todas, nova linha
		JMP 	write

clear	JNP		wcount,newline		*nada, provavelmente pode ser tirado

		MUL		t,wcount,100
		SETW	mem,50000
		ADDU	mem,mem,t
		PUSH	mem

		SETW	mem,30100
		PUSH	mem

		SETW	mem,30000
		PUSH	mem

		CALL	memcopy

		SUBU	wcount,wcount,1

		JMP 	clear

newline	SETW	rX,2				*coloca um \n
		SETW	rY,10
		INT		#80
		SETW	nwords,0			*reseta valores
		SETW	scount,0
		SETW	wcount,0
		ADDU	current,col,0
		JMP		get

last	JNP		nwords,end 			*pula para o fim se não faltar nada
		SETW	t,1
		CMP		t,t,nwords			*Tem só mais uma palavra na linha?
		JZ		t,tlast

		MUL		t,wcount,16			*Empilha o fim da palavra
		SETW	mem,40000
		ADDU	mem,mem,t
		LDWU	mem,mem,0
		PUSH	mem

		MUL		t,wcount,100		*Empilha o começo da palavra
		SETW	mem,50000
		ADDU	mem,mem,t
		PUSH	mem

		CALL 	printf				*Imprime a palavra

		SETW	rX,2				*Coloca um espaço
		SETW	rY,32
		INT		#80

		ADDU	wcount,wcount,1		*Uma palavra a menos
		SUBU	nwords,nwords,1

		JMP		last

tlast	MUL		t,wcount,16			*A ultima palavra, não coloque um espaço no final
		SETW	mem,40000
		ADDU	mem,mem,t
		LDWU	mem,mem,0
		PUSH	mem

		MUL		t,wcount,100
		SETW	mem,50000
		ADDU	mem,mem,t
		PUSH	mem

		CALL 	printf

end		SETW	rX,2
		SETW	rY,10
		INT		#80
		INT		0