#pragma warning disable 0649

using System.Reflection;
using System.Runtime.CompilerServices;

namespace System
{
    public abstract class Type : MemberInfo
    {
        internal unsafe struct TypeData
        {
            public sbyte* Namespace;
            public int NamespaceLength;
            public sbyte* Name;
            public int NameLength;
            public void** VirtualMethodTable;
            //public MemberAttributes Attributes;
            public uint Size;
            public FieldData* Fields;
            public int FieldCount;
            public StaticFieldData* StaticFields;
            public int StaticFieldCount;
            public TypeData* EnumerationBaseType;
            public EnumerationData* Enumerations;
            public int EnumerationCount;
            public TypeData* ArrayElementType;
        }
        internal unsafe struct FieldData
        {
            public TypeData* Type;
            public int Offset;
        }
        internal unsafe struct StaticFieldData
        {
            public TypeData* Type;
            public void* Data;
        }
        internal unsafe struct EnumerationData
        {
            public sbyte* Name;
            public int NameLength;
            public int Value;
        }

        public static readonly Type[] EmptyTypes = new Type[0];

        internal RuntimeTypeHandle mHandle;

        protected Type() { }

        public static Type GetTypeFromHandle(RuntimeTypeHandle handle) { return new RuntimeType(handle); }

        internal unsafe TypeData* GetTypeDataPointer() { return (TypeData*)mHandle.Value.ToPointer(); }

        public abstract Type BaseType
        {
            get;
        }

        public abstract bool IsEnum
        {
            get;
        }

        public abstract string Namespace
        {
            get;
        }

        public abstract string FullName
        {
            get;
        }

        public abstract bool IsGenericType
        {
            get;
        }

        public abstract Type GetGenericTypeDefinition();

        public abstract Type[] GetGenericArguments();

        public extern bool IsValueType
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            get;
        }

        public bool Equals(Type o) { return mHandle.Value == o.mHandle.Value; }

        public override string ToString()
        {
            return FullName;
        }
    }
}
