#pragma once

#if defined(_WIN32) && _MSC_VER < 1800
#  define constexpr
#endif
namespace Core
{
    //Provides a simple pointer wrapper that does null checks
    template <typename T>
    class Ptr
    {
    public:
        constexpr Ptr() : mPtr (nullptr)
        {
        }
        constexpr Ptr(const Ptr & pOther) : mPtr (pOther.mPtr)
        {
        }
        constexpr Ptr(T * pOther) : mPtr (pOther)
        {
        }
        Ptr & operator=(const Ptr & pOther)
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
        Ptr operator++(int)
        {
            return {mPtr++};
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
#if defined(_WIN32) && _MSC_VER < 1800
#  define constexpr const
#endif