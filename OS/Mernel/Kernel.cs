using System;
using System.Collections.Generic;

namespace Mernel
{
    public static class Kernel
    {
        private static void Main()
        {
            int x = 0;
            ++x;
            if(x == 1) Console.WriteLine("Mernel: Startup");
            switch (x)
            {
                case 0:
                    Console.WriteLine("Case 0");
                    break;
                case 1:
                    Console.WriteLine("Case 1");
                    break;
                default:
                    Console.WriteLine("Case Default");
                    break;
            }
            SomeType<int> test = new SomeType<int>();
            test.AnotherMethod();
            //Type t = typeof(SomeType<int>);
            //Console.WriteLine(t.Name);
        }
    }

    public class SomeType<T>
    {
        public T SomeField;

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
