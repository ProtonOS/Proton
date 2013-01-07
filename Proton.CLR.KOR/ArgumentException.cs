namespace System
{
    public class ArgumentException : SystemException
    {
        private string mParamName;

        public ArgumentException() : base("An invalid argument was specified.") { }

        public ArgumentException(string message) : base(message) { }

        public ArgumentException(string message, Exception innerException) : base(message, innerException) { }

        public ArgumentException(string message, string paramName) : base(message) { mParamName = paramName; }

        public virtual string ParamName { get { return mParamName; } }

        public override string Message
        {
            get
            {
                string baseMessage = base.Message;
                if (baseMessage == null)
                {
                    baseMessage = "An invalid argument was specified.";
                }
                if (mParamName == null)
                {
                    return baseMessage;
                }
                else
                {
                    return baseMessage + Environment.NewLine + "Parameter name: " + mParamName;
                }
            }
        }
    }
}
