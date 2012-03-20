using System;
using System.Runtime.CompilerServices;

namespace Mernel
{
    internal static class Kernel
    {
        private static uint[] barr = new uint[] 
        {
            0x000000FF, 0xFF000000
        };

        private static void Main()
        {
            //uint[] aarr = new uint[] 
            //{
            //    0x000000FF, 0xFF000000
            //};
            long ticks = DateTime.InternalUtcNow();
            Console.WriteLine("Started @ " + ((ulong)ticks).ToString());
            //Console.WriteLine("Mernel: Hello World!");

            //Test t = new Test();
            for (uint x = 0; x < 1000000; x++) ;
            //Console.WriteLine("aarr[0] : " + aarr[0].ToString());
            Console.WriteLine("barr[0] : " + barr[0].ToString());
            //Console.WriteLine("carr[0] : " + t.carr[0].ToString());
            //uint test = 0;
            Console.WriteLine("barr[0] : " + barr[0].ToString());
            //Console.WriteLine("test : " + test.ToString());

            ticks = DateTime.InternalUtcNow();
            Console.WriteLine("Finished @ " + ((ulong)ticks).ToString());
        }

        public class Test
        {
            public uint[] carr = new uint[] { 0xFF, 0xFF };
        }
    }
}
