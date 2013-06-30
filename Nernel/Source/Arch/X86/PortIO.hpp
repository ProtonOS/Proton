#pragma once

#ifdef _WIN32
#define PORTIO(bwl, bw, type)                               \
inline void out##bwl(int pPort,                             \
                            unsigned type pValue)           \
{                                                           \
}                                                           \
                                                            \
inline unsigned type in##bwl(int pPort)                     \
{                                                           \
	return 0;				                                \
}                                                           \
                                                            \
inline void out##bwl##_p(int pPort,                         \
                                unsigned type pValue)       \
{                                                           \
	out##bwl(pPort, pValue);                                \
}                                                           \
                                                            \
inline unsigned type in##bwl##_p(int pPort)                 \
{                                                           \
	return 0;											    \
}                                                           \
                                                            \
inline void outs##bwl(int pPort,                            \
                             const void *pAddress,          \
                             unsigned long pCount)          \
{                                                           \
}                                                           \
                                                            \
inline void ins##bwl(int pPort,                             \
                            void *pAddress,                 \
                            unsigned long pCount)           \
{                                                           \
}
#else

#define PORTIO(bwl, bw, type)                               \
inline void out##bwl(int pPort,                             \
                            unsigned type pValue)           \
{                                                           \
	__asm volatile("out" #bwl " %" #bw "0, %w1"             \
		     : : "a"(pValue), "Nd"(pPort));                 \
}                                                           \
                                                            \
inline unsigned type in##bwl(int pPort)                     \
{                                                           \
	unsigned type pValue;                                   \
	__asm volatile("in" #bwl " %w1, %" #bw "0"              \
		     : "=a"(pValue) : "Nd"(pPort));                 \
	return pValue;                                          \
}                                                           \
                                                            \
inline void out##bwl##_p(int pPort,                         \
                                unsigned type pValue)       \
{                                                           \
	out##bwl(pPort, pValue);                                \
}                                                           \
                                                            \
inline unsigned type in##bwl##_p(int pPort)                 \
{                                                           \
	unsigned type pValue = in##bwl(pPort);                  \
	return pValue;                                          \
}                                                           \
                                                            \
inline void outs##bwl(int pPort,                            \
                             const void *pAddress,          \
                             unsigned long pCount)          \
{                                                           \
	__asm volatile("rep; outs" #bwl                         \
		     : "+S"(pAddress), "+c"(pCount) : "d"(pPort));  \
}                                                           \
                                                            \
inline void ins##bwl(int pPort,                             \
                            void *pAddress,                 \
                            unsigned long pCount)           \
{                                                           \
	__asm volatile("rep; ins" #bwl                          \
		     : "+D"(pAddress), "+c"(pCount) : "d"(pPort));  \
}
#endif

PORTIO(b, b, char)
PORTIO(w, w, short)
PORTIO(l, , int)

#ifdef _WIN32
#define IOWAIT()
#else
#define IOWAIT()		__asm volatile( "outb %%al, $0x80" : : "a"(0) )
#endif

