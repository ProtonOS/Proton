namespace System
{
    public abstract class MulticastDelegate : Delegate
    {
        protected override Delegate CombineImpl(Delegate follow)
        {
            MulticastDelegate ret = (MulticastDelegate)object.MemberwiseClone(this);
            MulticastDelegate cur = ret;

            // Clone and add all the current delegate(s)
            for (MulticastDelegate del = (MulticastDelegate)mNext; del != null; del = (MulticastDelegate)del.mNext)
            {
                cur.mNext = (MulticastDelegate)object.MemberwiseClone(del);
                cur = (MulticastDelegate)cur.mNext;
            }

            // Add all the following delegate(s)
            cur.mNext = (MulticastDelegate)object.MemberwiseClone(follow);
            cur = (MulticastDelegate)cur.mNext;
            for (MulticastDelegate del = (MulticastDelegate)((MulticastDelegate)follow).mNext; del != null; del = (MulticastDelegate)del.mNext)
            {
                cur.mNext = (MulticastDelegate)object.MemberwiseClone(del);
                cur = (MulticastDelegate)cur.mNext;
            }
            cur.mNext = null;

            return ret;
        }

        protected override Delegate RemoveImpl(Delegate d)
        {
            MulticastDelegate ret = null, cur = null;

            for (MulticastDelegate del = this; del != null; del = (MulticastDelegate)del.mNext)
            {
                // Miss out the one we're removing
                if (!del.Equals(d))
                {
                    if (ret == null)
                    {
                        ret = (MulticastDelegate)object.MemberwiseClone(del);
                        cur = ret;
                    }
                    else
                    {
                        cur.mNext = (MulticastDelegate)object.MemberwiseClone(del);
                        cur = (MulticastDelegate)cur.mNext;
                    }
                }
            }
            if (cur != null)
            {
                cur.mNext = null;
            }

            return ret;
        }

    }
}
