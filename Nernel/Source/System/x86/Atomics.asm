.intel_syntax noprefix
.global Atomic_CompareExchange
.global Atomic_Increment

Atomic_CompareExchange:
	mov edx, [esp + 4]
	mov eax, [esp + 8]
	mov ecx, [esp + 12]
	lock cmpxchgb byte ptr [edx], cl
	ret

Atomic_Increment:
	mov edx, [esp + 4]
	mov eax, 1
	lock xadd dword ptr [edx], eax
	ret
