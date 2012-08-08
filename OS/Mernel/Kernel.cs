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
            int x = 0;
            while (x < 10)
            {
                Write("Hello\n");
                ++x;
            }
            ++x;
            //while (true) { Write("."); }
            //Write("Hello World!\n");
        }
    }
}
