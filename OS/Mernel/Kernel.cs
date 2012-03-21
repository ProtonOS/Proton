using System;
using System.Runtime.CompilerServices;

namespace Mernel
{
    internal static class Kernel
    {
        private static void Main()
        {
            long startedAt = DateTime.InternalUtcNow();
            Console.WriteLine("Started @ " + ((ulong)startedAt).ToString());

            if (startedAt > 0) throw new Exception();

            for (uint x = 0; x < 10000000; x++) ;

            long finishedAt = DateTime.InternalUtcNow();
            //Console.WriteLine("Finished @ " + ((ulong)finishedAt).ToString());
            Console.WriteLine("Finished @ " + ((ulong)finishedAt).ToString() + ", " + ((ulong)finishedAt - (ulong)startedAt).ToString() + "MS");
        }

        public class Test
        {
            public uint[] carr = new uint[] { 0xFF, 0xFF };
        }
    }
}
