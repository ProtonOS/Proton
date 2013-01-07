namespace System
{
    public unsafe struct IntPtr
    {
        public static readonly IntPtr Zero = new IntPtr(0);

        private void* mValue;

        public IntPtr(int value) { mValue = (void*)value; }

        public IntPtr(long value) { mValue = (void*)value; }

        public IntPtr(void* value) { mValue = value; }

        public static int Size { get { return sizeof(void*); } }

        public int ToInt32() { return (int)mValue; }

        public long ToInt64() { return (long)mValue; }

        public void* ToPointer() { return mValue; }

        public override bool Equals(object obj) { return (obj is IntPtr && ((IntPtr)obj).mValue == mValue); }

        public override int GetHashCode() { return (int)mValue; }

        public static bool operator ==(IntPtr a, IntPtr b) { return a.mValue == b.mValue; }

        public static bool operator !=(IntPtr a, IntPtr b) { return a.mValue != b.mValue; }

        public override string ToString()
        {
            if (Size == 4)
            {
                return string.Format("0x{0:x4}", (int)mValue);
            }
            return string.Format("0x{0:x8}", (long)mValue);
        }
    }
}
