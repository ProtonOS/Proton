.intel_syntax noprefix
.global Atomic_CompareExchange

Atomic_CompareExchange:
	mov edx, [esp + 4]
	mov eax, [esp + 8]
	mov ecx, [esp + 12]
	lock cmpxchgb byte ptr [edx], cl
	ret
