#pragma once

void CPUID_Startup();
void CPUID_Shutdown();

extern uint32_t gCPUID_LargestStandardFunction;
extern char gCPUID_ProcessorVendor[];
extern uint8_t gCPUID_ProcessorFamily;
extern uint8_t gCPUID_ProcessorModel;
extern uint8_t gCPUID_ProcessorStepping;
extern uint8_t gCPUID_LocalAPICID;
extern uint8_t gCPUID_LogicalProcessorCount;
extern uint8_t gCPUID_CacheLineFlushSize;
extern uint8_t gCPUID_BrandID;
extern uint32_t gCPUID_Features1;
extern uint32_t gCPUID_Features2;
