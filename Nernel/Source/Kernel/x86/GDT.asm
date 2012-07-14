.intel_syntax noprefix
.global GDTUpdateRegister
.extern gGDTRegisterPointer

GDTUpdateRegister:
	cli
	mov eax, gGDTRegisterPointer
    lgdt [eax]
 
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    jmp 0x08:GDTFlush
GDTFlush:
    ret
