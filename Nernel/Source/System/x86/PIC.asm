.intel_syntax noprefix
.global PIC_StartInterrupts
.global PIC_StopInterrupts

PIC_StartInterrupts:
	sti
	ret

PIC_StopInterrupts:
	cli
	ret
