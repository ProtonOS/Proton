#pragma once

namespace Core
{
    //Provides a simple pointer wrapper that does null checks
    template <typename T>
    class Ptr
    {
    public:
        constexpr Ptr() : mPtr {nullptr}
        {
        }
        constexpr Ptr(Ptr && pOther) : mPtr {pOther.mPtr}
        {
        }
        constexpr Ptr(const Ptr & pOther) : mPtr {pOther.mPtr}
        {
        }
        constexpr Ptr(T * pOther) : mPtr {pOther}
        {
        }
        constexpr Ptr & operator=(Ptr && pOther)
        {
            return mPtr = pOther.mPtr, *this;
        }
        constexpr Ptr & operator=(const Ptr & pOther)
        {
            return mPtr = pOther.mPtr, *this;
        }
        constexpr T * operator->() const
        {
            return mPtr ? mPtr : mPtr;
        }
        constexpr T & operator*() const
        {
            return mPtr ? *mPtr : *mPtr;
        }
        Ptr & operator++()
        {
            return ++mPtr, *this;
        }
        constexpr Ptr operator+(UInt mSize) const
        {
            return {mPtr + mSize};
        }
        Ptr & operator+=(UInt pSize)
        {
            return mPtr += pSize, *this;
        }
        constexpr T & operator[](UInt pIndex) const {
            return mPtr ? mPtr[pIndex] : mPtr[pIndex];
        }
        constexpr T * Get() const
        {
            return mPtr ? mPtr : mPtr;
        }
    private:
        T * mPtr;
    };
}