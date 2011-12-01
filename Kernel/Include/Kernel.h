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
#include <Core/FileSystemManager.h>
#include <Core/GDT.h>
#include <Core/IDT.h>

using namespace Core;
using namespace Core::Driver;

// SystemCalls

void Halt();
void Panic(const char* pMessage);

class Kernel
{
public:
    static bool Startup(uint32_t pMultiBootMagic, void* pMultiBootData);
    static void Shutdown();

private:
    static COMPortLogger* sCOMPortLogger;
    static Console* sConsole;
};