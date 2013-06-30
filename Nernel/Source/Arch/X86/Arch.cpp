#include "Arch.h"
#include "GDT.h"
#include "IDT.h"
#include "PIC.h"

void Arch::Load()
{
	GDT::Load();
	PIC::Load();
	IDT::Load();
}
