#pragma once

typedef struct _SMP_FPS SMP_FPS;
typedef struct _SMP_ConfigHeader SMP_ConfigHeader;
typedef struct _SMP_Processor SMP_Processor;
typedef struct _SMP_Bus SMP_Bus;
typedef struct _SMP_IOAPIC SMP_IOAPIC;
typedef struct _SMP_IOInterruptAssignment SMP_IOInterruptAssignment;
typedef struct _SMP_LocalInterruptAssignment SMP_LocalInterruptAssignment;

#include <System/APIC.h>


struct _SMP_FPS
{
	char Signature[4];
	uint32_t ConfigurationBaseAddress;
	uint8_t Length;
	uint8_t SpecificationRevision;
	uint8_t Checksum;
	uint8_t FeatureInformation1;
	uint8_t FeatureInformation2;
	uint8_t Reserved[3];
};

#define SMP__ConfigEntry__EntryType_Processor					0
#define SMP__ConfigEntry__EntryType_Bus							1
#define SMP__ConfigEntry__EntryType_IOAPIC						2
#define SMP__ConfigEntry__EntryType_IOInterruptAssignment		3
#define SMP__ConfigEntry__EntryType_LocalInterruptAssignment	4


struct _SMP_ConfigHeader
{
	char Signature[4];
	uint16_t Length;
	uint8_t SpecificationRevision;
	uint8_t Checksum;
	char OEMID[8];
	char ProductID[12];
	uint32_t OEMBaseAddress;
	uint16_t OEMLength;
	uint16_t EntryCount;
	uint32_t LocalAPICBaseAddress;
	uint16_t ExtendedLength;
	uint8_t ExtendedChecksum;
	uint8_t Reserved;
};

#define SMP__Processor__Flags_Available			(1 << 0)
#define SMP__Processor__Flags_Bootstrap			(1 << 1)

struct _SMP_Processor
{
	uint8_t EntryType;
	uint8_t LocalAPICID;
	uint8_t LocalAPICVersion;
	uint8_t Flags;
	uint32_t Signature;
	uint32_t FeatureFlags;
	uint32_t Reserved[2];
};

struct _SMP_Bus
{
	uint8_t EntryType;
	uint8_t BusID;
	char BusType[6];
};

#define SMP__IOAPIC__Flags_Enabled				(1 << 0)

struct _SMP_IOAPIC
{
	uint8_t EntryType;
	uint8_t IOAPICID;
	uint8_t IOAPICVersion;
	uint8_t Flags;
	uint32_t IOAPICBaseAddress;
};

struct _SMP_IOInterruptAssignment
{
	uint8_t EntryType;
	uint8_t InterruptType;
	uint16_t Flags;
	uint8_t SourceBusID;
	uint8_t SourceBusIRQ;
	uint8_t DestinationIOAPICID;
	uint8_t DestinationIOAPICInterrupt;
};

struct _SMP_LocalInterruptAssignment
{
	uint8_t EntryType;
	uint8_t InterruptType;
	uint16_t Flags;
	uint8_t SourceBusID;
	uint8_t SourceBusIRQ;
	uint8_t DestinationLocalAPICID;
	uint8_t DestinationLocalAPICInterrupt;
};

void SMP_Startup(APIC* pBootstrapAPIC);
void SMP_Shutdown();
