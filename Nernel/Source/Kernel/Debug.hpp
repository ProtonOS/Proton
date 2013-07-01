#pragma once

namespace Debug
{
    void WriteString(const char* pString);

    template <typename T, UInt8 Base>
    void Write(T pValue) {
        if (!pValue) return WriteString("0");
        char buf[0x20] = {};
        bool negative = pValue < 0;
        if (negative) pValue = -pValue;
        UInt i;
        for (i = 0x19; pValue; --i) {
            T quot = pValue / Base;
            T rem = pValue % Base;
            buf[i] = rem + (rem < 10 ? 48 : 55);
            pValue = quot;
        }
        if (negative) buf[i] = '-';
        else ++i;
        WriteString(buf + i);
    }
};
