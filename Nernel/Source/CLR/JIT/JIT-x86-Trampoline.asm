CPU 686
BITS 32

extern JIT_CompileMethod
extern AppDomainRegistry_GetDomain
extern Panic

BadAssemblyIndex	db	"Assembly Index is too High!", 0
BadTypeIndex		db	"Type Index is too High!", 0
BadMethodIndex		db	"Method Index is too High!", 0

global JIT_Trampoline_DoCall
JIT_Trampoline_DoCall:
	mov		edx, [ebp + 12]
	mov		eax, [edx + 30]
	push	edx
	push	eax
	call	AppDomainRegistry_GetDomain
	pop		edx
	push	eax
	
	; Check Assembly Index
	mov		eax, [eax + 4]
	mov		ebx, [edx + 34]
	cmp		eax, ebx
	jae		Bad_AssemblyIndex
	; Assembly index is ok
	mov		ebx, [esp]
	mov		ebx, [ebx + 8]
	sal		eax, 2
	add		eax, ebx
	mov		eax, [eax]
	mov		[esp], eax
	; The assembly is now on the top of the stack.
	
	; Now check type index
	mov		eax, [eax + 28]
	mov		ebx, [edx + 38]
	cmp		eax, ebx
	jae		Bad_TypeIndex
	; Type index is ok
	mov		ebx, [esp]
	mov		ebx, [ebx + 32]
	sal		eax, 2
	add		eax, ebx
	mov		eax, [eax]
	mov		[esp], eax
	; The type is now on the top of the stack.
	
	; Now check method index
	mov		eax, [eax + 17]
	mov		ebx, [ebp + 8]
	cmp		eax, ebx
	jae		Bad_MethodIndex
	; Method index is ok
	mov		eax, [esp]
	mov		eax, [eax + 32]
	sal		ebx, 2
	add		ebx, eax
	mov		ebx, [ebx]
	mov		[esp], ebx
	; The method is now on the top of the stack.
	
	mov		eax, [ebx + 4]
	test	eax, eax
	jne		MethodReady
	
	; Push the pointer to the 
	; method, and call for it
	; to be compiled.
	push	ebx
	call	JIT_CompileMethod
	
	; The method is ready,
	; and the original copy
	; of the pointer is
	; still on the top of the
	; stack, so we now call
	; the compiled method.
MethodReady:
	pop		eax
	mov		eax, [eax + 4]
	call	eax
	ret
	
	
Bad_AssemblyIndex:
	push	BadAssemblyIndex
	jmp		Bad_Common

Bad_TypeIndex:
	push	BadTypeIndex
	jmp		Bad_Common

Bad_MethodIndex:
	push	BadMethodIndex

Bad_Common:
	call	Panic
	
	