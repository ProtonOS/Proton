.intel_syntax noprefix
.global IDTUpdate
.global IDTISRDispatch
.global IDTIRQDispatch

IDTUpdate:
	cli
    mov eax, [esp]+4 
    lidt [eax]
	sti
    ret

IDTISRDispatch:
    pusha
    mov ax, ds
    push eax

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
 
    call IDTISRHandler
 
	pop eax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
 
	popa
    add esp, 8
    or dword ptr [esp + 8], 0x200
    iret
 
IDTIRQDispatch:
    pusha
    mov ax, ds
    push eax
 
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
 
    call IDTIRQHandler
 
    pop ebx
    mov ds, bx
    mov es, bx
    mov fs, bx
    mov gs, bx
 
    popa
    add esp, 8
    or dword ptr [esp + 8], 0x200
    iret
