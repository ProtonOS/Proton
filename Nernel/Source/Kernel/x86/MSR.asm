.intel_syntax noprefix
.global MSRRead
.global MSRWrite

MSRRead:
	mov ecx, [esp+4]
	rdmsr
    ret

MSRWrite:
	mov ecx, [esp+4]
	mov eax, [esp+8]
	xor edx, edx
	wrmsr
    ret
