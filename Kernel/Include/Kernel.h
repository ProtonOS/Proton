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
#include <Driver/PIC8259A.h>
#include <Driver/PIT8254.h>
#include <Driver/RTC146818A.h>

// SystemCalls

void Halt();
void Panic(const char* pMessage);

class Kernel
{
public:
    static bool Startup(uint32_t pMultiBootMagic, void* pMultiBootData);
    static void Shutdown();

private:
    static Core::COMPortLogger* sCOMPortLogger;
    static Core::Driver::Console* sConsole;
    static Driver::PIC8259A* sPIC;
    static Driver::PIT8254* sPIT;
    static Driver::RTC146818A* sRTC;
};