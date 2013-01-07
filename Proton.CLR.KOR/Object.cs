using System.Runtime.CompilerServices;

namespace System
{
    public class Object
    {
        public Object() { }

        ~Object() { }

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern unsafe void* Internal_ReferenceToPointer();

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static unsafe object Internal_PointerToReference(void* obj);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static unsafe void Internal_FastCopy(void* source, void* dest, int size);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static unsafe void Internal_FastZero(void* data, int size);

        internal static unsafe Type GetType(object obj)
        {
            void* objectPtr = obj.Internal_ReferenceToPointer();
            void** typeData = (void**)((byte*)objectPtr - sizeof(void*));
            return new RuntimeType(new RuntimeTypeHandle(new IntPtr(*typeData)));
        }

        internal unsafe object[] GetFields()
        {
            Type.TypeData* typeData = GetType().GetTypeDataPointer();
            object[] fields = new object[typeData->FieldCount];
            for (int i = 0; i < typeData->FieldCount; ++i)
            {
                // TODO: Create new object using typeData.Fields[i].Type, and then byte copy the field data
            }
            return fields;
        }

        public Type GetType() { return GetType(this); }

        public virtual bool Equals(object obj)
        {
            if (obj == null) return false;
            Type thisType = GetType();
            Type objType = obj.GetType();
            if (!thisType.Equals(objType)) return false;
            if (!thisType.IsValueType) return ReferenceEquals(this, obj);
            return ValueType.Equals(this, obj);
        }

        internal static unsafe int GetHashCode(object obj)
        {
            if (!obj.GetType().IsValueType) return (int)obj.Internal_ReferenceToPointer();
            return ValueType.GetHashCode(obj);
        }

        public virtual int GetHashCode() { return GetHashCode(this); }

        public virtual string ToString() { return GetType().FullName; }

        public static bool Equals(object objA, object objB)
        {
            if (objA == objB) return true;
            if (objA == null || objB == null) return false;
            return objA.Equals(objB);
        }

        public static unsafe bool ReferenceEquals(object objA, object objB)
        {
            if (objA == null || objB == null) return false;
            return objA.Internal_ReferenceToPointer() == objB.Internal_ReferenceToPointer();
        }

        internal static object MemberwiseClone(object obj)
        {
            return obj.MemberwiseClone();
        }

        protected object MemberwiseClone()
        {
            Type type = GetType();
            // TODO: internal void GC.AllocateObject(type), then copy using Pointers because reference-type fields are pointers and can be copied bit for bit like value types
            return null;
        }
    }
}
