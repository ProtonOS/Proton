using System;
using System.Runtime.CompilerServices;

namespace Mernel
{
    internal static class Program
    {
        private static ulong Blarh342()
        {
            return 42;
        }

        private static void Main()
        {
            long ticks = DateTime.InternalUtcNow();
            Console.WriteLine("Started @ " + ((uint)ticks).ToString());
            //ulong blargh = 0x000000FF0000FFFF;
            //Console.WriteLine("Blargh! " + ((uint)blargh).ToString());
            //char[] @char = new char[4];
            //uint[] uarr = new uint[3];
            //uarr[2] = 93;
            //Console.WriteLine(uarr[2].ToString());
            //Test[] tarr = new Test[9];
            //tarr[0] = new Test();
            //Console.WriteLine(tarr[0].ToString());
            //object[] objarr = new object[2];
            //objarr[0] = new Test();
            //Console.WriteLine(objarr[0].ToString());
            //uint ch = (uint)objarr[0];
            //@char[0] = 'T';
            //@char[1] = 'E';
            //@char[2] = 'S';
            //@char[3] = 'T';
            //string str2 = new string(@char);
            //string str = new string('*', 40);
            //Console.WriteLine(str);
            //Console.WriteLine(str2);
            //Test t = new Test();
            //t.Really();
            //t.JustATest();
            //Console.WriteLine("Factorial of 6 (Via Multiplication): " + Factorial(6).ToString());
            //Console.WriteLine("Mernel Started");
            

            ticks = DateTime.InternalUtcNow();
            Console.WriteLine("Finished @ " + ((uint)ticks).ToString());
        }

        private static void ActionTest() { Console.WriteLine("ActionTest"); }

        private static uint Factorial(uint val)
        {
            return (val == 1) ? val : val * Factorial(val - 1);
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
    public static class TestRunner
    {
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