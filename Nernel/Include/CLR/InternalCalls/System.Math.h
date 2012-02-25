#pragma once

#include <CLR/AppDomain.h>

uint64_t System_Math_Sin(AppDomain* pAppDomain, uint64_t pAngle);
uint64_t System_Math_Sinh(AppDomain* pAppDomain, uint64_t pAngle);
uint64_t System_Math_Asin(AppDomain* pAppDomain, uint64_t pSine);
uint64_t System_Math_Cos(AppDomain* pAppDomain, uint64_t pAngle);
uint64_t System_Math_Cosh(AppDomain* pAppDomain, uint64_t pAngle);
uint64_t System_Math_Acos(AppDomain* pAppDomain, uint64_t pCosine);
uint64_t System_Math_Tan(AppDomain* pAppDomain, uint64_t pAngle);
uint64_t System_Math_Tanh(AppDomain* pAppDomain, uint64_t pAngle);
uint64_t System_Math_Atan(AppDomain* pAppDomain, uint64_t pTangent);
uint64_t System_Math_Atan2(AppDomain* pAppDomain, uint64_t pY, uint64_t pX);
uint64_t System_Math_Exp(AppDomain* pAppDomain, uint64_t pExponent);
uint64_t System_Math_Pow(AppDomain* pAppDomain, uint64_t pBase, uint64_t pExponent);
uint64_t System_Math_Sqrt(AppDomain* pAppDomain, uint64_t pNumber);
uint64_t System_Math_Log(AppDomain* pAppDomain, uint64_t pAngle, uint64_t pBase);

