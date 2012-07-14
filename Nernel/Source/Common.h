#pragma once

#include <cstdint>
#include <cstddef>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <algorithm>
#include <functional>
//#include <unistd.h>
//#include <System/Log.h>


extern "C" {
#ifdef _WIN32
#include <IntellisenseFixes.h>
#define __attribute__(x)
#endif
extern void Halt() __attribute__((noreturn));
void Panic(const char* pMessage) __attribute__((noreturn));
}