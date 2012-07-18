.intel_syntax noprefix
.global GDTSwitchToUserMode
.extern gEnteredUserModeAddress

GDTSwitchToUserMode:
	mov ax, 0x23
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

	mov eax, esp
	push 0x23
	push eax
	pushf
	push 0x1B
	push gEnteredUserModeAddress
	iret
