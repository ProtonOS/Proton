namespace System
{
    public struct RuntimeTypeHandle
    {
        private IntPtr mValue;

        internal RuntimeTypeHandle(IntPtr v) { mValue = v; }

        public IntPtr Value { get { return mValue; } }
    }
}
