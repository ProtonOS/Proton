.intel_syntax noprefix
.global CPUID_GetEAX
.global CPUID_GetEBX
.global CPUID_GetECX
.global CPUID_GetEDX

CPUID_GetEAX:
	mov eax, [esp+4]
	cpuid
    ret

CPUID_GetEBX:
	mov eax, [esp+4]
	cpuid
	mov eax, ebx
    ret

CPUID_GetECX:
	mov eax, [esp+4]
	cpuid
	mov eax, ecx
    ret

CPUID_GetEDX:
	mov eax, [esp+4]
	cpuid
	mov eax, edx
    ret
