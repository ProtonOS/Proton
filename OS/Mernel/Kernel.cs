using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;

namespace Mernel
{
    public static class Kernel
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void Write(string pString);

        private static void Main()
        {
            char[] test = new char[] { 'T', 'E', 'S', 'T', '\n' };
            Kernel.Write(new string(test));
        }
    }
}
