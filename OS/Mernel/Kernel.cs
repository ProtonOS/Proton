using System;
using System.Collections.Generic;

namespace Mernel
{
    public static class Kernel
    {
        private static void Main()
        {
            int x = 0;
            int y = 1;
            if((x + y) == 1) Console.WriteLine("Mernel: Startup");
        }
    }

    public class SomeType<T>
    {
        public static T SomeField;

        public SomeType()
        {
            Console.WriteLine("I says hello!");
        }

        public void AnotherMethod()
        {
            Console.WriteLine("Hy lookide!" + SomeField.ToString());
        }
    }
}
