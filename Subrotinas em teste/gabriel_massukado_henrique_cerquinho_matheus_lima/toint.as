		EXTERN	toint

n		IS		$0
d		IS		$1
t 		IS		$2

toint	SUBU	n,rSP,16
		LDOU	n,n,0
		LDBU	d,n,0

		XOR		rA,rA,rA

num		SUBU	t,d,48
		JN		t,end
		SUBU	t,d,57
		JP		t,end

		MUL		rA,rA,10
		SUBU	d,d,48
		ADDU	rA,rA,d

		ADDU	n,n,1
		LDB 	d,n,0
		JMP		num

end		RET		1