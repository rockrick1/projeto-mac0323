		EXTERN	main

t 			IS 		$0		*literalmente tudo
mem			IS		$1		*guarda alguns endereços de memoria quando necessario
col			IS		$49		*numero de colunas pra justificar
nwords		IS		$50		*numero de palavras por linha
current		IS		$51		*quantos espaços temos sobrando na linha
wstart		IS		$52		*marca o inicio da palavra atual
wend		IS		$53		*marca o fim da palavra atual
wcount		IS		$54		*"int i" para fazer while ( i < nwords)
scount		IS		$55		*conta os espacos por linha
twoline		IS 		$56		*adiciona mais um \n
tempcurr	IS		$57		*current temporario


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

		MUL		t,nwords,50	*copia ela lá para os 60000 da memoria
		SETW	mem,60000
		ADDU	mem,mem,t
		PUSH	mem
		PUSH	wend
		PUSH	wstart
		CALL	memcopy

		MUL		t,nwords,16		*guarda um ponteiro para o fim dela lá nos 58000
		SETW	mem,58000
		ADDU	mem,mem,t
		STW 	rA,mem,0

		SUBU 	t,wend,wstart	*tamanho da palavra
		JNP  	t,last			*se o tamanho for 0 ou negativo, o texto acabou
		SUBU	$3,col,t 		*vê se a palavra supera o col por si só
		JNP 	$3,ignora
		SUBU	tempcurr,current,t *subtrai o tamanho da palavra dos espaços que faltam
		JN		tempcurr,spaces 	  *deu certinho ou passou?
		SUBU	current,current,t
		SUBU	current,current,1 *adiciona pelo menos um espaço então

		LDBU	$3,wend,0	*Essa palavra termina em um \n?
		SETW	t,10
		CMP		$3,$3,t
		JZ		$3,parag

getcont	ADDU	nwords,nwords,1	  *aumenta quantas palavras temos na linha
		ADDU	wstart,wend,0	  *pega a proxima palavra depois da atual

		JMP 	get


ignora	JP      nwords,spaces	*se tem alguma palavra antes, imprime elas
		LDBU	$3,wend,0	*Essa palavra termina em um \n?
		SETW	t,10
		CMP		$3,$3,t
		JNZ		$3,igcont 		*Não, continua normalmente
		ADDU	$3,wend,1		*São dois \n seguidos?
		LDBU	$3,$3,0
		CMP		$3,$3,t
		JNZ		$3,igcont 		*Não são, continua normalmente
		SETW	twoline,1		*São, termina a linha aqui e imprima um \n
igcont		PUSH	wend			*imprime a palavra e passa para a proxima
		PUSH 	wstart
		CALL	printf
		ADDU	wstart,wend,0
		JMP		newline

parag	ADDU	$3,wend,1		*São dois \n seguidos?
		LDBU	$3,$3,0
		CMP		$3,$3,t

		JNZ		$3,getcont 		*Não são, continua normalmente

		SETW	twoline,1		*São, termina a linha aqui e imprima um \n
		ADDU	nwords,nwords,1
		ADDU	wstart,wend,0
		JMP		last			*Ultima linha do paragrafo

spaces	SUBU	t,nwords,scount		*Já adicionamos pelo menos um espaço por palavra?
		SUBU 	t,t,1
		JNP		t,remain

		MUL		t,scount,16			*Adiciona um espaço no fim da palavra
		SETW	mem,58000
		ADDU	t,mem,t
		LDWU	mem,t,0
		SETW	$3,32
		STB		$3,mem,0

		ADDU	mem,mem,1			*Atualiza Ponteiro
		STW 	mem,t,0

		ADDU 	scount,scount,1		*Colocamos o espaço em mais uma palavra
		JMP 	spaces

remain	SUBU 	scount,nwords,2
		ADDU 	current,current,1
justif	JZ		current,write
		MUL		t,scount,16			*Adiciona um espaço no fim da palavra
		SETW	mem,58000
		ADDU	t,mem,t
		LDWU	mem,t,0
		SETW	$3,32
		STB		$3,mem,0

		ADDU	mem,mem,1			*Atualiza Ponteiro
		STW 	mem,t,0

		SUBU 	scount,scount,1		*Colocamos o espaço em mais uma palavra
		SUBU	current,current,1	*Current fica menor
		JNN		scount,justif
		SUBU 	scount,nwords,2
		JMP		justif


write	MUL		t,wcount,16			*Pega o fim da palavra, empilha
		SETW	mem,58000
		ADDU	mem,mem,t
		LDWU	mem,mem,0
		PUSH	mem

		MUL		t,wcount,50		*Pega o começo da palavra, empilha
		SETW	mem,60000
		ADDU	mem,mem,t
		PUSH	mem

		CALL 	printf				*Imprime ela

		ADDU	wcount,wcount,1		*Imprimimos uma palavra
		SUBU	t,nwords,wcount

		JNP		t,newline			*Se imprimimos todas, nova linha
		JMP 	write

newline	SETW	rX,2				*coloca um \n
		SETW	rY,10
		INT		#80
		SETW	nwords,0			*reseta valores
		SETW	scount,0
		SETW	wcount,0
		ADDU	current,col,0

		JNP		twoline,get			*É um paragrafo?

		SETW	rX,2
		SETW	rY,10
		INT		#80
		SETW	twoline,0
		JMP		get

last	JNP		nwords,end 			*pula para o fim se não faltar nada
		SETW	t,1
		CMP		t,t,nwords			*Tem só mais uma palavra na linha?
		JZ		t,tlast

		MUL		t,wcount,16			*Empilha o fim da palavra
		SETW	mem,58000
		ADDU	mem,mem,t
		LDWU	mem,mem,0
		PUSH	mem

		MUL		t,wcount,50		*Empilha o começo da palavra
		SETW	mem,60000
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
		SETW	mem,58000
		ADDU	mem,mem,t
		LDWU	mem,mem,0
		PUSH	mem

		MUL		t,wcount,50
		SETW	mem,60000
		ADDU	mem,mem,t
		PUSH	mem

		CALL 	printf

		JP 		twoline,newline 	*Era a ultima linha do paragrafo, não do texto

end		SETW	rX,2
		SETW	rY,10
		INT		#80
		INT		0
