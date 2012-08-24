using System;
using Proton.Plugs;

namespace Mernel
{
    [PlugType(typeof(TestPluggedType))]
    public sealed class TestManagedPlug
    {
        [PlugMethod]
        public static void TestMethod()
        {
            Kernel.Write("Managed plugs work.... Now isn't that something.\n");
        }
    }
}
