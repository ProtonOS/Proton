#include <PortIO.h>
#include <Core/IDT.h>
#include <Driver/PIC8259A.h>

Driver::PIC8259A::PIC8259A()
{
}

bool Driver::PIC8259A::OnRegister()
{
    if (!IsIOPortAvailable(MasterCommandIOPort) ||
        !IsIOPortAvailable(MasterDataIOPort) ||
        !IsIOPortAvailable(SlaveCommandIOPort) ||
        !IsIOPortAvailable(SlaveDataIOPort)) return false;

    ClaimIOPort(MasterCommandIOPort);
    ClaimIOPort(MasterDataIOPort);
    ClaimIOPort(SlaveCommandIOPort);
    ClaimIOPort(SlaveDataIOPort);

    // ICW 1
	outb(MasterCommandIOPort, Command::ConfigModeCommand | Command::InitializeCommand); IOWAIT();
	outb(SlaveCommandIOPort, Command::ConfigModeCommand | Command::InitializeCommand); IOWAIT();

	// ICW 2
	outb(MasterDataIOPort, Core::IDT::RemappedIRQBase); IOWAIT();
	outb(SlaveDataIOPort, Core::IDT::RemappedIRQBase + 0x08); IOWAIT();

	// ICW 3
	outb(MasterDataIOPort, Line::MasterToSlaveLine); IOWAIT();
	outb(SlaveDataIOPort, Line::SlaveToMasterLine); IOWAIT();

	// ICW 4
	outb(MasterDataIOPort, Mode::Microprocessor8086Mode); IOWAIT();
	outb(SlaveDataIOPort, Mode::Microprocessor8086Mode); IOWAIT();

	outb(MasterDataIOPort, 0x00); IOWAIT();
	outb(SlaveDataIOPort, 0x00); IOWAIT();

	StartInterrupts();

    return true;
}

void Driver::PIC8259A::OnUnregister()
{
    ReleaseIOPort(MasterCommandIOPort);
    ReleaseIOPort(MasterDataIOPort);
    ReleaseIOPort(SlaveCommandIOPort);
    ReleaseIOPort(SlaveDataIOPort);
}

void Driver::PIC8259A::StartInterrupts() { asm("sti"); }

void Driver::PIC8259A::StopInterrupts() { asm("cli"); }

void Driver::PIC8259A::ResetInterrupts(bool pSlave)
{
    if (pSlave) outb(SlaveCommandIOPort, Command::ResetCommand);
    outb(MasterCommandIOPort, Command::ResetCommand);
}
