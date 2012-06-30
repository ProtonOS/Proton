#include "Console.h"

Console* Console::Instance = nullptr;

Console::Console()
{
	Instance = this;
}
