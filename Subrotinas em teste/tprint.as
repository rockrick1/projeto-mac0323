		EXTERN	main
hello	STR		"toma no cu"
main	GETA	$0,hello
		PUSH	$0
		CALL	print
		SETW	rX,2
		SETW	rY,10
		INT		#80
		INT		0