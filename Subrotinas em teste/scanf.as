		EXTERN	scanf
p		IS		$0
c		IS		$1
t1		IS		$2
scanf	SUBU	p,rSP,16
		LDOU	p,p,0
		LDBU	c,p,0

words	SUBU	t1,c,33
		JNN		t1,start
		JZ		c,start
		ADDU	p,p,1
		LDB 	c,p,0
		JMP		words

start	ADDU	$99,p,0

worde	SUBU	t1,c,33
		JN		t1,end
		JZ		c,end
		ADDU	p,p,1
		LDBU	c,p,0
		JMP		worde

end		ADDU	rA,p,0
		RET		1