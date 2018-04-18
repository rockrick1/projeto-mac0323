		EXTERN	scan
p		IS		$0
scan	SUBU	p,rSP,16
		LDOU	p,p,0
		SETW	rX,1
read	INT		#80
		JN		rA,end
		STBU	rA,p,0
		ADDU	p,p,1
		JMP		read
end		ADDU	rA,p,0
		RET		1