using System;
using System.Runtime.CompilerServices;

namespace Mernel
{
    internal static class Kernel
    {
        private static void Main()
        {
            long ticks = DateTime.InternalUtcNow();
            Console.WriteLine("Started @ " + ((ulong)ticks).ToString());
            Console.WriteLine("Mernel: Hello World!");

            ticks = DateTime.InternalUtcNow();
            Console.WriteLine("Finished @ " + ((ulong)ticks).ToString());
        }
    }
}
