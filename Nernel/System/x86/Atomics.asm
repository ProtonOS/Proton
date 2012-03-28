.intel_syntax noprefix
.global Atomic_CompareExchange

Atomic_CompareExchange:
	mov eax, [esp + 8]
	mov ecx, [esp + 12]
	lock cmpxchg byte ptr [esp + 4], ecx
	ret
