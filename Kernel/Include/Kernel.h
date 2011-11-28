#pragma once

extern "C" {
#include <types.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
}

#include <iostream>

#include <Core/Config.h>
#include <Core/MultiBoot.h>
#include <Core/DeviceManager.h>

using namespace Core;

// SystemCalls

void Halt();
void Panic(const char * pMessage);
