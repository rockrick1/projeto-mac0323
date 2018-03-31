		EXTERN	main

t 		IS 		$0
mem		IS		$1
col		IS		$49
nwords	IS		$50
current	IS		$51
wstart	IS		$52
wend	IS		$53
wcount	IS		$54

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

		SUBU 	t,wend,wstart
		JNP  	t,end
		SUBU	current,current,t
		JNP		current,spaces

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

		ADDU	nwords,nwords,1
		ADDU	wstart,wend,0
		JMP 	get
		
spaces	JMP write

write	MUL		t,wcount,16
		SETW	mem,40000
		ADDU	mem,mem,t
		LDWU	mem,mem,0
		PUSH	mem

		MUL		t,wcount,100
		SETW	mem,50000
		ADDU	mem,mem,t
		PUSH	mem

		INT		#DB0101

		CALL 	printf

		SETW	rX,2
		SETW	rY,32
		INT		#80

		ADDU	wcount,wcount,1
		SUBU	t,nwords,wcount

		JNP		t,clear
		JMP 	write

clear	JNP		wcount,end
		
		SETW	nwords,0

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

		JMP clear

end		SETW	rX,2
		SETW	rY,10
		INT		#80
		INT		0