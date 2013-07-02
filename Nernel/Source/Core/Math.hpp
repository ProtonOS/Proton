#pragma once

namespace Core
{
    inline bool IsPowerOfTwo(UInt pArg)
    {
        return !(pArg & (pArg - 1));
    }
    inline UInt GetHighestBit(UInt pArg)
    {
        UInt n;
        for (n = 0; pArg; ++n) pArg >>= 1;
        return n;
    }
    inline UInt RoundUpToPowerOfTwo(UInt pArg)
    {
        if (IsPowerOfTwo(pArg)) return pArg;
        return 1 << GetHighestBit(pArg);
    }

}
