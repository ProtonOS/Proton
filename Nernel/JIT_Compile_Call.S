JIT_Compile_Call:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp
	
	nop; nop
	
	movl	16(%ebp), %eax
	movl	%eax, (%esp)
	call	JIT_Trampoline_DoCall
	
	nop; nop
	
	movl	12(%ebp), %eax
	leave
	ret