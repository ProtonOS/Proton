extern "C" {
#include <stdio.h>
}

#include <Core/MultiBoot.h>
#include <Runtime/Interpreter.h>

bool Runtime::Interpreter::Startup()
{
    printf("Module: %u, %u, %s\n", (unsigned int)Core::MultiBoot::GetLoadedModule(0).Address, (unsigned int)Core::MultiBoot::GetLoadedModule(0).Length, Core::MultiBoot::GetLoadedModule(0).Identifier);
    return true;
}

void Runtime::Interpreter::Shutdown()
{
}
