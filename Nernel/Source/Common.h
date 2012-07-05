#pragma once

#include <cstdint>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <algorithm>
//#include <unistd.h>
//#include <System/Log.h>


extern "C" {
#ifdef _WIN32
#include <IntellisenseFixes.h>
#define noreturn
#else
#define noreturn __attribute__((noreturn))
#endif
extern void Halt() noreturn;
void Panic(const char* pMessage) noreturn;
}