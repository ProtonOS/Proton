namespace System
{
    public abstract class Delegate
    {
        private object mTargetObj = null;
        private IntPtr mTargetMethod = IntPtr.Zero;
        protected Delegate mNext = null;

        public override bool Equals(object obj)
        {
            Delegate d = obj as Delegate;
            if (d == null)
            {
                return false;
            }
            return d.mTargetObj == mTargetObj && d.mTargetMethod.Equals(mTargetMethod);
        }

        public override int GetHashCode()
        {
            int ret = mTargetMethod.GetHashCode();
            if (mTargetObj != null)
            {
                ret ^= mTargetObj.GetHashCode();
            }
            return ret;
        }

        public static Delegate Combine(Delegate a, Delegate b)
        {
            if (a == null)
            {
                return b;
            }
            else if (b == null)
            {
                return a;
            }

            if (a.GetType() != b.GetType())
            {
                throw new ArgumentException("Incompatible delegate types");
            }

            return a.CombineImpl(b);
        }

        protected virtual Delegate CombineImpl(Delegate d)
        {
            throw new MulticastNotSupportedException();
        }

        public static Delegate Remove(Delegate source, Delegate value)
        {
            if (source == null)
            {
                return null;
            }
            return source.RemoveImpl(value);
        }

        protected virtual Delegate RemoveImpl(Delegate d)
        {
            if (d.Equals(this))
            {
                return null;
            }
            return this;
        }
    }
}
