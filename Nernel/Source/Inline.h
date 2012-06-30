#pragma once



// This makes it so VS doesn't complain about the attribute.
// GCC sees the attribute just fine.
#ifdef _WIN32

#define ALWAYS_INLINE

#else

#ifdef NEVER_INLINE
#define ALWAYS_INLINE
#else
#define ALWAYS_INLINE __attribute__((always_inline)) 
#endif

#endif 
