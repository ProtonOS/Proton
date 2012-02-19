CPU 686
BITS 32

extern AppDomainRegistry_GetDomain
extern Panic

BadAssemblyIndex	db	"Assembly Index is too High!", 0
BadTypeIndex		db	"Type Index is too High!", 0
BadMethodIndex		db	"Assembly Index is too High!", 0

global JIT_Trampoline_DoCall
JIT_Trampoline_DoCall:
	mov		eax, [ebp + 12]
	mov		eax, [eax + 30]
	push	eax
	call	AppDomainRegistry_GetDomain
	push	eax
	mov		eax, [eax + 4]
;	cmp		

	
Bad_AssemblyIndex:
	push	BadAssemblyIndex
	jmp		Bad_Common

Bad_TypeIndex:
	push	BadTypeIndex
	jmp		Bad_Common

Bad_MethodIndex:
	push	BadMethodIndex
	jmp		Bad_Common

Bad_Common:
	call	Panic
	
	