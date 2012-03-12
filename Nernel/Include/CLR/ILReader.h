#pragma once

#include <CLR/IRStructures.h>

IRAssembly* ILReader_CreateAssembly(CLIFile* fil, AppDomain* dom);
void DecomposeMethod(IRMethod* mth);