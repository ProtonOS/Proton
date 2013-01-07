namespace System
{
    public struct Decimal
    {
        private uint mFlags;
        private uint mHigh;
        private uint mLow;
        private uint mMiddle;

        public Decimal(int[] bits)
        {
            if (bits == null) throw new ArgumentNullException("bits is a null reference");
            if (bits.Length != 4) throw new ArgumentException("bits does not contain four values");
            mLow = (uint)bits[0];
            mMiddle = (uint)bits[1];
            mHigh = (uint)bits[2];
            mFlags = (uint)bits[3];
        }

        public static int[] GetBits(Decimal d) { return new int[] { (int)d.mLow, (int)d.mMiddle, (int)d.mHigh, (int)d.mFlags }; }
    }
}
