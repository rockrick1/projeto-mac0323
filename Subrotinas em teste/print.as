		extern	print
s		is		$0
print	subu	s,rSP,16
		ldou	s,s,0
		setw	rX,2
write	ldb		ry,s,0
		jz		ry,end
		int		#80
		addu	s,s,1
		jmp		write
end		ret		1