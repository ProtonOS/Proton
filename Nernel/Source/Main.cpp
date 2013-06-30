#include "MultiBoot.h"
#include "KernelConsole.h"

extern "C" void Main(uint32 pMultiBootMagic, MultiBoot::MultiBootHeader* pMultiBootHeader)
{
	KernelConsole::Load();
	MultiBoot::Load(pMultiBootMagic, pMultiBootHeader);

	KernelConsole::WriteLine("Hello World!");
}
