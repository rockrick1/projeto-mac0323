		EXTERN	main

main	SETW	$100,0
		PUSH	$100
		CALL	scan
		ADDU	$101,rA,0

		SETW	$102,10000
		SETW	$0,33
		STB		$0,$102,0

		SETW	rX,2
		SETW	rY,10
		INT		#80

		PUSH 	$100
		CALL	print

		SETW	rX,2
		SETW	rY,10
		INT		#80

		PUSH 	$102
		CALL	print

		SETW	rX,2
		SETW	rY,10
		INT		#80

		SETW	rX,2
		SETW	rY,10
		INT		#80

		PUSH	$102
		PUSH	$101
		PUSH	$100
		CALL	memcopy

		PUSH 	$100
		CALL	print

		SETW	rX,2
		SETW	rY,10
		INT		#80

		PUSH 	$102
		CALL	print

		SETW	rX,2
		SETW	rY,10
		INT		#80

		INT		0