.intel_syntax noprefix
.global TSSUpdateRegister

TSSUpdateRegister:
	mov eax, [esp + 4]
	ltr ax
	ret
