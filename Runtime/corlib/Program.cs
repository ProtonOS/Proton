using System;
using System.Runtime.CompilerServices;

namespace Mernel
{
    internal static class Program
    {
        private static void Main()
        {
            //long ticks = DateTime.InternalUtcNow();
            //Console.WriteLine("Started @ " + ((uint)ticks).ToString());

            string testStr = new string('*', 40);
            Console.WriteLine(testStr);

            //ticks = DateTime.InternalUtcNow();
            //Console.WriteLine("Finished @ " + ((uint)ticks).ToString());
        }

        private static bool Test(bool b)
        {
            return b;
        }
    }
}
