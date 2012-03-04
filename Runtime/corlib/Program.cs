using System;
using System.Runtime.CompilerServices;

namespace Mernel
{
    internal static class Program
    {
        private static void Main()
        {
            Console.WriteLine("Mernel Started @ " + ((uint)DateTime.InternalUtcNow()).ToString());
            Console.WriteLine("Factorial of 6 (Via Multiplication): " + Factorial(6).ToString());
        }

        private static uint Factorial(uint val)
        {
            return (val == 1) ? val : val * Factorial(val - 1);
        }
    }
}
