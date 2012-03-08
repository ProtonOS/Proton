using System;
using System.Runtime.CompilerServices;

namespace Mernel
{
    internal static class Program
    {
        private static ulong Blarh342()
        {
            return 42;
        }

        private static void Main()
        {
            long ticks = DateTime.InternalUtcNow();
            Console.WriteLine("Started @ " + ((uint)ticks).ToString());
            //ulong blargh = 0x000000FF0000FFFF;
            //Console.WriteLine("Blargh! " + ((uint)blargh).ToString());
            //char[] @char = new char[4];
            //uint[] uarr = new uint[3];
            //uarr[2] = 93;
            //Console.WriteLine(uarr[2].ToString());
            //Test[] tarr = new Test[9];
            //tarr[0] = new Test();
            //Console.WriteLine(tarr[0].ToString());
            //object[] objarr = new object[2];
            //objarr[0] = new Test();
            //Console.WriteLine(objarr[0].ToString());
            //uint ch = (uint)objarr[0];
            //@char[0] = 'T';
            //@char[1] = 'E';
            //@char[2] = 'S';
            //@char[3] = 'T';
            //string str2 = new string(@char);
            //string str = new string('*', 40);
            //Console.WriteLine(str);
            //Console.WriteLine(str2);
            //Test t = new Test();
            //t.Really();
            //t.JustATest();
            //Console.WriteLine("Factorial of 6 (Via Multiplication): " + Factorial(6).ToString());
            //Console.WriteLine("Mernel Started");

            Bob b = new Bob();
            b.FindPeter();
            

            uint x = 10;
            switch (x)
            {
                case 1:
                    Console.WriteLine("Switched for 1");
                    break;
                case 2:
                    Console.WriteLine("Switched for 2");
                    break;
                default:
                    Console.WriteLine("Switched for " + x.ToString());
                    break;
            }
            b.HeWasLate();

            ticks = DateTime.InternalUtcNow();
            Console.WriteLine("Finished @ " + ((uint)ticks).ToString());
        }

        private static void ActionTest() { Console.WriteLine("ActionTest"); }

        private static uint Factorial(uint val)
        {
            return (val == 1) ? val : val * Factorial(val - 1);
        }
    }

    internal sealed class Bob
    {
        //public Bob() { }

        public void FindPeter()
        {
            Console.Write("Looking for Peter.");
            for (uint i2 = 10; i2 < 30; i2++)
            {
                for (uint i = 0; i < 5000000; i++) ;
                Console.Write(".");
            }
            Console.WriteLine("Failed to find Peter.");
        }

        public void HeWasLate()
        {
            Console.WriteLine("I found Peter, but I don't need him anymore....");
        }
    }

    internal sealed class Test
    {
        uint A;

        public Test()
        {
            Console.WriteLine("Creating a new Test object.");
            A = 4;
            Console.WriteLine("Finished Creating a new Test object.");
        }

        public void Really()
        {
            Console.WriteLine("Hey look! It works!");
        }

        public void JustATest()
        {
            Console.WriteLine("Value of A: " + A.ToString());
        }

        //public override string ToString()
        //{
        //    return "It Says hello!";
        //}
    }
}
