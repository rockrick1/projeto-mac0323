		EXTERN	main
digitou	STR		"Voce digitou:\n"
main	SETW	$100,0

		PUSH	$100
		CALL	scan
		ADDU	$101,rA,0

		SETW	rX,2
		SETW	rY,10
		INT		#80

		SUBU	$0,$101,$100
		DIVU	$0,$0,2
		PUSH	$0
		PUSH 	$100
		CALL	printf

		SETW	rX,2
		SETW	rY,10
		INT		#80

		INT		0