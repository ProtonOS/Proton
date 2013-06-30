#include "Arch.h"
#include "GDT.h"

void Arch::Load()
{
	GDT::Load();
}
