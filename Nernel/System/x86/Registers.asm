.intel_syntax noprefix
.global Register_GetCodeSegment
.global Register_GetDataSegment
.global Register_GetStackSegment

Register_GetCodeSegment:
	mov eax, cs
	ret

Register_GetDataSegment:
	mov eax, ds
	ret

Register_GetStackSegment:
	mov eax, ss
	ret
