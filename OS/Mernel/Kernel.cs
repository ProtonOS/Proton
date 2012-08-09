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
            Console.Write("Hello!");
            //int x = 0xFFF;
            //int y = 10;
            //int z = 0;
            //for (int i = 0; i < 10; i++)
            //{
            //    z = x * y;
            //}
            //++z;
            //while (true) { Write("."); }
            //Write("Hello World!\n");
        }
    }
}
