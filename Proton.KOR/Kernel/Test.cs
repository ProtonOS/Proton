using System.Collections.Generic;

namespace System.Kernel
{
    public class Test<T> : List<List<string>>
    {
    }

    public static class Blah
    {
        public static void Blah2()
        {
            int w = 0;
            int x = 0;
            int y = 0;
            switch (x)
            {
                case 0:
                    if (y == 0) y = w == 0 ? 1 : 3;
                    break;
                case 1:
                    if (y == 1) y = 2;
                    break;
                case 5:
                    if (y == 5) y = 5;
                    break;
                case 10:
                    if (y == 10) y = 10;
                    break;
                default: break;
            }
            int z = x + y;
        }
    }
}
