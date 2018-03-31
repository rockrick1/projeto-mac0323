		EXTERN	main

col		IS		$49
nwords	IS		$50

main	SUBU	$0,rSP,16
		LDWU	col,$0,0

		INT		#DB3032

		SETW	$100,0

end		SETW	rX,2
		SETW	rY,10
		INT		#80
		INT		0