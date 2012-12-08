namespace System
{
    public struct RuntimeFieldHandle
    {
        private IntPtr mValue;

        internal RuntimeFieldHandle(IntPtr v) { mValue = v; }

        public IntPtr Value { get { return mValue; } }
    }
}
