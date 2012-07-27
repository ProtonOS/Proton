using System;
using System.Collections.Generic;

namespace Mernel
{
    public static class Kernel
    {
        private static void Main()
        {
            int w = 0; // w0 = 0
            int x = 1; // x0 = 1
            int y = w + x; // y0 = w0 + x0
            int z = y; // z0 = y0
            if (z != 0) z = z + 1; // if (z0 != 0) z1 = z0 + 1;
            z = z + 1; // z3 = (z2 = phi(z0, z1)) + 1

            //switch (z) // switch (z3)
            //{
            //    case 0: y = 0; break; // y1 = 0
            //    case 1: y = 1; break; // y2 = 1
            //    case 2: y = 2; break; // y3 = 2
            //    default: y = 3; break; // y4 = 3
            //}

            //z = y; // z4 = (y5 = phi(y1, y2, y3, y4))

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

            
        //    bool a = true; // 0 - 0
        //    bool b = false;
        //    int x = 1;
        //TestLabel: // 1 - 0
        //    if (a)
        //    {
        //        x = 0; // 2 - 1
        //        if (x == 0)
        //        {
        //            b = true; // 3 - 2
        //            // stloc.0
        //            // br.s LoopCondition
        //            // LoopBody // 4 - 12
        //            // beq x--
        //            // ld b // 5 - 4
        //            // brfalse Load30
        //            // ld 92 // 6 - 5
        //            // br.s AfterLoad
        //            // Load30: ld 30 // 7 - 5
        //            // AfterLoad:
        //            // dup // 8 - 6
        //            // stloc x
        //            // ld 0
        //            // beq x++
        //            // ld b // 9 - 8
        //            // brfalse x++
        //            // x-- // 10 - 4
        //            // x++ // 11 - 8
        //            // i34++
        //            // LoopCondition // 12 - 3
        //            // brtrue.s LoopBody
        //            for (int i34 = 0; i34 < 99; i34++)
        //            {
        //                if (i34 == 8 || ((x = (b ? 92 : 30)) != 0 && b))
        //                {
        //                    x--;
        //                }
        //                x++;
        //            }
        //            // 13 - 12
        //        }
        //        else
        //        {
        //            x = 0; // 14 - 2
        //            goto TestLabel;
        //        }
        //        x = 1; // 15 - 13
        //    }
        //    a = b; // 16 - 1


            //// Node 0 - Dominator 0
            //bool a = true; // a0 <- true
            //bool b = false; // b0 <- true
            //if (a) // a0 -> condition
            //{
            //    // Node 1 - Dominator 0
            //    a = false; // a1 <- false
            //}
            //else
            //{
            //    // Node 2 - Dominator 0
            //    if (b) // b0 -> condition
            //    {
            //        // Node 3 - Dominator 2
            //        b = false; // b1 <- false
            //    }
            //    else
            //    {
            //        // Node 4 - Dominator 2
            //        if (a && // a0 -> condition
            //            // Node 5 - Dominator 4
            //            !b) // b0 -> condition
            //        {
            //            // Node 6 - Dominator 5
            //            b = true; // b2 <- true
            //            a = false; // a2 <- false
            //        }
            //        // Node 7 - Dominator 4
            //        // a5 <- phi(a0, a0, a2)
            //        // b4 <- phi(b0, b0, b2)
            //        b = true; // b3 <- true
            //    }
            //    // Node 8 - Dominator 2
            //    // a6 <- phi(a0, a5)
            //    // b5 <- phi(b1, b3)
            //    a = true; // a3 <- true
            //}
            //// Node 9 - Dominator 0
            //// a7 <- phi(a0, a3)
            //// b6 <- phi(b0, b5)
            //a = b; // a4 <- b6

            //// Node 0 - Dominator 0
            //int x = 0;
            //// Node 2 - Dominator 0
            //while (x < 10)
            //{
            //    // Node 1 - Dominator 2
            //    // stackLocal1 <- x
            //    // Nop
            //    // stackLocal1 <- stackLocal1 + 1
            //    // x <- stackLocal1
            //    x = x + 1;
            //}
            //// Node 3 - Dominator 2
            //int y = x;
            //++y;
            ////x = y;
        }
    }
}
