using System;
using System.Collections.Generic;

namespace Mernel
{
    public static class Kernel
    {
        private static void Main()
        {
            //int x = 0;
            //int y = 1;
            ////if ((x + y) == 0) return;
            ////else if ((x + y) == 1) Console.WriteLine("Mernel: Startup");
            ////else if ((x + y) == 2) Console.WriteLine("Mernel: WTF");
            ////x++;
            //int z = 0;
            //z = (x > y) ? 1 : 0;
            //z++;
            ////if (x > 0 && (z = ((x + y) > 1 ? 9 : 10)) == 9)
            ////{
            ////    x = z;
            ////}
            bool a = true; // 0 - 0
            bool b = false;
            int x = 1;
            TestLabel: // 1 - 0
            if (a)
            {
                x = 0; // 2 - 1
                if (x == 0)
                {
                    b = true; // 3 - 2
                    // stloc.0
                    // br.s LoopCondition
                    // LoopBody // 4 - 12
                    // beq x--
                    // ld b // 5 - 4
                    // brfalse Load30
                    // ld 92 // 6 - 5
                    // br.s AfterLoad
                    // Load30: ld 30 // 7 - 5
                // AfterLoad:
                    // dup // 8 - 6
                    // stloc x
                    // ld 0
                    // beq x++
                    // ld b // 9 - 8
                    // brfalse x++
                    // x-- // 10 - 4
                    // x++ // 11 - 8
                    // i34++
                    // LoopCondition // 12 - 3
                    // brtrue.s LoopBody
                    for (int i34 = 0; i34 < 99; i34++)
                    {
                        if (i34 == 8 || ((x = (b ? 92 : 30)) != 0 && b))
                        {
                            x--;
                        }
                        x++;
                    }
                    // 13 - 12
                }
                else
                {
                    x = 0; // 14 - 2
                    goto TestLabel;
                }
                x = 1; // 15 - 13
            }
            a = b; // 16 - 1
        }
    }
}
