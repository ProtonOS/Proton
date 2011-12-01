#include <Core/GDT.h>

using namespace Core;

extern "C" {
extern void GDTUpdate(GDT::Register* pRegister);
}

GDT::Register GDT::sRegister;
GDT::DescriptorsArray GDT::sDescriptors;

void GDT::SetSegment(uint8_t pIndex, uint32_t pAddress, uint32_t pLimit, uint8_t pAccess, uint8_t pFlags)
{
    sDescriptors[pIndex].AddressLow = pAddress & 0xFFFF;
    sDescriptors[pIndex].AddressMid = (pAddress >> 16) & 0xFF;
    sDescriptors[pIndex].AddressHigh = (pAddress >> 24) & 0xFF;
    sDescriptors[pIndex].LimitLow = pLimit & 0xFFFF;
    sDescriptors[pIndex].LimitHighAndFlags = ((pLimit >> 16) & 0x0F) | (pFlags & 0xF0);
    sDescriptors[pIndex].Access = pAccess;
}

bool GDT::Startup()
{
    sDescriptors.fill(Descriptor());

    sRegister.Limit = (sizeof(Descriptor) * MaxDescriptors) - 1;
    sRegister.Address = (uint32_t)sDescriptors.data();

    SetSegment(0, 0x00000000, 0x00000000, 0x00, 0x00);
    SetSegment(1, 0x00000000, 0xFFFFFFFF, Access::ReadWriteOnePresentAccess | Access::ExecutableAccess, Flags::Selector32BitGranularity4KBFlags);
    SetSegment(2, 0x00000000, 0xFFFFFFFF, Access::ReadWriteOnePresentAccess, Flags::Selector32BitGranularity4KBFlags);
    SetSegment(3, 0x00000000, 0xFFFFFFFF, Access::ReadWriteOnePresentAccess | Access::ExecutableAccess | Access::Ring3Access, Flags::Selector32BitGranularity4KBFlags);
    SetSegment(4, 0x00000000, 0xFFFFFFFF, Access::ReadWriteOnePresentAccess | Access::Ring3Access, Flags::Selector32BitGranularity4KBFlags);

    GDTUpdate(&sRegister);
    
     return true;
}

void GDT::Shutdown()
{
}
