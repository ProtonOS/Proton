using System;
using System.Runtime.CompilerServices;

namespace Mernel
{
    internal static class Program
    {
        private static void Main()
        {
            char[] @char = new char[4];
            @char[0] = 'T';
            @char[1] = 'E';
            @char[2] = 'S';
            @char[3] = 'T';
            string str2 = new string(@char);
            string str = new string('*', 40);
            Console.WriteLine(str);
            Console.WriteLine(str2);
            Console.WriteLine("Mernel Started");
            Console.WriteLine("Factorial of 6 (Via Multiplication): " + Factorial(6).ToString());
        }

        private static void ActionTest() { Console.WriteLine("ActionTest"); }

        private static uint Factorial(uint val)
        {
            return (val == 1) ? val : val * Factorial(val - 1);
        }
    }
}
