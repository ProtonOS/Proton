.intel_syntax noprefix
.global Register_GetCodeSegment
.global Register_GetDataSegment
.global Register_GetStackSegment
.global Register_GetESP

Register_GetCodeSegment:
	mov eax, cs
	ret

Register_GetDataSegment:
	mov eax, ds
	ret

Register_GetStackSegment:
	mov eax, ss
	ret

Register_GetESP:
	mov eax, esp
	ret
