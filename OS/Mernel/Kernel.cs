using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;

namespace Mernel
{
    internal class TestA
    {
        public void TestMethod(string param)
        {
            Kernel.Write(param);
        }
    }
    public static class Kernel
    {
        private static string StaticTest;
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void Write(string pString);

        static Kernel()
        {
            StaticTest = "Test\n";
            Kernel.Write("Static Constructor called!\n");
        }

        private static void Main()
        {
            Kernel.Write(StaticTest);
            Kernel.Write(StaticTest);
            Kernel.Write(StaticTest);
            //Console.Write("Hello!");
            //int i = 0;
            //i++;
            //TestA b = new TestA();
            //b.TestMethod("Test");
            //Kernel.Write(Test2(i));
            //string s = i.ToString();
            //Kernel.Write(s);
            //Kernel.Write("\n");
            //Kernel.Write("Hello!");
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

        private static string Test2(int i)
        {
            return Test3(i);
        }

        private static string Test3(int i)
        {
            Kernel.Write("Test\n");
            i++;
            return "Hello!";
        }
    }
}
