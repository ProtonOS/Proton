using System.Runtime.CompilerServices;

namespace System
{
    public abstract class ValueType
    {
        protected ValueType() { }

        internal static new bool Equals(object objA, object objB)
        {
            if (objA == null || objB == null || objA.GetType() != objB.GetType()) return false;
            object[] objAFields = objA.GetFields();
            object[] objBFields = objB.GetFields();
            int len = objAFields.Length;
            for (int i = 0; i < len; ++i)
            {
                if (!object.Equals(objAFields[i], objBFields[i]))
                {
                    return false;
                }
            }
            return true;
        }

        public override bool Equals(object obj) { return Equals(this, obj); }

        internal static new int GetHashCode(object obj)
        {
            object[] fields = obj.GetFields();

            int hash = 0;
            int len = fields.Length;
            for (int i = 0; i < len; i++)
            {
                hash ^= fields[i].GetHashCode();
            }
            return hash;
        }

        public override int GetHashCode() { return GetHashCode(this); }
    }
}
