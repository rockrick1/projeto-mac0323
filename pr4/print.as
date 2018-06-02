s		IS		$0
		SUBU	s,rSP,16
		LDOU	s,s,0
		SETW	rX,2
write	LDB		rY,s,0
		JZ		rY,end
		INT		#80
		ADDU	s,s,1
		JMP		write
end		RET		1