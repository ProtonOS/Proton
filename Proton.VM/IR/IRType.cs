using System;
using System.Linq;
using System.Collections.Generic;

namespace Proton.VM.IR
{
    public sealed class IRType
    {
        public static readonly List<IRType> VarPlaceholders = new List<IRType>();
        public static readonly List<IRType> MVarPlaceholders = new List<IRType>();

        public static IRType GetVarPlaceholder(uint pIndex)
        {
            while (pIndex + 1 >= VarPlaceholders.Count) VarPlaceholders.Add(new IRType() { IsTemporaryVar = true, TemporaryVarOrMVarIndex = (uint)VarPlaceholders.Count });
            return VarPlaceholders[(int)pIndex];
        }

        public static IRType GetMVarPlaceholder(uint pIndex)
        {
            while (pIndex + 1 >= MVarPlaceholders.Count) MVarPlaceholders.Add(new IRType() { IsTemporaryMVar = true, TemporaryVarOrMVarIndex = (uint)MVarPlaceholders.Count });
            return MVarPlaceholders[(int)pIndex];
        }

        public IRAssembly Assembly = null;

        public string Namespace = null;
        public string Name = null;

        public readonly List<IRField> Fields = new List<IRField>();
        public readonly List<IRMethod> Methods = new List<IRMethod>();
        public readonly List<IRType> NestedTypes = new List<IRType>();
        public IRType BaseType = null;

        private bool mGlobalTypeIDSet = false;
        private int mGlobalTypeID;
        public int GlobalTypeID
        {
            get
            {
                if (!mGlobalTypeIDSet)
                    throw new Exception("This type doesn't have a global type id!");
                return mGlobalTypeID;
            }
            set
            {
                if (mGlobalTypeIDSet)
                    throw new Exception("Cannot set the global type id more than once!");
                mGlobalTypeID = value;
                mGlobalTypeIDSet = true;
            }
        }

        private bool mIsValueType = false;
        public bool IsValueType { get { return mIsValueType; } }

        public bool IsEnumType { get { return BaseType == Assembly.AppDomain.System_Enum; } }
        
        // We can cache this because the state
        // of the cache doesn't extend to any
        // derived type.
        private bool? mResolvedCache;
		private bool mResolving = false;
        /// <summary>
        /// True if this type and it's members are fully
        /// resolved, aka. this type has been fully instantiated.
        /// </summary>
        public bool Resolved
        {
            get
            {
				if (mResolvedCache != null)
					return mResolvedCache.Value;
				
				if (IsTemporaryVar) return false;
				if (IsTemporaryMVar) return false;
				if (UnmanagedPointerType != null && !UnmanagedPointerType.Resolved) return false;
				if (ManagedPointerType != null && !ManagedPointerType.Resolved) return false;
				if (ArrayElementType != null && !ArrayElementType.Resolved) return false;
				if (!GenericParameters.Resolved) return false;
				if (mResolving)
					return true;

				mResolving = true;
				if (GenericType != null && (Fields.Count != GenericType.Fields.Count || Methods.Count != GenericType.Methods.Count)) return false;
                if (BaseType != null && !BaseType.Resolved) return false;
                if (!Fields.Where(f => f.Type != this).TrueForAll(f => f.Resolved)) return false;
                if (!Methods.TrueForAll(m => m.Resolved)) return false;
				mResolving = false;

                mResolvedCache = true;
				Assembly.AppDomain.Types.Add(this);
                return true;
            }
        }

        // Dynamic Types
        public bool IsTemporaryVar = false;
        public bool IsTemporaryMVar = false;
        public uint TemporaryVarOrMVarIndex = 0;

        private bool? mIsGenericCache;
        /// <summary>
        /// True if this type is generic, this is still true
        /// even after all generic parameters have been resolved.
        /// </summary>
        public bool IsGeneric
        {
            get 
            {
                if (mIsGenericCache != null)
                    return mIsGenericCache.Value;
				bool isGenericParameter = GenericParameters.Count > 0 || IsTemporaryVar || IsTemporaryMVar;
				bool isGenericArray = ArrayElementType != null && ArrayElementType.IsGeneric;
				bool isGenericPointer = ManagedPointerType != null && ManagedPointerType.IsGeneric;
				mIsGenericCache = isGenericParameter || isGenericArray || isGenericPointer;
				if (isGenericArray) { }

				//mIsGenericCache =
				//    GenericParameters.Count > 0 ||
				//    IsTemporaryVar ||
				//    IsTemporaryMVar ||
				//    (
				//        ArrayElementType != null &&
				//        ArrayElementType.IsGeneric
				//    ) ||
				//    (
				//        ManagedPointerType != null &&
				//        ManagedPointerType.IsGeneric
				//    )
				//;

                return mIsGenericCache.Value; 
            }
        }
		private IRType mGenericType = null;
		public IRType GenericType
		{
			get { return mGenericType; }
			set
			{
				if (value == null) throw new Exception();
				mGenericType = value;
			}
		}
        public readonly IRGenericParameterList GenericParameters = new IRGenericParameterList();

