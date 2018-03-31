		EXTERN	printf
s		IS		$0
f		IS		$1
printf	SUBU	s,rSP,16
		SUBU	f,rSP,24
		LDOU	s,s,0
		LDOU	f,f,0
		SETW	rX,2
		SUBU	f,f,s
		SUBU	f,f,1
write	LDB		rY,s,0
		JN		f,end
		INT		#80
		ADDU	s,s,1
		SUB 	f,f,1
		JMP		write
end		RET		2