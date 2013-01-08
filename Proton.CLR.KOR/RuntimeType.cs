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
			get { return GC.AllocateStringFromASCII(GetTypeDataPointer()->Namespace, (uint)GetTypeDataPointer()->NamespaceLength); }
		}

		public override string Name
		{
			get { return GC.AllocateStringFromASCII(GetTypeDataPointer()->Name, (uint)GetTypeDataPointer()->NameLength); }
		}

		public override string FullName
		{
			get { return Namespace + "." + Name; }
		}

		public override bool IsGenericType { get { return GetTypeDataPointer()->IsGenericType; } }

		public override Type GetGenericTypeDefinition() { throw new NotImplementedException(); }

		public override Type[] GetGenericArguments() { throw new NotImplementedException(); }
	}
}
