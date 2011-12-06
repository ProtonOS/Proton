using System;
using System.Runtime.CompilerServices;

namespace Proton.IO
{
    internal static class PortIO
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern byte InByte(ushort port);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern ushort InUShort(ushort port);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern uint InUInt(ushort port);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void OutByte(ushort port, byte value);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void OutUShort(ushort port, ushort value);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void OutUInt(ushort port, uint value);
    }
}
