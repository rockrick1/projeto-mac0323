		EXTERN	main

col		IS		$49
nwords	IS		$50

main	SUBU	col,rSP,16
		LDOU	col,col,0

		PUSH	col
		CALL	toint
		ADDU	col,rA,0

		INT		#DB3131

		SETW	$100,0

end		SETW	rX,2
		SETW	rY,10
		INT		#80
		INT		0