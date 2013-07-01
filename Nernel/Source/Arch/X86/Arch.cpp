#include "../../Core/Core.hpp"

#include "Arch.hpp"
#include "GDT.hpp"
#include "IDT.hpp"
#include "PIC.hpp"
#include "PIT.hpp"
#include "RTC.hpp"

void Arch::Load()
{
    GDT::Load();
    PIC::Load();
	PIT::Load();
    IDT::Load();
	RTC::Load();
}
