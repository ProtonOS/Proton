using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;

namespace Mernel
{
    //public enum TestEnum
    //{
    //    TestValue1,
    //    TestValue2,
    //}

    //[Test(1, 2, 3, 4, 5, 6, 7, 8, "Hello World", typeof(Kernel), TestEnum.TestValue1, new int[] { 0, 1, 2, 3, 4 }, new Type[] { typeof(AttributeTest), typeof(Kernel) }, new TestEnum[] { TestEnum.TestValue1, TestEnum.TestValue2 })]
    //public class AttributeTest
    //{

    //}

    //[AttributeUsage(AttributeTargets.All)]
    //public sealed class TestAttribute : Attribute
    //{
    //    public TestAttribute(byte pByteVal, sbyte pSByteVal, ushort pUShortVal, short pShortVal, uint pUIntVal, int pIntVal, ulong pULongVal, long pLongVal, string pStringVal, Type pTypeVal, TestEnum pEnumVal, int[] pIntArrayVal, Type[] pTypeArrayVal, TestEnum[] pEnumArrayVal)
    //    {

    //    }
    //}

    public static class Kernel
    {
        //public static readonly byte[] Values = new byte[]
        //{
        //    1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19
        //};

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void Write(string pString);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void Write(uint pValue);

        private unsafe static void Main()
        {
            Startup();
        }

        private static void Startup()
        {
        }
    }

    //internal class List<T>
    //{
    //    private T[] internalArr;
    //    public List() { }

    //    public void Add(T val)
    //    {
    //        if (internalArr == null)
    //        {
    //            internalArr = new T[1];
    //            internalArr[0] = val;
    //        }
    //        else
    //        {
    //            T[] tmp = new T[internalArr.Length + 1];
    //            for (int i = 0; i < internalArr.Length; i++)
    //            {
    //                tmp[i] = internalArr[i];
    //            }
    //            tmp[internalArr.Length] = val;
    //            internalArr = tmp;
    //        }
    //    }

    //    public T this[int idx]
    //    {
    //        get
    //        {
    //            return internalArr[idx];
    //        }
    //    }
    //}
}
