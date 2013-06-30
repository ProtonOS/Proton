#include "Core/Core.hpp"
#include "MultiBoot.hpp"
#include "KernelConsole.hpp"
#include "Arch.hpp"

extern "C" void Main(UInt32 pMultiBootMagic, MultiBoot::MultiBootHeader* pMultiBootHeader)
{
	KernelConsole::Load();
	MultiBoot::Load(pMultiBootMagic, pMultiBootHeader);
	Arch::Load();

	KernelConsole::WriteLine("Hello World!");
	while (true) ;
}
