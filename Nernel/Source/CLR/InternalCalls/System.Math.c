#include <CLR/InternalCalls/System.Math.h>
#include <CLR/InternalCalls/Helpers.h>
#include <math.h>

const double NaN = 0.0 / 0.0;
const double NegativeInfinity = -1.0 / 0.0;
const double PositiveInfinity = 1.0 / 0.0;

void System_Math_Sin(AppDomain* pAppDomain, uint32_t pArgCount, void** pArgs, void* pReturn)
{
	*(double*)pReturn = sin(INTERNAL_CALL_PARAM(0, double));
}

void System_Math_Sinh(AppDomain* pAppDomain, uint32_t pArgCount, void** pArgs, void* pReturn)
{
	*(double*)pReturn = sinh(INTERNAL_CALL_PARAM(0, double));
}

void System_Math_Asin(AppDomain* pAppDomain, uint32_t pArgCount, void** pArgs, void* pReturn)
{
	*(double*)pReturn = asin(INTERNAL_CALL_PARAM(0, double));
}

void System_Math_Cos(AppDomain* pAppDomain, uint32_t pArgCount, void** pArgs, void* pReturn)
{
	*(double*)pReturn = cos(INTERNAL_CALL_PARAM(0, double));
}

void System_Math_Cosh(AppDomain* pAppDomain, uint32_t pArgCount, void** pArgs, void* pReturn)
{
	*(double*)pReturn = cosh(INTERNAL_CALL_PARAM(0, double));
}

void System_Math_Acos(AppDomain* pAppDomain, uint32_t pArgCount, void** pArgs, void* pReturn)
{
	*(double*)pReturn = acos(INTERNAL_CALL_PARAM(0, double));
}

void System_Math_Tan(AppDomain* pAppDomain, uint32_t pArgCount, void** pArgs, void* pReturn)
{
	*(double*)pReturn = tan(INTERNAL_CALL_PARAM(0, double));
}

void System_Math_Tanh(AppDomain* pAppDomain, uint32_t pArgCount, void** pArgs, void* pReturn)
{
	*(double*)pReturn = tanh(INTERNAL_CALL_PARAM(0, double));
}

void System_Math_Atan(AppDomain* pAppDomain, uint32_t pArgCount, void** pArgs, void* pReturn)
{
	*(double*)pReturn = atan(INTERNAL_CALL_PARAM(0, double));
}

void System_Math_Atan2(AppDomain* pAppDomain, uint32_t pArgCount, void** pArgs, void* pReturn)
{
	*(double*)pReturn = atan2(INTERNAL_CALL_PARAM(0, double), INTERNAL_CALL_PARAM(1, double));
}

void System_Math_Exp(AppDomain* pAppDomain, uint32_t pArgCount, void** pArgs, void* pReturn)
{
	*(double*)pReturn = exp(INTERNAL_CALL_PARAM(0, double));
}

void System_Math_Pow(AppDomain* pAppDomain, uint32_t pArgCount, void** pArgs, void* pReturn)
{
	*(double*)pReturn = pow(INTERNAL_CALL_PARAM(0, double), INTERNAL_CALL_PARAM(1, double));
}

void System_Math_Sqrt(AppDomain* pAppDomain, uint32_t pArgCount, void** pArgs, void* pReturn)
{
	*(double*)pReturn = sqrt(INTERNAL_CALL_PARAM(0, double));
}

void System_Math_Log(AppDomain* pAppDomain, uint32_t pArgCount, void** pArgs, void* pReturn)
{
	double x = INTERNAL_CALL_PARAM(0, double);
	double y = INTERNAL_CALL_PARAM(1, double);
	if (x == 0.0)
	{
		*(double*)pReturn = NegativeInfinity;
		return;
	}
	if (x < 1.0 && y < 1.0)
	{
		*(double*)pReturn = NaN;
		return;
	}
	double partial = 0.5;
	double integer = 0.0;
	double fraction = 0.0;

	while (x < 1.0)
	{
		integer -= 1.0;
		x *= y;
	}
	while (x >= y)
	{
		integer += 1.0;
		x /= y;
	}
	x *= x;
	while (partial >= 2.22045e-016)
	{
		if (x >= y)
		{
			fraction += partial;
			x /= y;
		}
		partial *= 0.5;
		x *= x;
	}
	*(double*)pReturn = (integer + fraction);
}
