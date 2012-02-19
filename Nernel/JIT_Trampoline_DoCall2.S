
JIT_Trampoline_DoCall2:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$56, %esp
	
 
 /Starting Method 
 

	movl	12(%ebp), %eax
	movl	30(%eax), %eax
	movl	%eax, -12(%ebp)
	
 
 /Got Domain Index 
 

	movl	12(%ebp), %eax
	movl	34(%eax), %eax
	movl	%eax, -16(%ebp)
	
 
 /Got Assembly Index 
 

	movl	12(%ebp), %eax
	movl	38(%eax), %eax
	movl	%eax, -20(%ebp)
	
 
 /Got Type Index 
 

	movl	8(%ebp), %eax
	movl	4(%eax), %eax
	movl	%eax, -24(%ebp)
	
 
 /Got Method Index 
 

	movl	-12(%ebp), %eax
	movl	%eax, (%esp)
	call	AppDomainRegistry_GetDomain
	movl	%eax, -28(%ebp)
	
 
 /Got Domain 
 

	movl	-28(%ebp), %eax
	movl	4(%eax), %eax
	cmpl	-16(%ebp), %eax
	ja	.L399
	
 
 /Checked Assembly Index 
 

	movl	$.LC12, (%esp)
	call	Panic
	
 
 /Finished Panicking 
	
 
 /Finished Checking Assembly Index 
 

	movl	-28(%ebp), %eax
	movl	8(%eax), %eax
	movl	-16(%ebp), %edx
	sall	$2, %edx
	addl	%edx, %eax
	movl	(%eax), %eax
	movl	%eax, -32(%ebp)
	
 
 /Got Assembly 
 

	movl	-32(%ebp), %eax
	movl	28(%eax), %eax
	cmpl	-20(%ebp), %eax
	ja	.L400
	
 
 /Checked Type Index 
 

	movl	$.LC13, (%esp)
	call	Panic
	
 
 /Finished Panicking 
	
 
 /Finished Checking Type Index 
 

	movl	-32(%ebp), %eax
	movl	32(%eax), %eax
	movl	-20(%ebp), %edx
	sall	$2, %edx
	addl	%edx, %eax
	movl	(%eax), %eax
	movl	%eax, -36(%ebp)
	
 
 /Got Type 
 

	movl	-36(%ebp), %eax
	movl	17(%eax), %eax
	cmpl	-24(%ebp), %eax
	ja	.L401
	
 
 /Checked Method Index 
 

	movl	$.LC14, (%esp)
	call	Panic
	
 
 /Finished Panicking 
	
 
 /Finished Checking Method Index 
 

	movl	-36(%ebp), %eax
	movl	21(%eax), %eax
	movl	-24(%ebp), %edx
	sall	$2, %edx
	addl	%edx, %eax
	movl	(%eax), %eax
	movl	%eax, -40(%ebp)
	
 
 /Got Method 
 

	movl	-40(%ebp), %eax
	movl	4(%eax), %eax
	testl	%eax, %eax
	jne	.L402
	
 
 /Checked if Method Was Compiled 
 

	movl	-40(%ebp), %eax
	movl	%eax, (%esp)
	call	JIT_CompileMethod
	
 
 /Finished Compiling Method 
	
 
 /Finished Checking if Method Was Compiled 
 

	movl	-40(%ebp), %eax
	movl	4(%eax), %eax
	call	*%eax
	
 
 /Finished Method 
 

	leave
	ret