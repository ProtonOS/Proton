using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.CompilerServices;

namespace Mernel
{
    public sealed class TestPluggedType
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void TestMethod();
    }
}
