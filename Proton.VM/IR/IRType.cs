using System;
using System.Linq;
using System.Collections.Generic;
using Proton.Metadata;
using System.Text;

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
		public TypeAttributes Flags = TypeAttributes.None;
		public bool PresolvedType = false;
		public bool PostsolvedType = false;

		public bool IsEnumType { get { return BaseType == Assembly.AppDomain.System_Enum; } }
		public bool IsValueType { get { return BaseType == Assembly.AppDomain.System_ValueType || IsEnumType; } }

		public bool IsAbstract { get { return (Flags & TypeAttributes.Abstract) == TypeAttributes.Abstract; } }
		public bool IsInterface { get { return (Flags & TypeAttributes.Interface) == TypeAttributes.Interface; } }
		public bool IsStructure { get { return IsValueType && !IsEnumType; } }
		public bool IsClass { get { return !IsStructure && !IsInterface; } }
		public bool IsSealed { get { return (Flags & TypeAttributes.Sealed) == TypeAttributes.Sealed; } }
		public bool IsExplicitLayout { get { return (Flags & TypeAttributes.ExplicitLayout) == TypeAttributes.ExplicitLayout; } }

		public IRType UnmanagedPointerType = null;
		public bool IsUnmanagedPointerType { get { return UnmanagedPointerType != null; } }

		public IRType ManagedPointerType = null;
		public bool IsManagedPointerType { get { return ManagedPointerType != null; } }

		public IRType ArrayElementType = null;
		public bool IsArrayType { get { return ArrayElementType != null; } }
        
        public readonly List<IRField> Fields = new List<IRField>();
		private readonly List<IRType> mImplementedInterfaces = new List<IRType>();
		public List<IRType> ImplementedInterfaces
		{
			get
			{
				if (mParentType != null && mImplementedInterfaces.Count != mParentType.ImplementedInterfaces.Count)
				{
					mImplementedInterfaces.AddRange(mParentType.ImplementedInterfaces);
				}
				return mImplementedInterfaces;
			}
		}
		public sealed class OverrideDescriptor
		{
			public IRMethod Overridden;
			public IRMethod Overridder;

			private static int sTempID = 0;
			internal int mTempID = 0;

			public OverrideDescriptor(IRMethod pOverridden, IRMethod pOverridder)
			{
				Overridden = pOverridden;
				Overridder = pOverridder;
				mTempID = sTempID++;
			}

			public OverrideDescriptor Clone() { return new OverrideDescriptor(Overridden, Overridder); }

			public void Dump(IndentableStreamWriter pWriter)
			{
				pWriter.WriteLine("#{0} | {1} -> {2}", mTempID, Overridden.ToString(), Overridder.ToString());
			}
		}
		private readonly List<OverrideDescriptor> mExplicitOverrides = new List<OverrideDescriptor>();
		public List<OverrideDescriptor> ExplicitOverrides
		{
			get
			{
				if (mParentType != null && mExplicitOverrides.Count != mParentType.mExplicitOverrides.Count)
				{
					mParentType.mExplicitOverrides.ForEach(od => mExplicitOverrides.Add(od.Clone()));
				}
				return mExplicitOverrides;
			}
		}

		public sealed class IRMethodList : IEnumerable<IRMethod>
		{
			private readonly List<IRMethod> vals = new List<IRMethod>(64);

			public int Count { get { return vals.Count; } }
			public IRMethod this[int idx] 
			{
				get { return vals[idx]; }
				set 
				{
					vals[idx].ParentTypeMethodIndex = -1;
					vals[idx] = value;
					value.ParentTypeMethodIndex = idx;
				}
			}

			public void Add(IRMethod m)
			{
				m.ParentTypeMethodIndex = vals.Count;
				vals.Add(m);
			}

			public IEnumerator<IRMethod> GetEnumerator()
			{
				foreach (IRMethod m in vals)
					yield return m;
			}

			System.Collections.IEnumerator System.Collections.IEnumerable.GetEnumerator()
			{
				foreach (IRMethod m in vals)
					yield return m;
			}

			public override string ToString()
			{
				return "Count: " + Count.ToString();
			}
		}
		public readonly IRMethodList Methods = new IRMethodList();
        public readonly List<IRType> NestedTypes = new List<IRType>();
		public IRType NestedInsideOfType = null;
		private IRType mParentType = null;
		private IRType mBaseType = null;
		public IRType BaseType
		{
			get
			{
				if (mParentType != null && mBaseType == null)
				{
					mBaseType = mParentType.mBaseType;
				}
				return mBaseType;
			}
			set
			{
				mBaseType = value;
			}
		}

		public readonly List<IRMethod> VirtualMethodTree = new List<IRMethod>();
		internal void CreateVirtualMethodTree()
		{
			if (VirtualMethodTree.Count == 0)
			{
				if (BaseType != null)
				{
					BaseType.CreateVirtualMethodTree();
					VirtualMethodTree.AddRange(BaseType.VirtualMethodTree);
				}
				foreach (IRMethod method in Methods)
				{
					if (!method.IsStatic && method.IsVirtual)
					{
						if (method.IsNewSlot)
						{
							VirtualMethodTree.Add(method);
						}
						else if (method.IsHideBySig)
						{
							var v = VirtualMethodTree.FindIndex(m2 => m2 == method);
							VirtualMethodTree[v] = method;
						}
						else
						{
							var v = VirtualMethodTree.FindIndex(m2 => m2.Name == method.Name);
							VirtualMethodTree[v] = method;
						}
					}
				}
			}
		}

		public readonly Dictionary<IRType, List<IRMethod>> InterfaceImplementationMap = new Dictionary<IRType, List<IRMethod>>();
		internal void CreateInterfaceImplementationMap()
		{
			if (InterfaceImplementationMap.Count == 0 && !IsInterface && !IsAbstract)
			{
				if (BaseType != null)
				{
					BaseType.CreateInterfaceImplementationMap();
					BaseType.InterfaceImplementationMap.ForEach(kv => InterfaceImplementationMap.Add(kv.Key, kv.Value));
				}
				foreach (var ii in ImplementedInterfaces)
				{
					InterfaceImplementationMap[ii] = new List<IRMethod>(ii.VirtualMethodTree.Count);
					for (int i = 0; i < ii.VirtualMethodTree.Count; i++) 
						InterfaceImplementationMap[ii].Add(null);
					int i3 = 0;
					foreach (var im in ii.VirtualMethodTree)
					{
						var m = VirtualMethodTree.Where(m2 => m2 == im).FirstOrDefault();
						var emd = ExplicitOverrides.Where(m2 => m2.Overridden == im).FirstOrDefault();
						if (m == null && emd == null)
							throw new Exception("This class doesn't implement '" + im + "'!");
						InterfaceImplementationMap[ii][i3] = emd == null ? m : emd.Overridder;
						i3++;
					}
				}
			}
		}

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

        // We can cache this because the state
        // of the cache doesn't extend to any
        // derived type.
        private bool? mResolvedCache;
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

				if (mResolvedCache != null)
					return mResolvedCache.Value;

                mResolvedCache = true;
				if (!PresolvedType && !PostsolvedType)
				{
					Assembly.AppDomain.Types.Add(this);
				}
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

		private static int sTempID = 0;
		internal readonly int mTempID;
		private IRType()
		{
			mTempID = sTempID++;
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
			t.ImplementedInterfaces.AddRange(this.ImplementedInterfaces);
			this.ExplicitOverrides.ForEach(od => t.ExplicitOverrides.Add(od.Clone()));
            this.Methods.ForEach(m => t.Methods.Add(m.Clone(t)));
            t.NestedTypes.AddRange(this.NestedTypes);
			t.NestedInsideOfType = this.NestedInsideOfType;
            t.GenericParameters.AddRange(this.GenericParameters);

            t.ArrayElementType = this.ArrayElementType;
			t.mParentType = this;
			if (this.mBaseType != null) t.BaseType = this.BaseType;
			if (this.GenericType != null) t.GenericType = this.GenericType;
			else if (this.GenericParameters.Count > 0) t.GenericType = this;

            t.IsTemporaryMVar = this.IsTemporaryMVar;
            t.IsTemporaryVar = this.IsTemporaryVar;
            t.Name = this.Name;
            t.Namespace = this.Namespace;
			t.Flags = this.Flags;
			t.ManagedPointerType = this.ManagedPointerType;
            t.UnmanagedPointerType = this.UnmanagedPointerType;
            t.TemporaryVarOrMVarIndex = this.TemporaryVarOrMVarIndex;

            return t;
        }

		public readonly Dictionary<IRMethod, IRMethod> GenericMethods = new Dictionary<IRMethod, IRMethod>();



        public static readonly Dictionary<IRType, IRType> GenericTypes = new Dictionary<IRType, IRType>();
        /// <summary>
        /// Resolve any generic types used in this type.
        /// </summary>
        /// <param name="selfReference"></param>
        /// <param name="typeParams"></param>
        /// <param name="methodParams"></param>
        public void Resolve(ref IRType selfReference, IRGenericParameterList typeParams, IRGenericParameterList methodParams)
        {
            if (!Resolved || PresolvedType || PostsolvedType)
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
						if (!this.GenericParameters.Resolved)
						{
							if (this.PostsolvedType)
							{
								this.GenericParameters.TrySubstitute(typeParams, methodParams);
							}
							else
							{
								if (this.PresolvedType)
								{
									selfReference = Assembly.AppDomain.PresolveGenericType(this.GenericType, this.GenericParameters.ToList());
								}
								else
								{
									selfReference = Assembly.AppDomain.PresolveGenericType(this, this.GenericParameters.ToList());
								}
								selfReference.PresolvedType = false;
								selfReference.PostsolvedType = true;
								selfReference.Resolve(ref selfReference, typeParams, methodParams);
								return;
							}
						}
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

									//for (int i = 0; i < tp.GenericParameters.Count; i++)
									//{
									//    tp.GenericParameters[i] = this.GenericParameters[i];
									//}
									for (int i = 0; i < tp.Methods.Count; i++)
									{
										tp.Methods[i] = tp.Methods[i].Clone(tp);
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
				this.BaseType.Resolve(ref this.mBaseType, this.GenericParameters, IRGenericParameterList.Empty);

			for (int i = 0; i < ImplementedInterfaces.Count; i++)
			{
				IRType t = ImplementedInterfaces[i];
				t.Resolve(ref t, this.GenericParameters, IRGenericParameterList.Empty);
				ImplementedInterfaces[i] = t;
			}

            this.Fields.ForEach(f => f.Substitute());
			this.Methods.ForEach(m => m.Substitute(m.GenericParameters));

			for (int i = 0; i < ExplicitOverrides.Count; i++)
			{
				IRMethod m = ExplicitOverrides[i].Overridden;
				m.Resolve(ref m, this.GenericParameters, IRGenericParameterList.Empty);
				ExplicitOverrides[i].Overridden = m;
				m = ExplicitOverrides[i].Overridder;
				m.Resolve(ref m, this.GenericParameters, IRGenericParameterList.Empty);
				ExplicitOverrides[i].Overridder = m;
			}
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
			string n = Namespace;
			if (NestedInsideOfType != null)
			{
				if (n != "")
					n += "::";
				else
					n = "";
				n += NestedInsideOfType.ToString() + "/";
			}
			else
				n += ".";
			string nn = n + Name;
			if (IsGeneric)
			{
				nn += GenericParameters.ToString();
			}
			return nn;
		}

		public void Dump(IndentableStreamWriter pWriter)
		{
			StringBuilder sb = new StringBuilder(256);
			if (IsStructure) sb.Append(" struct");
			else if (IsEnumType) sb.Append(" enum");
			else if (IsInterface) sb.Append(" interface");
			else sb.Append(" class");

			if (IsSealed) sb.Append(" sealed");
			if (IsAbstract) sb.Append(" abstract");
			if (IsExplicitLayout) sb.Append(" explicit");

			if (sb.Length > 0)
			{
				sb.Insert(1, '|');
				sb.Append('|');
			}

			pWriter.WriteLine("IRType({0}) #{1}{2} {3} : {4}{5}{6}", mGlobalTypeID, mTempID, sb.ToString(), ToString(), BaseType, BaseType != null && ImplementedInterfaces.Count > 0 ? ", " : "", String.Join(", ", ImplementedInterfaces.Select(i => i.ToString()).ToArray()));
			pWriter.WriteLine("{");
			pWriter.Indent++;
			pWriter.WriteLine("Resolution State: {0}", (PresolvedType ? "Presolved" : (PostsolvedType ? "Postsolved" : (IsGeneric ? (Resolved ? "Instantiated" : "Unresolved") : "Original"))));
			Fields.ForEach(f => f.Dump(pWriter));
			Methods.ForEach(m => m.Dump(pWriter));
			if (GenericMethods.Count > 0)
			{
				pWriter.WriteLine("GenericMethods {0}", GenericMethods.Count);
				pWriter.WriteLine("{");
				pWriter.Indent++;
				GenericMethods.ForEach(m => m.Value.Dump(pWriter));
				pWriter.Indent--;
				pWriter.WriteLine("}");
			}
			if (ExplicitOverrides.Count > 0)
			{
				pWriter.WriteLine("ExplicitOverrides {0}", ExplicitOverrides.Count);
				pWriter.WriteLine("{");
				pWriter.Indent++;
				ExplicitOverrides.ForEach(od => od.Dump(pWriter));
				pWriter.Indent--;
				pWriter.WriteLine("}");
			}
			pWriter.WriteLine("VirtualMethodTree {0}", VirtualMethodTree.Count);
			pWriter.WriteLine("{");
			pWriter.Indent++;
			for (int i = 0; i < VirtualMethodTree.Count; i++)
			{
				pWriter.WriteLine("[{0}] = {1}", i, VirtualMethodTree[i].ToString());
			}
			pWriter.Indent--;
			pWriter.WriteLine("}");
			if (InterfaceImplementationMap.Count > 0)
			{
				pWriter.WriteLine("InterfaceImplementationMap {0}", InterfaceImplementationMap.Count);
				pWriter.WriteLine("{");
				pWriter.Indent++;
				foreach (var kv in InterfaceImplementationMap)
				{
					pWriter.WriteLine(kv.Key.ToString());
					pWriter.WriteLine("{");
					pWriter.Indent++;
					for (int i = 0; i < kv.Value.Count; ++i)
					{
						pWriter.WriteLine("[{0}] = {1}", i, kv.Value[i].ToString());
					}
					pWriter.Indent--;
					pWriter.WriteLine("}");
				}
				pWriter.Indent--;
				pWriter.WriteLine("}");
			}
			pWriter.Indent--;
			pWriter.WriteLine("}");
		}
    }
}
