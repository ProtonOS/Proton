.intel_syntax noprefix
.global AtomicCompareExchange
.global AtomicIncrement

AtomicCompareExchange:
	mov edx, [esp + 4]
	mov eax, [esp + 8]
	mov ecx, [esp + 12]
	lock cmpxchgb byte ptr [edx], cl
	ret

AtomicIncrement:
	mov edx, [esp + 4]
	mov eax, 1
	lock xadd dword ptr [edx], eax
	ret
