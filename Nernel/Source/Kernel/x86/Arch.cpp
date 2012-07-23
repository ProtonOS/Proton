#include "../Arch.h"
#include "Console.h"
#include "GDT.h"
#include "IDT.h"
#include "PIC.h"
#include "PIT.h"
#include "RTC.h"
#include "SMP.h"
#include "SystemClock.h"

namespace Arch
{
    void Startup()
    {
		Console::Startup();
		GDT::Startup();
		IDT::Startup();
		PIC::Startup();
		PIT::Startup();
		RTC::Startup();
		SystemClock::Startup();

		PIC::StartInterrupts();

		SMP::Startup();
		while (true) ;
    }
}
