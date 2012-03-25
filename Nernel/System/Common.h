#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

extern void Halt() __attribute__((noreturn));
void Panic(const char* pMessage) __attribute__((noreturn));
