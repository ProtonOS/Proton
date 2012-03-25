.intel_syntax noprefix
.global stack
.global SSIZE
.global Boot
.global Halt
.global StartInterrupts
.global StopInterrupts

.set ALIGN, 1<<0
.set MMAPS, 1<<1
.set FLAGS, ALIGN | MMAPS
.set MAGIC, 0x1BADB002
.set CHECK, -(MAGIC + FLAGS)
.set SSIZE, 0x100000

.align 4
.long MAGIC
.long FLAGS
.long CHECK

.comm stack, SSIZE, 32

Boot:
    mov  stack + SSIZE, esp
    push ebx
    push eax

    call Main
	cli

Halt:
    hlt
    jmp Halt

StartInterrupts:
	sti
	ret

StopInterrupts:
	cli
	ret
