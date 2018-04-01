		EXTERN	main

t 		IS 		$0		*literalmente tudo
mem		IS		$1		
col		IS		$49		*numero de colunas pra justificar
nwords	IS		$50		*numero de palavras por linha
current	IS		$51
wstart	IS		$52
wend	IS		$53
wcount	IS		$54
scount	IS		$55		*conta os espacos por linha

main	SUBU	col,rSP,16
		LDOU	col,col,0

		PUSH	col
		CALL	toint
		ADDU	col,rA,0

		SETW	$100,0
		PUSH	$100
		CALL	scan

		ADDU	wstart,$100,0
		ADDU	current,col,0

get		PUSH	wstart
		CALL	scanf

		ADDU	wstart,$99,0
		ADDU	wend,rA,0

		MUL		t,nwords,100
		SETW	mem,50000
		ADDU	mem,mem,t
		PUSH	mem
		PUSH	wend
		PUSH	wstart
		CALL	memcopy

		MUL		t,nwords,16
		SETW	mem,40000
		ADDU	mem,mem,t
		STW 	rA,mem,0

		SUBU 	t,wend,wstart
		JNP  	t,last
		SUBU	$3,col,t
		JNP 	$3,ignora
		SUBU	current,current,t
		JN		current,spaces 
		SUBU	current,current,1
		JN		current,spaces

		ADDU	nwords,nwords,1
		ADDU	wstart,wend,0

		JMP 	get

ignora	JP      nwords,spaces
		PUSH	wend
		PUSH 	wstart
		CALL	printf
		ADDU	wstart,wend,0
		JMP		newline
		
spaces	SUBU	t,nwords,scount		*atualmente, isso começa como 0
		SUBU 	t,t,1
		JNP		t,remain

		MUL		t,scount,16
		SETW	mem,40000
		ADDU	t,mem,t
		LDWU	mem,t,0
		SETW	$3,32
		STB		$3,mem,0

		ADDU	mem,mem,1
		STW 	mem,t,0
		
		ADDU 	scount,scount,1
		JMP 	spaces

remain	JMP 	write

write	MUL		t,wcount,16
		SETW	mem,40000
		ADDU	mem,mem,t
		LDWU	mem,mem,0
		PUSH	mem

		MUL		t,wcount,100
		SETW	mem,50000
		ADDU	mem,mem,t
		PUSH	mem

		CALL 	printf

		ADDU	wcount,wcount,1
		SUBU	t,nwords,wcount

		JNP		t,newline
		JMP 	write

clear	JNP		wcount,newline

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

newline	SETW	rX,2
		SETW	rY,10
		INT		#80
		SETW	nwords,0
		SETW	scount,0
		SETW	wcount,0
		ADDU	current,col,0
		JMP		get

last	JNP		nwords,end
		SETW	t,1
		CMP		t,t,nwords
		JZ		t,tlast

		MUL		t,wcount,16
		SETW	mem,40000
		ADDU	mem,mem,t
		LDWU	mem,mem,0
		PUSH	mem

		MUL		t,wcount,100
		SETW	mem,50000
		ADDU	mem,mem,t
		PUSH	mem

		CALL 	printf

		SETW	rX,2
		SETW	rY,32
		INT		#80

		ADDU	wcount,wcount,1
		SUBU	nwords,nwords,1

		JMP		last

tlast	MUL		t,wcount,16
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