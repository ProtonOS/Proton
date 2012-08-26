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

#define x86_adjust_stack(inst, size) if ((int)size > 0) { x86_alu_reg_imm(inst, X86_SUB, X86_ESP, size); } else { x86_alu_reg_imm(inst, X86_ADD, X86_ESP, -(size)); }


#define x86_cvttss2si(inst,dreg,reg)	\
	do {	\
		x86_codegen_pre(&(inst), 4); \
		*(inst)++ = (unsigned char)0xf3;	\
		*(inst)++ = (unsigned char)0x0f;	\
		*(inst)++ = (unsigned char)0x2c;	\
		x86_reg_emit ((inst), (dreg), (reg));	\
	} while (0)



#define x86_fisttp_membase(inst,basereg,disp,is_long)	\
	do {	\
		x86_codegen_pre(&(inst), 1 + kMaxMembaseEmitPadding); \
		if ((is_long)) {	\
			*(inst)++ = (unsigned char)0xdd;	\
			x86_membase_emit ((inst), 1, (basereg), (disp));	\
		} else {	\
			*(inst)++ = (unsigned char)0xdb;	\
			x86_membase_emit ((inst), 1, (basereg), (disp));	\
		}	\
	} while (0)


#define x86_ud2(inst)	\
	do {	\
		x86_codegen_pre(&(inst), 2); \
		*(inst)++ = (unsigned char)0x0f;	\
		*(inst)++ = (unsigned char)0x0b;	\
	} while (0)
	

#define x86_lahf(inst)	\
	do {	\
		x86_codegen_pre(&(inst), 1); \
		*(inst)++ = (unsigned char)0x9f;	\
	} while (0)


#define x86_jump_memindex(inst,basereg,disp,indexreg,shift)	\
	do {	\
		x86_codegen_pre(&(inst), 1 + kMaxMemindexEmitPadding); \
		*(inst)++ = (unsigned char)0xff;	\
		if ((basereg) == X86_NOBASEREG) {	\
			x86_address_byte ((inst), 0, 4, 4);	\
			x86_address_byte ((inst), (shift), (indexreg), 5);	\
			x86_imm_emit32 ((inst), (disp));	\
		} else if ((disp) == 0 && (basereg) != X86_EBP) {	\
			x86_address_byte ((inst), 0, 4, 4);	\
			x86_address_byte ((inst), (shift), (indexreg), (basereg));	\
		} else if (x86_is_imm8((disp))) {	\
			x86_address_byte ((inst), 1, 4, 4);	\
			x86_address_byte ((inst), (shift), (indexreg), (basereg));	\
			x86_imm_emit8 ((inst), (disp));	\
		} else {	\
			x86_address_byte ((inst), 2, 4, 4);	\
			x86_address_byte ((inst), (shift), (indexreg), (basereg));	\
			x86_imm_emit32 ((inst), (disp));	\
		}	\
	} while (0)


