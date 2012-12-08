using System;

namespace Proton.VM.IR
{
    [Flags]
    public enum IRPrefixFlags
    {
        None = 0 << 0,
        Unaligned = 1 << 0,
        Volatile = 1 << 1,
        Tail = 1 << 2,
        Constrained = 1 << 3,
        No = 1 << 4,
        ReadOnly = 1 << 5,
    }
}
