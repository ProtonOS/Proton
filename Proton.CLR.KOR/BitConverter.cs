namespace System
{
    public static class BitConverter
    {
        public static readonly bool IsLittleEndian = AmILittleEndian();

        private static unsafe bool AmILittleEndian()
        {
            int i = 1;
            byte b = *((byte*)&i);
            return (b == 1);
        }

        public static unsafe long DoubleToInt64Bits(double value) { return *((long*)&value); }
    }
}
