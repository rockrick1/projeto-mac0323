		EXTERN	main

col		IS		$49
nwords	IS		$50

main	SUBU	rX,rSP,16
		LDOU	rX,rX,0
		PUSH	rX
		CALL	print

		SETW	$100,0

end		SETW	rX,2
		SETW	rY,10
		INT		#80
		INT		0