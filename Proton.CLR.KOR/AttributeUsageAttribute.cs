namespace System
{
    [AttributeUsage(AttributeTargets.Class)]
    public sealed class AttributeUsageAttribute : Attribute
    {
        private AttributeTargets mValidOn;
        private bool mAllowMultiple = false;
        private bool mInherited = true;

        public AttributeUsageAttribute(AttributeTargets validOn) { mValidOn = validOn; }

        public bool AllowMultiple { get { return mAllowMultiple; } set { mAllowMultiple = value; } }

        public bool Inherited { get { return mInherited; } set { mInherited = value; } }

        public AttributeTargets ValidOn { get { return mValidOn; } }
    }
}
