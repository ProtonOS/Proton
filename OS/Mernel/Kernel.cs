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
            //char[] test = new char[] { 'T', 'E', 'S', 'T', '\n' };
            char[] test = new char[5];
            test[0] = 'T';
            test[1] = 'E';
            test[2] = 'S';
            test[3] = 'T';
            test[4] = '\n';
            Kernel.Write(new string(test));
            //Kernel.Write("Test\n");
        }
    }
}
