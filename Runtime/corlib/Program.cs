using System;
using System.Runtime.CompilerServices;

namespace Mernel
{
    internal static class Program
    {
        private static void Main()
        {
            uint a = 15;
            Console.WriteLine(test3("Really?", "Really???"));
            Console.Write(test());
            //Console.WriteLine("Mernel: Startup");
            //string str = "Are you sure?";
            //string str2 = "Definately.";
            ////str2 = str2.Replace("fin", "fan");
            ////str2 = str2.Replace("Definately", "Fosho!");
            //Console.WriteLine(str + "\n" + str2);
            //int a = 2;
            //int b = 78;
            //b = 2;
            //a = aVal(b);
            //Console.WriteLine("A Five times is: \"" + Factorial(5, "") + "\"");
            //if (a < b) 
            //    Console.WriteLine("It is!");
            //else
            //    Console.WriteLine("It Isn't!");
            //string st4;
            //string str3 = ((uint)1).ToString();
            Console.WriteLine(a.ToString());
            uint b = 120;
            Console.WriteLine((a * b).ToString());
            Console.WriteLine("Factorial of 6 (Via Addition): " + Factorial(6).ToString());
        }

        private static string test()
        {
            return "Test\n";
        }

        private static string test3(string str1, string str4)
        {
            return str1 + "\nReally??\n" + str4 + "\nReally.";
        }
		
        private static uint Factorial(uint val)
        {
            return val == 0 ? val : val + Factorial(val - 1);
        }
    }
}
