#include "MultiBoot.hpp"
#include "KernelConsole.h"
#include "Arch.h"

extern "C" void Main(uint32 pMultiBootMagic, MultiBoot::MultiBootHeader* pMultiBootHeader)
{
	KernelConsole::Load();
	MultiBoot::Load(pMultiBootMagic, pMultiBootHeader);
	Arch::Load();

	KernelConsole::WriteLine("Hello World!");
	while (true) ;
}
