		EXTERN	main

main	SETW	$100,0
		PUSH	$100
		CALL	scan

		SETW	rX,2
		SETW	rY,10
		INT		#80

		PUSH 	$100
		CALL	toint
		ADDU	$100,rA,0
		
		INT		#DB6464

		INT		0