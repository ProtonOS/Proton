.intel_syntax noprefix
.global GlobalDescriptorTable_Update

GlobalDescriptorTable_Update:
	cli
    mov eax, [esp+4]
    lgdt [eax]
 
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    jmp 0x08:GlobalDescriptorTable_Flush
GlobalDescriptorTable_Flush:
    ret
