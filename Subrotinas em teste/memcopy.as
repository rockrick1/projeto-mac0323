		EXTERN	memcopy

from	IS		$0
until	IS		$1
to		IS		$2
temp	IS		$3

memcopy	SUBU	from,rSP,16
		LDOU	from,from,0
		SUBU	until,rSP,24
		LDOU	until,until,0
		SUBU	to,rSP,32
		LDOU	to,to,0

		SUBU	until,until,from

copy	SUBU	until,until,1
		JN		until,end

		LDB		temp,from,0
		ADDU	from,from,1

		STB		temp,to,0
		ADDU	to,to,1

		JMP 	copy

end		ADDU	rA,to,0
		RET		3