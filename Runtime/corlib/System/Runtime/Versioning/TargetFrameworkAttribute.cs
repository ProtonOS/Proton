namespace System.Runtime.Versioning
{
    [AttributeUsageAttribute(AttributeTargets.Assembly, AllowMultiple = false, Inherited = false)]
    public sealed class TargetFrameworkAttribute : Attribute
    {
        private string _frameworkName;
        public string FrameworkName
        {
            get { return _frameworkName; }
            private set { _frameworkName = value; }
        }

        private string _frameworkDisplayName;
        public string FrameworkDisplayName
        {
            get { return _frameworkDisplayName; }
            set { _frameworkDisplayName = value; }
        }

        public TargetFrameworkAttribute(string frameworkName)
        {
            if (frameworkName == null)
                throw new ArgumentNullException("frameworkName");

            this._frameworkName = frameworkName;
        }
    }
}