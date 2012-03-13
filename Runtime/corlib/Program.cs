using System;
using System.Runtime.CompilerServices;

namespace Mernel
{
    internal static class Program
    {
        private static long Blarh342()
        {
            return 42;
        }

        private static long aStatic;

        public static void Main()
        {
            long ticks = DateTime.InternalUtcNow();
            Console.WriteLine("Started @ " + ((ulong)ticks).ToString());
            string str = new string('*', 40);
            Console.WriteLine(str);

            //aStatic = Blarh342();// unchecked((uint)-1);
            //uint bStatic = (uint)(aStatic);
            //Console.WriteLine("aStatic = " + ((ulong)aStatic).ToString());

            //ulong boxTest = 4;
            //object boxedTest = boxTest;
            //Console.WriteLine("Unboxed Test = " + ((ulong)boxedTest).ToString());

            //ulong blargh = 0x000000FF0000FFFF;
            //Console.WriteLine("Blargh! " + ((uint)blargh).ToString());
            //uint[] uarr = new uint[3];
            //uarr[2] = 93;
            //Console.WriteLine("ElementTest = " + uarr[2].ToString());
            //Test2[] tarr = new Test2[9];
            //tarr[0] = new Test2();
            //Console.WriteLine("Test2 = " + tarr[0].ToString());
            //object[] objarr = new object[2];
            //objarr[0] = new Test2();
            //Console.WriteLine("Test2 AsObj = " + objarr[0].ToString());
            //char[] strArr = new char[4];
            //strArr[0] = 'T';
            //strArr[1] = 'E';
            //strArr[2] = 'S';
            //strArr[3] = 'T';
            //string str2 = new string(strArr);
            //Console.WriteLine("strArr .ctor = " + str2);
            //Test2 t = new Test2();
            //t.Really();
            //t.JustATest();
            //Console.WriteLine("Factorial of 6 (Via Multiplication): " + Factorial(6).ToString());
            //Console.WriteLine("Mernel Started");

            InterfaceTest iface = new InterfaceTestImpl();
            iface.TestMe();


            ticks = DateTime.InternalUtcNow();
            Console.WriteLine("Finished @ " + ((uint)ticks).ToString());
        }

        private static void ActionTest() { Console.WriteLine("ActionTest"); }

        private static uint Factorial(uint val)
        {
            return (val == 1) ? val : val * Factorial(val - 1);
        }
    }

    internal interface InterfaceTest
    {
        void TestMe();
    }

    internal class InterfaceTestImpl : InterfaceTest
    {
        public void TestMe()
        {
            Console.WriteLine("Hey lookie! Interfaces, they work!");
        }
    }

    internal abstract class Test
    {
        /// <summary>
        /// Run the test and return the result.
        /// </summary>
        /// <returns>False if the test failed.</returns>
        public abstract bool RunTest();

        public void TestFailed(string msg)
        {
            Console.WriteLine("Test FAILED! '" + msg + "'");
        }
    }

    #region TestRunner
    internal static class TestRunner
    {
        public static Test[] Tests = new Test[]
        {
            new SwitchTest(),
            new BobFindPeterTest(),
        };

        public static void RunTests()
        {

        }
    }
    #endregion

    #region Tests

    #region Switch Test
    /// <summary>
    /// This test checks that switch
    /// statements work.
    /// </summary>
    internal class SwitchTest : Test
    {
        public override bool RunTest()
        {
            if (DoSwitchTest(1) != 9)
            {
                TestFailed("Switch test 1 failed!");
                return false;
            }
            if (DoSwitchTest(2) != 5)
            {
                TestFailed("Switch test 3 failed!");
                return false;
            }
            if (DoSwitchTest(0) != 3)
            {
                TestFailed("Switch test 3 failed!");
                return false;
            }
            return true;
        }

        private static uint DoSwitchTest(uint switchVal)
        {
            switch (switchVal)
            {
                case 1: return 9;
                case 2: return 5;
                default: return 3;
            }
        }
    }
    #endregion

    #region Bob Find Peter Test
    /// <summary>
    /// This is mostly a test
    /// to make sure that the 
    /// stack isn't borked when
    /// calling a method, but is
    /// also a decent benchmark as
    /// well.
    /// </summary>
    internal class BobFindPeterTest : Test
    {
        public void FindPeter()
        {
            Console.Write("Looking for Peter");
            for (uint i2 = 0; i2 < 5; i2++)
            {
                Console.Write(".");
                for (uint i = 0; i < 20000000; i++) ;
            }
            Console.WriteLine("");
            Console.WriteLine("Failed to find Peter.");
        }

        public void HeWasLate()
        {
            Console.WriteLine("I found Peter, but I don't need him anymore....");
        }

        public override bool RunTest()
        {
            FindPeter();
            HeWasLate();
            return true;
        }
    }
    #endregion

    #endregion

    internal sealed class Test2
    {
        uint A;

        public Test2()
        {
            Console.WriteLine("Creating a new Test object.");
            A = 4;
            Console.WriteLine("Finished Creating a new Test object.");
        }

        public void Really()
        {
            Console.WriteLine("Hey look! It works!");
        }

        public void JustATest()
        {
            Console.WriteLine("Value of A: " + A.ToString());
        }

        //public override string ToString()
        //{
        //    return "It Says hello!";
        //}
    }
}