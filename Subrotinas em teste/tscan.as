		EXTERN	main
digitou	STR		"Voce digitou:\n"
main	SETW	$100,0
		PUSH	$100
		CALL	scan
		SETW	rX,2
		SETW	rY,10
		INT		#80
		GETA	$0,digitou
		PUSH	$0
		CALL	print
		PUSH 	$100
		CALL	print
		SETW	rX,2
		SETW	rY,10
		INT		#80
		INT		#DBffff
		INT		0