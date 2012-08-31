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
    public struct Tester
    {
        public uint a;
        public uint b;
        public uint c;

        public Tester(uint pA, uint pB, uint pC)
        {
            Kernel.Write(pA);
            Kernel.Write(pB);
            Kernel.Write(pC);
            a = pA; b = pB; c = pC;
            Kernel.Write(a);
            Kernel.Write(b);
            Kernel.Write(c);
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
        public static extern void Write(uint pValue);


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
            //Kernel.Write("Test\n");

            //char[] test = new char[] { 'T', 'E', 'S', 'T', '\n' };
            //Kernel.Write(new string(test));

            //string x = "Test1,Test2\n";
            //string[] y = x.Split(',');
            //Kernel.Write(y[1]);
            //Kernel.Write("Test\n");

            //TestPluggedType.TestMethod();

            //string x = "Test";
            //Kernel.Write(x + "\n");
            Tester[] test = new Tester[3];
            //test[0] = new Tester(1, 2, 3);
            //test[1] = new Tester(4, 5, 6);
            test[2] = new Tester(7, 8, 9);
            //List<Tester> test = new List<Tester>();
            //test.Add(new Tester(1, 2, 3));
            //test.Add(new Tester(4, 5, 6));
            //test.Add(new Tester(7, 8, 9));
            if (test[2].c == 9) Kernel.Write("Yay\n");
            Kernel.Write(test[2].a);
            Kernel.Write(test[2].b);
            Kernel.Write(test[2].c);
            Kernel.Write("Hello\n");
        }
    }

    internal class List<T>
    {
        private T[] internalArr;
        public List() { }

        public void Add(T val)
        {
            if (internalArr == null)
            {
                internalArr = new T[1];
                internalArr[0] = val;
            }
            else
            {
                T[] tmp = new T[internalArr.Length + 1];
                for (int i = 0; i < internalArr.Length; i++)
                {
                    tmp[i] = internalArr[i];
                }
                tmp[internalArr.Length] = val;
                internalArr = tmp;
            }
        }

        public T this[int idx]
        {
            get
            {
                return internalArr[idx];
            }
        }
    }
}
