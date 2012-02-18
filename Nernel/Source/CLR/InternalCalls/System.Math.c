#include <CLR/InternalCalls/System.Math.h>
#include <CLR/InternalCalls/Helpers.h>
#include <math.h>

void System_Math_Sin(AppDomain* pAppDomain, uint32_t pArgCount, void** pArgs, void* pReturn)
{
	*(double*)pReturn = sin(INTERNAL_CALL_PARAM(0, double));
}

void System_Math_Cos(AppDomain* pAppDomain, uint32_t pArgCount, void** pArgs, void* pReturn)
{
	*(double*)pReturn = cos(INTERNAL_CALL_PARAM(0, double));
}

void System_Math_Tan(AppDomain* pAppDomain, uint32_t pArgCount, void** pArgs, void* pReturn)
{
	*(double*)pReturn = tan(INTERNAL_CALL_PARAM(0, double));
}

void System_Math_Pow(AppDomain* pAppDomain, uint32_t pArgCount, void** pArgs, void* pReturn)
{
	*(double*)pReturn = pow(INTERNAL_CALL_PARAM(0, double), INTERNAL_CALL_PARAM(1, double));
}

void System_Math_Sqrt(AppDomain* pAppDomain, uint32_t pArgCount, void** pArgs, void* pReturn)
{
	*(double*)pReturn = sqrt(INTERNAL_CALL_PARAM(0, double));
}


