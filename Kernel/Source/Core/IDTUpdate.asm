.intel_syntax noprefix
.global IDTUpdate
.global IDTISRDispatch
.global IDTIRQDispatch

.extern IDTISRHandler
.extern IDTIRQHandler

IDTUpdate:
	cli
    mov eax, [esp]+4 
    lidt [eax]      
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
    sti
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
    sti
    iret
