#include <CLR/InternalCalls/System.Math.h>
#include <CLR/InternalCalls/Helpers.h>
#include <math.h>

const double NaN = 0.0 / 0.0;
const double NegativeInfinity = -1.0 / 0.0;
const double PositiveInfinity = 1.0 / 0.0;

uint64_t System_Math_Sin(AppDomain* pAppDomain, uint64_t pAngle)
{
	return sin((double)pAngle);
}

uint64_t System_Math_Sinh(AppDomain* pAppDomain, uint64_t pAngle)
{
	return sinh((double)pAngle);
}

uint64_t System_Math_Asin(AppDomain* pAppDomain, uint64_t pSine)
{
	return asin((double)pSine);
}

uint64_t System_Math_Cos(AppDomain* pAppDomain, uint64_t pAngle)
{
	return cos((double)pAngle);
}

uint64_t System_Math_Cosh(AppDomain* pAppDomain, uint64_t pAngle)
{
	return cosh((double)pAngle);
}

uint64_t System_Math_Acos(AppDomain* pAppDomain, uint64_t pCosine)
{
	return acos((double)pCosine);
}

uint64_t System_Math_Tan(AppDomain* pAppDomain, uint64_t pAngle)
{
	return tan((double)pAngle);
}

uint64_t System_Math_Tanh(AppDomain* pAppDomain, uint64_t pAngle)
{
	return tanh((double)pAngle);
}

uint64_t System_Math_Atan(AppDomain* pAppDomain, uint64_t pTangent)
{
	return atan((double)pTangent);
}

uint64_t System_Math_Atan2(AppDomain* pAppDomain, uint64_t pY, uint64_t pX)
{
	return atan2((double)pY, (double)pX);
}

uint64_t System_Math_Exp(AppDomain* pAppDomain, uint64_t pExponent)
{
	return exp((double)pExponent);
}

uint64_t System_Math_Pow(AppDomain* pAppDomain, uint64_t pBase, uint64_t pExponent)
{
	return pow((double)pBase, (double)pExponent);
}

uint64_t System_Math_Sqrt(AppDomain* pAppDomain, uint64_t pNumber)
{
	return sqrt((double)pNumber);
}

uint64_t System_Math_Log(AppDomain* pAppDomain, uint64_t pAngle, uint64_t pBase)
{
	double x = (double)pAngle;
	double y = (double)pBase;
	if (x == 0.0)
	{
		return (uint64_t)NegativeInfinity;
	}
	if (x < 1.0 && y < 1.0)
	{
		return (uint64_t)NaN;
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
	return (uint64_t)(integer + fraction);
}
