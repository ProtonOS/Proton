.intel_syntax noprefix
.global Boot
.global Halt

.set MultiBootModuleAlignment, 1<<0
.set MultiBootMemoryMapping, 1<<1
.set MultiBootRequestedFlags, MultiBootModuleAlignment | MultiBootMemoryMapping
.set MultiBootMagicMarker, 0x1BADB002
.set MultiBootChecksum, -(MultiBootMagicMarker + MultiBootRequestedFlags)

.set RealModeInitAddress, 0x8000
.set GDTRegisterAddress, 0x9000
.set GDTDescriptorAddress, 0xA000
.set GDTDescriptorMax, 512

.code32

.align 4
.long MultiBootMagicMarker
.long MultiBootRequestedFlags
.long MultiBootChecksum

Boot:
	/ TODO: Confirm that word ptr is right here, we need gGDTRegister to point to GDTRegisterAddress
	mov  word ptr gGDTRegister, GDTRegisterAddress
	mov  word ptr gGDTDescriptors, GDTDescriptorAddress
	mov  dword ptr gGDTDescriptorMax, GDTDescriptorMax
	mov  gBootStackTop, esp
	push ebx
	push eax

	call Main
	cli
Halt:
	hlt
	jmp Halt

.code16
RealModeInit:
	mov eax, GDTRegisterAddress
	lgdt [eax]

	mov eax, cr0
	or eax, 0x01
	mov cr0, eax

	jmp 0x08:((ProtectedModeInit - RealModeInit) + RealModeInitAddress)

.code32
ProtectedModeInit:
	lea ecx, [ProtectedModeLoad]
	jmp ecx

ProtectedModeLoad:
	mov eax, GDTRegisterAddress
	lgdt [eax]

	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

	jmp 0x08:ProtectedModeBoot

ProtectedModeBoot:
	mov esp, gTempStackCursor
	mov eax, 0x1000
	sub gTempStackCursor, eax
	call ProtectedModeStartup
	jmp $

UserModeBoot:
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
	push UserModeStartup
	iret
