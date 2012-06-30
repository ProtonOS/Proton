.intel_syntax noprefix
.global TSSUpdate
.global TSSGetTaskRegister

TSSUpdate:
	mov eax, [esp + 4]
	ltr ax
	ret

TSSGetTaskRegister:
	str eax
	ret
