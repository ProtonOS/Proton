#pragma once

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <unistd.h>

#include <System/Log.h>

#define PROTON_VERSION_STRING "1.0.0.0"

#ifdef _WIN32
#include <IntellisenseFixes.h>
__declspec(noreturn) extern void Halt();
__declspec(noreturn) void Panic(const char* pMessage);
#else
extern void Halt() __attribute__((noreturn));
void Panic(const char* pMessage) __attribute__((noreturn));
#endif