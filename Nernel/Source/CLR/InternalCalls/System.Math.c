#include <math.h>
#include <CLR/InternalCalls/System.Math.h>

uint64_t System_Math_Floor(AppDomain* pAppDomain, uint64_t pValue)
{
	double d = floor(*(double*)&pValue);
	return *(uint64_t*)&d;
}

uint64_t System_Math_Round(AppDomain* pAppDomain, uint64_t pValue)
{
	double d = round(*(double*)&pValue);
	return *(uint64_t*)&d;
}

uint64_t System_Math_Sin(AppDomain* pAppDomain, uint64_t pValue)
{
	double d = sin(*(double*)&pValue);
	return *(uint64_t*)&d;
}

uint64_t System_Math_Cos(AppDomain* pAppDomain, uint64_t pValue)
{
	double d = cos(*(double*)&pValue);
	return *(uint64_t*)&d;
}

uint64_t System_Math_Tan(AppDomain* pAppDomain, uint64_t pValue)
{
	double d = tan(*(double*)&pValue);
	return *(uint64_t*)&d;
}

uint64_t System_Math_Sinh(AppDomain* pAppDomain, uint64_t pValue)
{
	double d = sinh(*(double*)&pValue);
	return *(uint64_t*)&d;
}

uint64_t System_Math_Cosh(AppDomain* pAppDomain, uint64_t pValue)
{
	double d = cosh(*(double*)&pValue);
	return *(uint64_t*)&d;
}

uint64_t System_Math_Tanh(AppDomain* pAppDomain, uint64_t pValue)
{
	double d = tanh(*(double*)&pValue);
	return *(uint64_t*)&d;
}

uint64_t System_Math_Acos(AppDomain* pAppDomain, uint64_t pValue)
{
	double d = acos(*(double*)&pValue);
	return *(uint64_t*)&d;
}

uint64_t System_Math_Asin(AppDomain* pAppDomain, uint64_t pValue)
{
	double d = asin(*(double*)&pValue);
	return *(uint64_t*)&d;
}

uint64_t System_Math_Atan(AppDomain* pAppDomain, uint64_t pValue)
{
	double d = atan(*(double*)&pValue);
	return *(uint64_t*)&d;
}

uint64_t System_Math_Atan2(AppDomain* pAppDomain, uint64_t pValue1, uint64_t pValue2)
{
	double d = atan2(*(double*)&pValue1, *(double*)&pValue2);
	return *(uint64_t*)&d;
}

uint64_t System_Math_Exp(AppDomain* pAppDomain, uint64_t pValue)
{
	double d = exp(*(double*)&pValue);
	return *(uint64_t*)&d;
}

uint64_t System_Math_Log(AppDomain* pAppDomain, uint64_t pValue)
{
	double d = log(*(double*)&pValue);
	return *(uint64_t*)&d;
}

uint64_t System_Math_Log10(AppDomain* pAppDomain, uint64_t pValue)
{
	double d = log10(*(double*)&pValue);
	return *(uint64_t*)&d;
}

uint64_t System_Math_Pow(AppDomain* pAppDomain, uint64_t pValue1, uint64_t pValue2)
{
	double d = pow(*(double*)&pValue1, *(double*)&pValue2);
	return *(uint64_t*)&d;
}

uint64_t System_Math_Sqrt(AppDomain* pAppDomain, uint64_t pValue)
{
	double d = sqrt(*(double*)&pValue);
	return *(uint64_t*)&d;
}