		public IRType UnmanagedPointerType = null;
        public bool IsUnmanagedPointerType { get { return UnmanagedPointerType != null; } }

		public IRType ManagedPointerType = null;
		public bool IsManagedPointerType { get { return ManagedPointerType != null; } }

        public IRType ArrayElementType = null;
        public bool IsArrayType { get { return ArrayElementType != null; } }

		private static int GLOBALTYPEITEDSTES = 0;
		private readonly int mTrueGlobalTypeID;
		private IRType()
		{
			mTrueGlobalTypeID = GLOBALTYPEITEDSTES++;

		}

        public IRType(IRAssembly pAssembly) : this() { Assembly = pAssembly; }

        /// <summary>
        /// This creates a shallow copy of this <see cref="IRType"/>.
        /// </summary>
        /// <returns>The new IRType.</returns>
        public IRType Clone()
        {
            IRType t = new IRType(this.Assembly);

            this.Fields.ForEach(f => t.Fields.Add(f.Clone(t)));
            this.Methods.ForEach(m => t.Methods.Add(m.Clone(t)));
            t.NestedTypes.AddRange(this.NestedTypes);
            t.GenericParameters.AddRange(this.GenericParameters);

            t.ArrayElementType = this.ArrayElementType;
            t.BaseType = this.BaseType;
			if (this.GenericType != null)
				t.GenericType = this.GenericType;

            t.IsTemporaryMVar = this.IsTemporaryMVar;
            t.IsTemporaryVar = this.IsTemporaryVar;
            t.Name = this.Name;
            t.Namespace = this.Namespace;
			t.ManagedPointerType = this.ManagedPointerType;
            t.UnmanagedPointerType = this.UnmanagedPointerType;
            t.TemporaryVarOrMVarIndex = this.TemporaryVarOrMVarIndex;

            return t;
        }

        public static readonly Dictionary<IRType, IRType> GenericTypes = new Dictionary<IRType, IRType>();
        /// <summary>
        /// Resolve any generic types used in this type.
        /// </summary>
        /// <param name="selfReference"></param>
        /// <param name="typeParams"></param>
        /// <param name="methodParams"></param>
        public void Resolve(ref IRType selfReference, IRGenericParameterList typeParams, IRGenericParameterList methodParams)
        {
            if (!Resolved)
            {
				if (IsGeneric)
				{
					if (IsTemporaryVar)
					{
						selfReference = typeParams[this.TemporaryVarOrMVarIndex];
					}
					else if (IsTemporaryMVar)
					{
						selfReference = methodParams[this.TemporaryVarOrMVarIndex];
					}
					else if (IsArrayType)
					{
						IRType elemType = ArrayElementType;
						elemType.Resolve(ref elemType, typeParams, methodParams);
						selfReference = Assembly.AppDomain.GetArrayType(elemType);
					}
					else if (IsManagedPointerType)
					{
						IRType typePointedTo = ManagedPointerType;
						typePointedTo.Resolve(ref typePointedTo, typeParams, methodParams);
						selfReference = Assembly.AppDomain.GetManagedPointerType(typePointedTo);
					}
					else
					{
						if (!this.GenericParameters.Resolved && this.GenericType != null)
							this.GenericParameters.TrySubstitute(typeParams, methodParams);
						if (this.GenericParameters.Resolved)
						{
							IRType tp = null;
							if (!GenericTypes.TryGetValue(this, out tp))
							{
								IRType tp2 = null;
								tp = this.GenericType.Clone();
								tp.GenericParameters.Substitute(this.GenericParameters, methodParams);
								if (!GenericTypes.TryGetValue(tp, out tp2))
								{
									GenericTypes.Add(tp, tp);

									for (int i = 0; i < tp.GenericParameters.Count; i++)
									{
										tp.GenericParameters[i] = this.GenericParameters[i];
									}
									for (int i = 0; i < tp.Methods.Count; i++)
									{
										tp.Methods[i] = tp.Methods[i].Resolved ? tp.Methods[i] : tp.Methods[i].Clone(tp);
									}

									tp.Substitute(typeParams, methodParams);
								}
								else
								{
									tp = tp2;
								}
							}
							selfReference = tp;
						}
						else
						{
	#warning Need to do the rest of this resolution.
						}
					}
				}
				else
				{
					Substitute(IRGenericParameterList.Empty, IRGenericParameterList.Empty);
				}
            }
        }

