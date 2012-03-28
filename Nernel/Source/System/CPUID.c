#include <Common.h>
#include <System/CPUID.h>

extern uint32_t CPUID_GetEAX(uint32_t pFunction);
extern uint32_t CPUID_GetEBX(uint32_t pFunction);
extern uint32_t CPUID_GetECX(uint32_t pFunction);
extern uint32_t CPUID_GetEDX(uint32_t pFunction);

#define CPUID__Function__LargestStandardFunction							0
#define CPUID__Function__ProcessorVendor									0
#define CPUID__Function__Family_Model_Stepping								1
#define CPUID__Function__LocalApicID_LogicalProcessorCount_CLFlush_Brand	1
#define CPUID__Function__Features_1											1
#define CPUID__Function__Features_2											1

#define CPUID__ProcessorVendor__MaxLength									12

uint32_t gCPUID_LargestStandardFunction = 0;
char gCPUID_ProcessorVendor[CPUID__ProcessorVendor__MaxLength + 1];
uint8_t gCPUID_ProcessorFamily = 0;
uint8_t gCPUID_ProcessorModel = 0;
uint8_t gCPUID_ProcessorStepping = 0;
uint8_t gCPUID_LocalAPICID = 0;
uint8_t gCPUID_LogicalProcessorCount = 0;
uint8_t gCPUID_CacheLineFlushSize = 0;
uint8_t gCPUID_BrandID = 0;
uint32_t gCPUID_Features1 = 0;
uint32_t gCPUID_Features2 = 0;


void CPUID_Startup()
{
	uint32_t eax = CPUID_GetEAX(CPUID__Function__LargestStandardFunction);
	uint32_t ebx = CPUID_GetEBX(CPUID__Function__ProcessorVendor);
	uint32_t ecx = CPUID_GetECX(CPUID__Function__ProcessorVendor);
	uint32_t edx = CPUID_GetEDX(CPUID__Function__ProcessorVendor);
	gCPUID_LargestStandardFunction = eax;
	gCPUID_ProcessorVendor[0] = (ebx >> 0) & 0xFF;
	gCPUID_ProcessorVendor[1] = (ebx >> 8) & 0xFF;
	gCPUID_ProcessorVendor[2] = (ebx >> 16) & 0xFF;
	gCPUID_ProcessorVendor[3] = (ebx >> 24) & 0xFF;
	gCPUID_ProcessorVendor[4] = (edx >> 0) & 0xFF;
	gCPUID_ProcessorVendor[5] = (edx >> 8) & 0xFF;
	gCPUID_ProcessorVendor[6] = (edx >> 16) & 0xFF;
	gCPUID_ProcessorVendor[7] = (edx >> 24) & 0xFF;
	gCPUID_ProcessorVendor[8] = (ecx >> 0) & 0xFF;
	gCPUID_ProcessorVendor[9] = (ecx >> 8) & 0xFF;
	gCPUID_ProcessorVendor[10] = (ecx >> 16) & 0xFF;
	gCPUID_ProcessorVendor[11] = (ecx >> 24) & 0xFF;
	gCPUID_ProcessorVendor[12] = 0;

	eax = CPUID_GetEAX(CPUID__Function__Family_Model_Stepping);
	ebx = CPUID_GetEBX(CPUID__Function__LocalApicID_LogicalProcessorCount_CLFlush_Brand);
	ecx = CPUID_GetECX(CPUID__Function__Features_1);
	edx = CPUID_GetEDX(CPUID__Function__Features_2);

	bool_t extendedReserved = ((eax >> 8) & 0xF) < 0xF;
	gCPUID_ProcessorFamily = (eax >> 8) & 0xF;
	if (!extendedReserved)
	{
		gCPUID_ProcessorFamily += (eax >> 20) & 0xFF;
	}
	gCPUID_ProcessorModel = (eax >> 4) & 0xF;
	if (!extendedReserved)
	{
		gCPUID_ProcessorModel |= ((eax >> 16) & 0xF) << 4;
	}
	gCPUID_ProcessorStepping = (eax >> 0) & 0xF;
	gCPUID_LocalAPICID = (ebx >> 24) & 0xFF;
	gCPUID_LogicalProcessorCount = 1;
	if ((edx & (1 << 28)) != 0)
	{
		gCPUID_LogicalProcessorCount = (ebx >> 16) & 0xFF;
	}
	gCPUID_CacheLineFlushSize = (ebx >> 8) & 0xFF;
	gCPUID_BrandID = (ebx >> 0) & 0xFF;
	gCPUID_Features1 = ecx;
	gCPUID_Features2 = edx;


	Log_WriteLine(LOGLEVEL__Information, "CPU Supports: Largest Standard Function = %u", (unsigned int)gCPUID_LargestStandardFunction);
	Log_WriteLine(LOGLEVEL__Information, "Processor Vendor: %s", gCPUID_ProcessorVendor);
	Log_WriteLine(LOGLEVEL__Information, "Processor Family: %u, Model: %u, Stepping: %u", (unsigned int)gCPUID_ProcessorFamily, (unsigned int)gCPUID_ProcessorModel, (unsigned int)gCPUID_ProcessorStepping);
	Log_WriteLine(LOGLEVEL__Information, "Local APIC ID: %u, Logical Processor Count: %u, Cache Flush Size: %u", (unsigned int)gCPUID_LocalAPICID, (unsigned int)gCPUID_LogicalProcessorCount, (unsigned int)gCPUID_CacheLineFlushSize);
	Log_WriteLine(LOGLEVEL__Information, "Brand ID: %u, Features1: 0x%x, Features2: 0x%x", (unsigned int)gCPUID_BrandID, (unsigned int)gCPUID_Features1, (unsigned int)gCPUID_Features2);
}

void CPUID_Shutdown()
{
}

