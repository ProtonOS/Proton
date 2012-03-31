.intel_syntax noprefix
.global IDT_Update
.global IDT_ISRDispatch
.global IDT_IRQDispatch

.global IDT_ThrowInterrupt80

.extern IDT_ISRHandler
.extern IDT_IRQHandler

IDT_Update:
	cli
    mov eax, [esp]+4 
    lidt [eax]      
    ret

IDT_ISRDispatch:
    pusha
    mov ax, ds
    push eax

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
 
    call IDT_ISRHandler
 
	pop eax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
 
	popa
    add esp, 8
    or dword ptr [esp + 8], 0x200
    iret
 
IDT_IRQDispatch:
    pusha
    mov ax, ds
    push eax
 
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
 
    call IDT_IRQHandler
 
    pop ebx
    mov ds, bx
    mov es, bx
    mov fs, bx
    mov gs, bx
 
    popa
    add esp, 8
    or dword ptr [esp + 8], 0x200
    iret

IDT_ThrowInterrupt80:
	int 0x80
	ret
