#include "../Arch.h"
#include "GDT.h"
#include "IDT.h"
#include "../PIC.h"
#include "../PIT.h"
#include "../RTC.h"
#include "../SystemClock.h"

namespace Arch
{
    void Startup()
    {
		GDT::Startup();
		IDT::Startup();
		PIC::Startup();
		PIT::Startup();
		RTC::Startup();
		SystemClock::Startup();

		PIC::StartInterrupts();
		printf("Started\n");
    }
}
