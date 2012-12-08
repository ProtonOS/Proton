namespace System
{
    public class Exception
    {
        private Exception mInnerException;
        private string mMessage;
        private int mHResult = unchecked((int)0x80004005);

        public Exception() { }

        public Exception(string message) { mMessage = message; }

        public Exception(string message, Exception innerException)
        {
            mInnerException = innerException;
            mMessage = message;
        }

        public virtual string Message { get { return mMessage; } }

        public Exception InnerException { get { return mInnerException; } }

        protected int HResult
        {
            get { return mHResult; }
            set { mHResult = value; }
        }

        public override string ToString() { return mMessage ?? string.Empty; }
    }
}
