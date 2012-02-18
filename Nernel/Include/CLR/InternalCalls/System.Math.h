#pragma once

#include <CLR/AppDomain.h>

void System_Math_Sin(AppDomain* pAppDomain, uint32_t pArgCount, void** pArgs, void* pReturn);
void System_Math_Cos(AppDomain* pAppDomain, uint32_t pArgCount, void** pArgs, void* pReturn);
void System_Math_Tan(AppDomain* pAppDomain, uint32_t pArgCount, void** pArgs, void* pReturn);
void System_Math_Pow(AppDomain* pAppDomain, uint32_t pArgCount, void** pArgs, void* pReturn);
void System_Math_Sqrt(AppDomain* pAppDomain, uint32_t pArgCount, void** pArgs, void* pReturn);