        /// <summary>
        /// Resolves generic parameters within this type.
        /// </summary>
        /// <param name="typeParams"></param>
        /// <param name="methodParams"></param>
        public void Substitute(IRGenericParameterList typeParams, IRGenericParameterList methodParams)
        {
            this.GenericParameters.Substitute(typeParams, methodParams);

			if (!GenericParameters.Resolved)
				return;

			if (this.BaseType != null)
				this.BaseType.Resolve(ref this.BaseType, this.GenericParameters, IRGenericParameterList.Empty);

            this.Fields.ForEach(f => f.Substitute());
			this.Methods.ForEach(m => m.Substitute(m.GenericParameters));
        }


        private int? mHashCodeCache;
        public override int GetHashCode()
        {
            if (mHashCodeCache != null)
                return mHashCodeCache.Value;

            int res;
            if (this.IsTemporaryVar)
			{
				// 5th bit from the top set
				res = (int)this.TemporaryVarOrMVarIndex + 1;
            }
            else if (this.IsTemporaryMVar)
            {
                // Allow support for up to 256 generic type parameters before
                // hash collisions occur.
				res = (int)((this.TemporaryVarOrMVarIndex + 1) << 8);
			}
			else if (IsManagedPointerType)
			{
				// 4rd bit from the top set
				res = ManagedPointerType.GetHashCode() | unchecked((int)0x10000000);
			}
			else if (IsUnmanagedPointerType)
			{
				// 3rd bit from the top set
				res = UnmanagedPointerType.GetHashCode() | unchecked((int)0x20000000);
			}
            else if (IsArrayType)
            {
                // 2nd bit from the top set
                res = ArrayElementType.GetHashCode() | unchecked((int)0x40000000);
            }
            else
            {
                // The OR at the end is to ensure that this hash code can never conflict with
                // any of the above.
                // Top bit set
                res = Namespace.GetHashCode() ^ Name.GetHashCode() ^ GenericParameters.GetHashCode() | unchecked((int)0x80000000);
            }

            mHashCodeCache = res;
            return mHashCodeCache.Value;
        }

        public override bool Equals(object obj)
        {
            if (ReferenceEquals(this, obj)) return true;
            if (obj == null) return false;
            if (!(obj is IRType)) return false;
            return ((IRType)obj).GetHashCode() == this.GetHashCode();
        }

        public static bool operator ==(IRType a, IRType b)
        {
            if (ReferenceEquals(a, b)) return true;
            if (((object)a == null) || ((object)b == null)) return false;
            return a.GetHashCode() == b.GetHashCode();
        }

        public static bool operator !=(IRType a, IRType b) { return !(a == b); }

		public override string ToString()
		{
			if (IsTemporaryVar)
				return "T(" + TemporaryVarOrMVarIndex.ToString() + ")";
			if (IsTemporaryMVar)
				return "M(" + TemporaryVarOrMVarIndex.ToString() + ")";
			if (IsUnmanagedPointerType)
				return UnmanagedPointerType.ToString() + "*";
			if (IsManagedPointerType)
				return ManagedPointerType.ToString() + "&";
			if (IsArrayType)
				return ArrayElementType.ToString() + "[]";

			switch (Namespace)
			{
				case "System":
					switch (Name)
					{
						case "Boolean":
							return "bool";
						case "Byte":
							return "byte";
						case "SByte":
							return "sbyte";
						case "UInt16":
							return "ushort";
						case "Int16":
							return "short";
						case "UInt32":
							return "uint";
						case "Int32":
							return "int";
						case "UInt64":
							return "ulong";
						case "Int64":
							return "long";
						case "Single":
							return "float";
						case "Double":
							return "double";
						case "Decimal":
							return "decimal";
						case "String":
							return "string";
						case "Char":
							return "char";
						case "Void":
							return "void";
						case "Object":
							return "object";
						default:
							break;
					}
					break;
				default:
					break;
			}
			string nn = Namespace + "." + Name;
			if (IsGeneric)
			{
				nn += GenericParameters.ToString();
			}
			return nn;
		}

		public void Dump(IndentableStreamWriter pWriter)
		{
			pWriter.WriteLine("IRType #{0} {1} : {2}", mTrueGlobalTypeID, ToString(), BaseType);
			pWriter.WriteLine("{");
			pWriter.Indent++;
			Fields.ForEach(f => f.Dump(pWriter));
			Methods.ForEach(m => m.Dump(pWriter));
			pWriter.Indent--;
			pWriter.WriteLine("}");
		}
    }
}
