#include <CLR/InternalCalls/System.UInt32.h>
#include <CLR/InternalCalls/System.String.h>

static uint8_t Digits[] = 
{
	'0', 0,
	'1', 0,
	'2', 0,
	'3', 0,
	'4', 0,
	'5', 0,
	'6', 0,
	'7', 0,
	'8', 0,
	'9', 0,
};
static uint8_t Zero[] = { '0', 0 };
ReferenceTypeObject* System_UInt32_ToString(AppDomain* pAppDomain, uint32_t* pThis)
{
	if (*pThis == 0)
	{
		return GC_AllocateString(pAppDomain->GarbageCollector, pAppDomain->RootObject, Zero, 2);
	}
	uint16_t* ResultChars = (uint16_t*)malloc(sizeof(uint16_t) * 11);
	int32_t CurrentPos = 10;
	uint32_t Value = *pThis;
	while (Value > 0) 
	{
		uint8_t Pos = (uint8_t)(Value % 10);
		Value /= 10;
		ResultChars[CurrentPos] = ((uint16_t*)Digits)[Pos];
		CurrentPos -= 1;	
	}
	ReferenceTypeObject* result = System_String_Ctor_CharPtrAndStartAndLength(pAppDomain, 0, ResultChars, CurrentPos + 1, 10 - CurrentPos);
	free(ResultChars);
	return result;
}
