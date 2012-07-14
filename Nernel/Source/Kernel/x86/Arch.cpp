#include "../Arch.h"
#include "GDT.h"

namespace Arch
{
    void Startup()
    {
		GDT::Startup();
    }
}
