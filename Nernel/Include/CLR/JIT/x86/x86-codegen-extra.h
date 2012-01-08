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


