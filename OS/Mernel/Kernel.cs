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
            //if ((x + y) == 0) return;
            //else if ((x + y) == 1) Console.WriteLine("Mernel: Startup");
            //else if ((x + y) == 2) Console.WriteLine("Mernel: WTF");
            //x++;
            int z = 0;
            z = (x > y) ? 1 : 0;
            z++;
            //if (x > 0 && (z = ((x + y) > 1 ? 9 : 10)) == 9)
            //{
            //    x = z;
            //}
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
