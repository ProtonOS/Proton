#pragma once

namespace Debug
{
    void WriteString(Core::String pString);

    template <typename T, UInt8 Base>
    void Write(T pValue)
    {
        if (!pValue) return WriteString("0");
        char buf[0x20] = { };
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

    template <char pChar>
    UInt FindFormat(Core::String pString)
    {
        bool found = false;
        for (UInt i = 0; i < pString.Length(); ++i) {
            if (found) {
                if (pString[i] == pChar) {
                    return i - 1;
                } else if (pString[i] == '%') {
                    found = false;
                } else {
                    return -1;
                }
            } else if (pString[i] == '%') {
                found = true;
            }
        }
        return -1;
    }

    void WriteFormat(Core::String pString);

    template <typename ... R>
    void WriteFormat(Core::String pString, UInt pArg, R ... pRest);
    template <typename ... R>
    void WriteFormat(Core::String pString, Int pArg, R ... pRest);
    template <typename ... R>
    void WriteFormat(Core::String pString, Core::String pArg, R ... pRest);

    template <typename ... R>
    void WriteFormat(Core::String pString, UInt pArg, R ... pRest)
    {
        UInt pos;
        if ((pos = FindFormat<'u'>(pString)) != -1) {
            WriteFormat(pString.SubString(0, pos));
            Write<UInt, 10>(pArg);
        } else if ((pos = FindFormat<'i'>(pString)) != -1) {
            WriteFormat(pString.SubString(0, pos));
            Write<Int, 10>(pArg);
        } else if ((pos = FindFormat<'x'>(pString)) != -1) {
            WriteFormat(pString.SubString(0, pos));
            Write<UInt, 16>(pArg);
        } else {
            //Panic
        }
        WriteFormat(pString.SubString(pos + 2, pString.Length()), pRest...);
    }

    template <typename ... R>
    void WriteFormat(Core::String pString, Int pArg, R ... pRest)
    {
        UInt pos;
        if ((pos = FindFormat<'u'>(pString)) != -1) {
            WriteFormat(pString.SubString(0, pos));
            Write<UInt, 10>(pArg);
        } else if ((pos = FindFormat<'i'>(pString)) != -1) {
            WriteFormat(pString.SubString(0, pos));
            Write<Int, 10>(pArg);
        } else if ((pos = FindFormat<'x'>(pString)) != -1) {
            WriteFormat(pString.SubString(0, pos));
            Write<Int, 16>(pArg);
        } else {
            //Panic
        }
        WriteFormat(pString.SubString(pos + 2, pString.Length()), pRest...);
    }

    template <typename ... R>
    void WriteFormat(Core::String pString, Core::String pArg, R ... pRest)
    {
        UInt pos = FindFormat<'s'>(pString);
        if (pos == -1) {
        }//Panic
        WriteFormat(pString.SubString(0, pos));
        WriteString(pArg);
        WriteFormat(pString.SubString(pos + 2, pString.Length()), pRest...);
    }
};
