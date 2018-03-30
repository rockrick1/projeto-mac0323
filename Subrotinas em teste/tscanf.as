		EXTERN	main
i 		IS		$11
main	SETW	$100,0
		PUSH	$100
		CALL	scan
		ADDU	$101,rA,0

		SETW	rX,2
		SETW	rY,10
		INT		#80

		SETW	rX,2
		SETW	rY,10
		INT		#80

		ADDU	$99,$100,0

for		PUSH 	$99
		CALL 	scanf
		SUBU 	i,rA,$99
		JNP  	i,end
		SUBU 	$0,rA,$99
		PUSH 	$0
		PUSH 	$99
		CALL 	printf
		SETW	rX,2
		SETW	rY,10
		INT		#80
		ADDU 	$99,rA,0
		JMP	 	for



end		SETW	rX,2
		SETW	rY,10
		INT		#80
		INT		0