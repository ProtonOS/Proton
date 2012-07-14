.intel_syntax noprefix
.global PICStartInterrupts
.global PICStopInterrupts

PICStartInterrupts:
	sti
	ret

PICStopInterrupts:
	cli
	ret
