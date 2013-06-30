#pragma once
#include "Ptr.hpp"
#include "Types.hpp"
namespace Core
{
    namespace Memory
    {
        //Requires that the destination and source do not overlap
        //Also does not work with any data type which is not POD due to virtual pointers
        template <typename T>
        void ByteCopy(Core::Ptr<T> pDest, Core::Ptr<const T> pSrc, UInt pLen)
        {
            UInt full = pLen * sizeof(T);
            UInt large = full >> Type<UInt>::Shift;
            UInt small = full & Type<UInt>::Mask;
            UInt const * srclarge = reinterpret_cast<UInt const *>(pSrc.Get());
            UInt * destlarge = reinterpret_cast<UInt *>(pDest.Get());
            for (UInt i = large; i; --i) {
                *destlarge++ = *srclarge++;
            }
            UInt8 const * srcsmall = reinterpret_cast<UInt8 const *>(pSrc.Get());
            UInt8 * destsmall = reinterpret_cast<UInt8 *>(pDest.Get());
            for (UInt i = small; i; --i) {
                *destsmall++ = *srcsmall++;
            }
        }
        template <typename T>
        void ByteCopy(Core::Ptr<T> pDest, Core::Ptr<T> pSrc, UInt pLen)
        {
            return ByteCopy<T>(pDest, pSrc, pLen);
        }
    }
}