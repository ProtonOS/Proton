#pragma once

namespace Core
{
    class String
    {
    public:
        String() : mStr(nullptr), mLength(0)
        {
        }
        String(String const & pOther) : mStr(pOther.mStr), mLength(pOther.mLength)
        {
        }
        String(Core::Ptr<char const> pString) : mStr(pString.Get()), mLength(0)
        {
            for (Core::Ptr<char const> i = pString; *i; ++i, ++mLength);
        }
        String(Core::Ptr<char const> pString, UInt pLength) : mStr(pString.Get()), mLength(pLength)
        {
        }
        String(char const * pString) : String(Core::Ptr<char const>(pString))
        {
        }
        UInt Length()
        {
            return mLength;
        }
        Core::Ptr<char const> Ptr()
        {
            return mStr;
        }
        char const & operator[](UInt pIndex) {
            return mStr[pIndex];
        }
        String SubString(UInt pBegin, UInt pEnd)
        {
            return {mStr + pBegin, pEnd - pBegin};
        }
    private:
        Core::Ptr<char const> mStr;
        UInt mLength;
    };
}