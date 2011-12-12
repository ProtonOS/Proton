#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <time.h>

#include <Core/Config.h>
#include <Core/Console.h>
#include <Core/GDT.h>
#include <Core/IDT.h>
#include <Core/MultiBoot.h>
#include <Core/PIC.h>
#include <Core/PIT.h>
#include <Core/RTC.h>
#include <Core/SerialWriter.h>
#include <Core/SystemClock.h>
#include <Runtime/CLIFile.h>
#include <Runtime/MetaData.h>
#include <Runtime/Type.h>
#include <Runtime/Heap.h>
#include <Runtime/Finalizer.h>

#include <CLR/CLIFile.h>

void Halt();
void Panic(const char* pMessage);

bool_t Nernel_Startup(uint32_t pMultiBootMagic, void* pMultiBootData);
void Nernel_Shutdown();
