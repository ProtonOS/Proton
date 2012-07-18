.intel_syntax noprefix
.global SMPInit
.global SMPInitEnd
.extern SMPStarted
.extern gGDTRegisterPointer
.extern gSMPTempStackTop

.code16
SMPInit:
	/cli

	mov eax, 0xA000
	lgdt [eax]

	mov eax, cr0
	or eax, 0x01
	mov cr0, eax

	jmp 0x08:((SMPEnterProtectedMode - SMPInit) + 0x8000)

.code32
SMPEnterProtectedMode:
	lea ecx, [SMPEnteredProtectedMode]
	jmp ecx

SMPEnteredProtectedMode:
	mov eax, gGDTRegisterPointer
	lgdt [eax]

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

	jmp 0x08:SMPSetTempStack

SMPSetTempStack:
	mov esp, gSMPTempStackTop
	mov eax, 0x4000
	sub gSMPTempStackTop, eax
	call SMPStarted
	jmp $

SMPInitEnd:
	ret
