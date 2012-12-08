namespace System
{
    public sealed unsafe class RuntimeType : Type
    {
        internal RuntimeType(RuntimeTypeHandle handle) { mHandle = handle; }

        public override Type BaseType
        {
            get { throw new NotImplementedException(); }
        }

        public override bool IsEnum
        {
            get { return GetTypeDataPointer()->EnumerationBaseType != null; }
        }

        public override string Namespace
        {
            get { return new string(GetTypeDataPointer()->Namespace, 0, GetTypeDataPointer()->NamespaceLength); }
        }

        public override string Name
        {
            get { return new string(GetTypeDataPointer()->Name, 0, GetTypeDataPointer()->NameLength); }
        }

        public override string FullName
        {
            get { return Namespace + "." + Name; }
        }

        public override bool IsGenericType
        {
            get { throw new NotImplementedException(); }
        }

        public override Type GetGenericTypeDefinition() { throw new NotImplementedException(); }

        public override Type[] GetGenericArguments() { throw new NotImplementedException(); }
    }
}
