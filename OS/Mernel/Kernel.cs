using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;

namespace Mernel
{

    public enum TestEnum
    {
        TestValue1,
        TestValue2,
    }

    [Test(1, 2, 3, 4, 5, 6, 7, 8, "Hello World", typeof(Kernel), TestEnum.TestValue1, new int[] { 0, 1, 2, 3, 4 }, new Type[] { typeof(AttributeTest), typeof(Kernel) }, new TestEnum[] { TestEnum.TestValue1, TestEnum.TestValue2 })]
    public class AttributeTest
    {

    }

    [AttributeUsage(AttributeTargets.All)]
    public sealed class TestAttribute : Attribute
    {
        public TestAttribute(byte pByteVal, sbyte pSByteVal, ushort pUShortVal, short pShortVal, uint pUIntVal, int pIntVal, ulong pULongVal, long pLongVal, string pStringVal, Type pTypeVal, TestEnum pEnumVal, int[] pIntArrayVal, Type[] pTypeArrayVal, TestEnum[] pEnumArrayVal)
        {

        }
    }
    public static class Kernel
    {
        public static readonly byte[] Values = new byte[]
        {
            1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19
        };

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void Write(string pString);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void Write(long pValue);

        private unsafe static void Main()
        {
            // TODO: Implement and test all string constructors
            //char[] test = new char[] { 'T', 'E', 'S', 'T', '\n' }; // TODO: Implement InitializeArray runtime helper support
            //char[] test = new char[5];
            //test[0] = 'T';
            //test[1] = 'E';
            //test[2] = 'S';
            //test[3] = 'T';
            //test[4] = '\n';
            //Kernel.Write(new string(test));
            //Kernel.Write(new string((sbyte*)0x1cdf37));
            //byte[] buf = new byte[3];
            //int bufLen = Buffer.ByteLength(buf);
            //if (bufLen == 3) Kernel.Write(bufLen.ToString());
            //int x = 5;
            //Kernel.Write(x.ToString());
            //char[] c = new char[4];
            //c[0] = 'T';
            //c[1] = 'W';
            //c[2] = 'F';
            //c[3] = 'u';
            ////string s = "TWFu";
            //byte[] b = Convert.FromBase64CharArray(c, 0, 4);
            //if (b[0] == 77) Kernel.Write("Yay!\n");
            //Kernel.Write(GC.GetTotalMemory(false));
            TestPluggedType.TestMethod();
            //Kernel.Write("Test\n");

            //char[] test = new char[] { 'T', 'E', 'S', 'T', '\n' };
            //Kernel.Write(new string(test));

            //string x = "Test1,Test2\n";
            //string[] y = x.Split(',');
            //Kernel.Write(y[1]);
            //Kernel.Write("Test\n");
            string x = "Test";
            Kernel.Write(x + "\n");

            //List<string> test = new List<string>();
            //test.Add("Hello!\n");
            //Kernel.Write(test[0]);

        }
    }
}
