
JIT_Trampoline_DoCall:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp
	
	pusha

	movl	8(%ebp), %eax
	movl	4(%eax), %eax
	testl	%eax, %eax
	jne	.L397
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	JIT_CompileMethod

	popa

	movl	8(%ebp), %eax
	movl	4(%eax), %eax
	call	*%eax
	leave
	ret