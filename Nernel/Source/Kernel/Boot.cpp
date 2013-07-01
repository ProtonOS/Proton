constexpr UInt32 gBootStackSize = 0x100000;
UInt8 gBootStack[gBootStackSize];
Core::Ptr<const UInt8> gBootStackTop = gBootStack + gBootStackSize;
Core::Ptr<const UInt8> gBootStackBottom = gBootStack;

constexpr UInt32 gTempStackSize = 0x100000;
UInt8 gTempStack[gTempStackSize];
Core::Ptr<const UInt8> gTempStackTop = gTempStack + gTempStackSize;
Core::Ptr<const UInt8> gTempStackBottom = gTempStack;
Core::Ptr<UInt8> gTempStackCursor = gTempStack + gTempStackSize;

extern "C" void ProtectedModeStartup()
{
	// TODO: IDTUpdate, Processor::Create, UserModeBoot
	while (true) ;
}

extern "C" void UserModeStartup()
{
	// TODO: Create idle thread for each processor, and
	// wait for apic timer to interrupt and call into
	// thread scheduler, which is disabled until bootstrap
	// processor enters here as the last processor
}
