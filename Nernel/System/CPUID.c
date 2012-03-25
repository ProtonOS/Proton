#include "Common.h"
#include "CPUID.h"

extern uint32_t CPUID_GetEAX(uint32_t pFunction);
extern uint32_t CPUID_GetEBX(uint32_t pFunction);
extern uint32_t CPUID_GetECX(uint32_t pFunction);
extern uint32_t CPUID_GetEDX(uint32_t pFunction);

#define CPUID_FUNCTION__LARGEST_STANDARD_FUNCTION_NUMBER						0
#define CPUID_FUNCTION__PROCESSOR_VENDOR										0
#define CPUID_FUNCTION__FAMILY_MODEL_STEPPING_IDENTIFIERS						1
#define CPUID_FUNCTION__LOCAL_APIC_ID_LOGICAL_PROCESSOR_COUNT_CLFLUSH_BRAND		1
#define CPUID_FUNCTION__FEATURE_IDENTIFIERS_1									1
#define CPUID_FUNCTION__FEATURE_IDENTIFIERS_2									1

#define CPUID_PROCESSORVENDOR_MAXLENGTH											12

uint32_t gCPUID_LargestStandardFunction = 0;
char gCPUID_ProcessorVendor[CPUID_PROCESSORVENDOR_MAXLENGTH + 1];
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
	uint32_t eax = CPUID_GetEAX(CPUID_FUNCTION__LARGEST_STANDARD_FUNCTION_NUMBER);
	uint32_t ebx = CPUID_GetEBX(CPUID_FUNCTION__PROCESSOR_VENDOR);
	uint32_t ecx = CPUID_GetECX(CPUID_FUNCTION__PROCESSOR_VENDOR);
	uint32_t edx = CPUID_GetEDX(CPUID_FUNCTION__PROCESSOR_VENDOR);
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

	eax = CPUID_GetEAX(CPUID_FUNCTION__FAMILY_MODEL_STEPPING_IDENTIFIERS);
	ebx = CPUID_GetEBX(CPUID_FUNCTION__LOCAL_APIC_ID_LOGICAL_PROCESSOR_COUNT_CLFLUSH_BRAND);
	ecx = CPUID_GetECX(CPUID_FUNCTION__FEATURE_IDENTIFIERS_1);
	edx = CPUID_GetEDX(CPUID_FUNCTION__FEATURE_IDENTIFIERS_2);

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
}

void CPUID_Shutdown()
{
}

