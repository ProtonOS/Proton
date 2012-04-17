using System;

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
        }
    }
}
