#pragma once

#include <CLR/AppDomain.h>

void System_NumberFormatter_GetFormatterTables(AppDomain* pAppDomain, const uint64_t** pMantissaBitsTable, const int32_t** pTensExponentTable, const uint16_t** pDigitLowerTable, const uint16_t** pDigitUpperTable, const int64_t** pTenPowersList, const int32_t** pDecimalHexDigits);
