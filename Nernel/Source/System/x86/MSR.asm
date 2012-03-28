.intel_syntax noprefix
.global MSR_Read
.global MSR_Write

MSR_Read:
	mov ecx, [esp+4]
	rdmsr
    ret

MSR_Write:
	mov ecx, [esp+4]
	mov eax, [esp+8]
	xor edx, edx
	wrmsr
    ret
