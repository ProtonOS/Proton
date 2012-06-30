.intel_syntax noprefix
.global gStack
.global gStackSize
.global Boot
.global Halt
.global _exit
.global kill

.set ALIGN, 1<<0
.set MMAPS, 1<<1
.set FLAGS, ALIGN | MMAPS
.set MAGIC, 0x1BADB002
.set CHECK, -(MAGIC + FLAGS)
.set gStackSize, 0x100000

.align 4
.long MAGIC
.long FLAGS
.long CHECK

.comm gStack, gStackSize, 32

Boot:
    mov  gStack + gStackSize, esp
    push ebx
    push eax

    call Main
	cli
_exit:
kill:
Halt:
    hlt
    jmp Halt
