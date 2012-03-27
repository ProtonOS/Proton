#pragma once

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "Log.h"

extern void Halt() __attribute__((noreturn));
void Panic(const char* pMessage) __attribute__((noreturn));