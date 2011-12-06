using System;
using System.Runtime.CompilerServices;

namespace Proton.IO
{
    internal static class MemoryIO
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern byte InByte(uint address);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern ushort InUShort(uint address);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern uint InUInt(uint address);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void OutByte(uint address, byte value);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void OutUShort(uint address, ushort value);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void OutUInt(uint address, uint value);
    }
}
