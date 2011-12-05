#pragma once

#define PortIO(bwl, bw, type)                              \
static inline void out##bwl(int pPort,                      \
                            unsigned type pValue)           \
{                                                           \
	__asm volatile("out" #bwl " %" #bw "0, %w1"             \
		     : : "a"(pValue), "Nd"(pPort));                 \
}                                                           \
                                                            \
static inline unsigned type in##bwl(int pPort)              \
{                                                           \
	unsigned type pValue;                                   \
	__asm volatile("in" #bwl " %w1, %" #bw "0"              \
		     : "=a"(pValue) : "Nd"(pPort));                 \
	return pValue;                                          \
}                                                           \
                                                            \
static inline void out##bwl##_p(int pPort,                  \
                                unsigned type pValue)       \
{                                                           \
	out##bwl(pPort, pValue);                                \
}                                                           \
                                                            \
static inline unsigned type in##bwl##_p(int pPort)          \
{                                                           \
	unsigned type pValue = in##bwl(pPort);                  \
	return pValue;                                          \
}                                                           \
                                                            \
static inline void outs##bwl(int pPort,                     \
                             const void *pAddress,          \
                             unsigned long pCount)          \
{                                                           \
	__asm volatile("rep; outs" #bwl                         \
		     : "+S"(pAddress), "+c"(pCount) : "d"(pPort));  \
}                                                           \
                                                            \
static inline void ins##bwl(int pPort,                      \
                            void *pAddress,                 \
                            unsigned long pCount)           \
{                                                           \
	__asm volatile("rep; ins" #bwl                          \
		     : "+D"(pAddress), "+c"(pCount) : "d"(pPort));  \
}

PortIO(b, b, char)
PortIO(w, w, short)
PortIO(l, , int)

#define IOWAIT()		__asm volatile( "outb %%al, $0x80" : : "a"(0) )
