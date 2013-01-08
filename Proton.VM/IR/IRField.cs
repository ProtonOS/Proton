using Proton.Metadata.Tables;
using Proton.Metadata.Signatures;
using System;
using System.Collections.Generic;
using Proton.Metadata;
using System.Text;

namespace Proton.VM.IR
{
	public sealed class IRField
	{
		public IRAssembly Assembly = null;

		public string Name = null;
		public FieldAttributes Flags = FieldAttributes.None;

		public bool IsStatic { get { return (Flags & FieldAttributes.Static) == FieldAttributes.Static; } }
		public bool IsLiteral { get { return (Flags & FieldAttributes.Literal) == FieldAttributes.Literal; } }
		internal SigElementType LiteralType = SigElementType.End;
		internal byte[] LiteralValue = null;

		public IRType ParentType = null;

		private IRField mParentField = null;
		private IRType mType;
		public IRType Type
		{
			get
			{
				if (mType == null && mParentField != null)
				{
					mType = mParentField.Type;
					mParentField = null;
				}
				return mType;
			}
			set
			{
				mType = value;
			}
		}

		public int Offset = -1;

		/// <summary>
		/// True if all the types that this field
		/// uses are fully resolved, aka. if they
		/// are generic, they are fully instantiated.
		/// </summary>
		public bool Resolved { get { return Type.Resolved; } }

		public void Resolve(ref IRField selfReference, IRGenericParameterList typeParams, IRGenericParameterList methodParams)
		{
			IRType t = ParentType;
			ParentType.Resolve(ref t, typeParams, methodParams);
			IRField f2 = selfReference;
			selfReference = t.Fields[ParentType.Fields.FindIndex(f => f == f2)];
		}

		/// <summary>
		/// Resolve any generic types in this field.
		/// </summary>
		/// <param name="typeParams">The type parameters to use to resolve with.</param>
		public void Substitute()
		{
			Type.Resolve(ref mType, ParentType.GenericParameters, IRGenericParameterList.Empty);
		}

		/// <summary>
		/// Creates a shallow copy of this field.
		/// </summary>
		/// <returns>The shallow copy.</returns>
		public IRField Clone(IRType newParent)
		{
			IRField f = new IRField(this.Assembly);

			f.Name = this.Name;
			f.Flags = this.Flags;
			f.ParentType = newParent;
			f.Type = this.Type;
			f.mParentField = this.Type == null ? this : null;

			return f;
		}

		public IRField(IRAssembly pAssembly)
		{
			Assembly = pAssembly;
		}

		public override string ToString()
		{
			return (IsStatic ? "static " : "") + Type.ToString() + " " + Name;
		}

		public bool CompareSignature(FieldSig pFieldSig)
		{
			if (Type != Assembly.AppDomain.PresolveType(pFieldSig.Type)) return false;
			return true;
		}

		public bool CompareSignature(MemberRefData pMemberRefData)
		{
			if (Name != pMemberRefData.Name) return false;
			return CompareSignature(pMemberRefData.ExpandedFieldSignature);
		}

		public void Dump(IndentableStreamWriter pWriter)
		{
			StringBuilder sb = new StringBuilder();
			if (IsLiteral) sb.AppendFormat(", LiteralType {0}", LiteralType.ToString());
			pWriter.WriteLine("IRField {0} @ {1}{2}", ToString(), Offset, sb.ToString());
		}
	}
}
