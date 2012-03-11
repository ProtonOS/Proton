#include <CLR/InternalCalls/System.IntCommon.h>
#include <CLR/InternalCalls/System.String.h>
#include <stdio.h>
#include <stdlib.h>

//static uint8_t Digits[] = 
//{
//	'0', 0,
//	'1', 0,
//	'2', 0,
//	'3', 0,
//	'4', 0,
//	'5', 0,
//	'6', 0,
//	'7', 0,
//	'8', 0,
//	'9', 0,
//};
//static uint8_t Zero[] = { '0', 0 };

ReferenceTypeObject* System_IntCommon_ToString(AppDomain* pAppDomain, uint64_t* pThis, bool_t pSigned, uint32_t pSize)
{
	//printf("Hello, val is %llu @ 0x%x!\n", (unsigned long long)(*pThis), (unsigned int)pThis);
	//if (*pThis == 0)
	//{
	//	return GC_AllocateString(pAppDomain->GarbageCollector, pAppDomain->RootObject, Zero, 2);
	//}
	//uint16_t* ResultChars = (uint16_t*)malloc(sizeof(uint16_t) * (pSize + 1));
	bool_t isNegative = FALSE;
	uint64_t val = *pThis;
	switch(pSize)
	{
		case 3: // I1 & U1
			if (pSigned)
			{
				if (val & 0x80)
				{
					isNegative = TRUE;
					val = (uint8_t)(-val);
				}
				else
				{
					val &= 0xFF;
				}
			}
			else
			{
				val &= 0xFF;
			}
			break;
		case 5: // I2 & U2
			if (pSigned)
			{
				if (val & 0x8000)
				{
					isNegative = TRUE;
					val = (uint16_t)(-val);
				}
				else
				{
					val &= 0xFFFF;
				}
			}
			else
			{
				val &= 0xFFFF;
			}
			break;
		case 10: // I4 & U4
			if (pSigned)
			{
				if (val & 0x80000000)
				{
					isNegative = TRUE;
					val = (uint32_t)(-val);
				}
				else
				{
					val &= 0xFFFFFFFF;
				}
			}
			else
			{
				val &= 0xFFFFFFFF;
			}
			break;
		case 20: // I8 & U8
			if (pSigned)
			{
				if (val & 0x8000000000000000)
				{
					isNegative = TRUE;
					val = (uint64_t)(-val);
				}
			}
			//printf("Hello, val is 0x%llx @ 0x%x!\n", (unsigned long long)(*pThis), (unsigned int)pThis);
			break;
		default:
			Panic("Invalid pSize!");
			break;
	}
	//int32_t CurrentPos = pSize;
	//while (val > 0) 
	//{
	//	uint8_t Pos = (uint8_t)(val % 10);
	//	val /= 10;
	//	ResultChars[CurrentPos] = ((uint16_t*)Digits)[Pos];
	//	CurrentPos--;
	//}

	//if (isNegative)
	//{
	//	ResultChars[CurrentPos] = '-';
	//	CurrentPos--;
	//}
	//printf("Got here, pSize is %i, IsNegative is %i CurrentPosition is %i Total: %i.\n", (int)pSize, (int)isNegative, (int)(CurrentPos + 1), (int)(pSize - CurrentPos));
	//printf("Value at ResultChars[0 ]: %c\n", (int)ResultChars[0]);
	//printf("Value at ResultChars[1 ]: %c\n", (int)ResultChars[1]);
	//printf("Value at ResultChars[2 ]: %c\n", (int)ResultChars[2]);
	//printf("Value at ResultChars[3 ]: %c\n", (int)ResultChars[3]);
	//if (pSize > 3)
	//{
	//	printf("Value at ResultChars[4 ]: %c\n", (int)ResultChars[4]);
	//	printf("Value at ResultChars[5 ]: %c\n", (int)ResultChars[5]);
	//	printf("Value at ResultChars[6 ]: %c\n", (int)ResultChars[6]);
	//	printf("Value at ResultChars[7 ]: %c\n", (int)ResultChars[7]);
	//	printf("Value at ResultChars[8 ]: %c\n", (int)ResultChars[8]);
	//	printf("Value at ResultChars[9 ]: %c\n", (int)ResultChars[9]);
	//	printf("Value at ResultChars[10]: %c\n", (int)ResultChars[10]);
	//}
	//if (pSize > 10)
	//{
	//	printf("Value at ResultChars[11]: %c\n", (int)ResultChars[11]);
	//	printf("Value at ResultChars[12]: %c\n", (int)ResultChars[12]);
	//	printf("Value at ResultChars[13]: %c\n", (int)ResultChars[13]);
	//	printf("Value at ResultChars[14]: %c\n", (int)ResultChars[14]);
	//	printf("Value at ResultChars[15]: %c\n", (int)ResultChars[15]);
	//	printf("Value at ResultChars[16]: %c\n", (int)ResultChars[16]);
	//	printf("Value at ResultChars[17]: %c\n", (int)ResultChars[17]);
	//	printf("Value at ResultChars[18]: %c\n", (int)ResultChars[18]);
	//	printf("Value at ResultChars[19]: %c\n", (int)ResultChars[19]);
	//	printf("Value at ResultChars[20]: %c\n", (int)ResultChars[20]);
	//}

	//ReferenceTypeObject* result = System_String_Ctor_CharPtrAndStartAndLength(pAppDomain, 0, ResultChars, (uint)(CurrentPos + 1), pSize - CurrentPos);
	//free(ResultChars);

	char buf[32];
	snprintf(buf + isNegative, 32 - isNegative, "%llu", (unsigned long long)val);
	if (isNegative) buf[0] = '-';
	
	ReferenceTypeObject* result = System_String_Ctor_SBytePtr(pAppDomain, 0, (int8_t*)buf);
	return result;
}
