#pragma once

namespace Core
{
    template <typename T>
    class ConstPtr;
    //Provides a simple pointer wrapper that does null checks
    template <typename T>
    class Ptr
    {
    public:
        Ptr() : mPtr {nullptr} { }
        Ptr(Ptr && ) = default;
        Ptr(const Ptr &) = default;
        Ptr(T * pOther) : mPtr {pOther} { }
        Ptr & operator=(Ptr && ) = default;
        Ptr & operator=(const Ptr &)= default;
        T * operator->() const
        {
            Check();
            return mPtr;
        }
        T & operator*() const
        {
            return *mPtr;
        }
        Ptr & operator++()
        {
            return ++mPtr, *this;
        }
        Ptr operator+(UInt mSize) const
        {
            return {mPtr + mSize};
        }
        Ptr & operator+=(UInt pSize)
        {
            return mPtr += pSize, *this;
        }
        T & operator[](UInt pIndex) const {
            Check();
            return mPtr[pIndex];
        }
        T * Get() const
        {
            Check();
            return mPtr;
        }
        void Check() const
        {
            if (!mPtr) {
                /*Panic("Null Pointer Access");*/
            }
        }
    protected:
        T * mPtr;
    };
    template <typename T>
    class Ptr<const T> : public Ptr<T> {
    public:
        Ptr() : Ptr<T>::mPtr {nullptr} { }
        Ptr(Ptr && ) = default;
        Ptr(Ptr const &) = default;
        Ptr(Ptr<T> && pOther) : Ptr<T>::mPtr {pOther.mPtr} { }
        Ptr(Ptr<T> const & pOther) : Ptr<T>::mPtr {pOther.mPtr} { }
        Ptr(T * pOther) : Ptr<T>::mPtr {pOther} { }
        Ptr(T const * pOther) : Ptr<T>::mPtr {pOther} { }
    };
}