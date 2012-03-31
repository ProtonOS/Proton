.intel_syntax noprefix
.global GDT_Update
.global TSS_Update
.global TSS_GetTaskRegister
.global GDT_SwitchToUserMode
.extern gEnteredUserModeAddress

GDT_Update:
	cli
    mov eax, [esp+4]
    lgdt [eax]
 
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    jmp 0x08:GDT_Flush
GDT_Flush:
    ret

TSS_Update:
	mov eax, [esp + 4]
	ltr ax
	ret

TSS_GetTaskRegister:
	str eax
	ret

GDT_SwitchToUserMode:
	mov ax, 0x23
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

	mov eax, esp
	push 0x23
	push eax
	pushf
	or dword ptr [esp], 0x200
	push 0x1B
	push gEnteredUserModeAddress
	iret
