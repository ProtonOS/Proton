.intel_syntax noprefix
.global IDTUpdateRegister
.extern gIDTRegisterPointer

IDTUpdateRegister:
	cli
	mov eax, gIDTRegisterPointer
    lidt [eax]
	sti
    ret
