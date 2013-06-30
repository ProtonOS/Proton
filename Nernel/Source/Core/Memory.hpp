#pragma once
#include "Ptr.hpp"
#include "Types.hpp"
namespace Core {
    namespace Memory {
        template <typename T>
        void ByteCopy(Core::Ptr<T> dest, Core::Ptr<T const> src, size_t len) {
            size_t full = len * sizeof(T);
            size_t large = full >> Types::SizeShift;
            size_t small = full & Types::SizeMask;
            size_t const * srclarge = reinterpret_cast<size_t const *>(src.Get());
            size_t * destlarge = reinterpret_cast<size_t *>(src.Get());
            for (size_t i = large; i; --i) {
                *destlarge++ = *srclarge++;
            }
            size_t const * srcsmall = reinterpret_cast<uint8 const *>(src.Get());
            size_t * destsmall = reinterpret_cast<uint8 *>(src.Get());
            for (size_t i = small; i; --i) {
                *destsmall++ = *srcsmall++;
            }
        }
    }
}