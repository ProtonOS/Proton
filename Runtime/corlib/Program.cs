﻿using System;
using System.Runtime.CompilerServices;

namespace Mernel
{
    internal static class Program
    {
        private static void Main()
        {

            Console.WriteLine("Mernel: Startup");
            string str = "Are you sure?";
            string str2 = "Definately.";
            Console.WriteLine(str + "\n" + str2);
            //Console.WriteLine((string)((int)1).ToString());
        }
    }
}