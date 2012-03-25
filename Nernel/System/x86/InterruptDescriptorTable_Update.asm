.intel_syntax noprefix
.global InterruptDescriptorTable_Update
.global InterruptDescriptorTable_ISRDispatch
.global InterruptDescriptorTable_IRQDispatch

.extern InterruptDescriptorTable_ISRHandler
.extern InterruptDescriptorTable_IRQHandler

InterruptDescriptorTable_Update:
	cli
    mov eax, [esp]+4 
    lidt [eax]      
    ret 

InterruptDescriptorTable_ISRDispatch:
    pusha
    mov ax, ds
    push eax
 
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
 
    call InterruptDescriptorTable_ISRHandler
 
	pop eax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
 
	popa
    add esp, 8
    sti
    iret
 
InterruptDescriptorTable_IRQDispatch:
    pusha
    mov ax, ds
    push eax
 
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
 
    call InterruptDescriptorTable_IRQHandler
 
    pop ebx
    mov ds, bx
    mov es, bx
    mov fs, bx
    mov gs, bx
 
    popa
    add esp, 8
    sti
    iret
