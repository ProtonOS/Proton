.intel_syntax noprefix
.global Boot

.set ALIGN, 1<<0
.set MMAPS, 1<<1
.set FLAGS, ALIGN | MMAPS
.set MAGIC, 0x1BADB002
.set CHECK, -(MAGIC + FLAGS)

.align 4
.long MAGIC
.long FLAGS
.long CHECK

.set SIZE, 0x4000
.comm stack, SIZE, 32

Boot:
    mov  stack + SIZE, esp
    push ebx
    push eax

    call Kernel
    cli
Hang:
    hlt
    jmp Hang
