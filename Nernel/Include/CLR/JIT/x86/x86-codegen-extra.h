/*
 * The code in this file is under the same license as the 
 * rest of Proton.
 *
 *
 */

#pragma once

/*
 * This writes the byte in AL
 * to the IO port in DX
 */
#define x86_out_byte(instr) \
	do { \
		*(code)++ = (unsigned char)0xEE; \
	} while(0);

/*
 * This writes the word/dword in AX/EAX
 * to the IO port in DX
 */
#define x86_out_word(instr) \
	do { \
		*(code)++ = (unsigned char)0xEF; \
	} while(0);


#define x86_fdecstp(inst) do { x86_codegen_pre(&(inst), 2); *(inst)++ = (unsigned char)0xd9; *(inst)++ = (unsigned char)0xf6; } while (0)

#define x86_adjust_stack(inst, size) if (size < 0) { x86_alu_reg_imm(inst, X86_SUB, X86_ESP, -(size)); } else { x86_alu_reg_imm(inst, X86_ADD, X86_ESP, size); }


#define x86_cvttss2si(inst,dreg,reg)	\
	do {	\
		x86_codegen_pre(&(inst), 4); \
		*(inst)++ = (unsigned char)0xf3;	\
		*(inst)++ = (unsigned char)0x0f;	\
		*(inst)++ = (unsigned char)0x2c;	\
		x86_reg_emit ((inst), (dreg), (reg));	\
	} while (0)


