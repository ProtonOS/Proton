.intel_syntax noprefix
.global APIC_TimerExpired
.global APIC_TimerCycle
.extern gAPIC_BaseAddress

APIC_TimerExpired:
	mov dword ptr [gAPIC_BaseAddress + 0xB0], 0
    iret

APIC_TimerCycle:
	iret
