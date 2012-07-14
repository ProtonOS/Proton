#include "../Arch.h"
#include "GDT.h"
#include "IDT.h"

namespace Arch
{
    void Startup()
    {
		GDT::Startup();
		IDT::Startup();
    }
}
