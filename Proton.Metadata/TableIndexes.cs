using Proton.Metadata.Tables;
using System;
using System.Collections.Generic;

namespace Proton.Metadata
{
	public struct HasConstantIndex
	{
		public enum HasConstantType : int
		{
			Field = 0,
			Param = 1,
			Property = 2
		}
		public const byte HasConstantTypeBits = 2;
		public const byte HasConstantTypeMask = ((1 << HasConstantTypeBits) - 1);
		public const ushort HasConstantTypeMax16BitRows = (0xFFFF >> HasConstantTypeBits);

		public HasConstantType Type;
		public ParamData Param;
		public FieldData Field;
		public PropertyData Property;

		public void LoadData(CLIFile pFile)
		{
			int token = 0;
			if (pFile.FieldTable.Length > HasConstantTypeMax16BitRows ||
				pFile.ParamTable.Length > HasConstantTypeMax16BitRows ||
				pFile.PropertyTable.Length > HasConstantTypeMax16BitRows) token = pFile.ReadInt32();
			else token = pFile.ReadUInt16();
			Type = (HasConstantType)(token & HasConstantTypeMask);
			token = (token >> HasConstantTypeBits) - 1;
			if (token >= 0)
			{
				switch (Type)
				{
					case HasConstantType.Field: Field = pFile.FieldTable[token]; break;
					case HasConstantType.Param: Param = pFile.ParamTable[token]; break;
					case HasConstantType.Property: Property = pFile.PropertyTable[token]; break;
					default: throw new BadImageFormatException("HasConstant Type");
				}
			}
		}
	}


	public struct HasCustomAttributeIndex
	{
		public enum HasCustomAttributeType : int
		{
			MethodDef = 0,
			Field = 1,
			TypeRef = 2,
			TypeDef = 3,
			Param = 4,
			InterfaceImpl = 5,
			MemberRef = 6,
			Module = 7,
			DeclSecurity = 8,
			Property = 9,
			Event = 10,
			StandAloneSig = 11,
			ModuleRef = 12,
			TypeSpec = 13,
			Assembly = 14,
			AssemblyRef = 15,
			File = 16,
			ExportedType = 17,
			ManifestResource = 18,
			GenericParam = 19,
			GenericParamConstraint = 20,
			MethodSpec = 21
		}
		public const byte HasCustomAttributeTypeBits = 5;
		public const byte HasCustomAttributeTypeMask = ((1 << HasCustomAttributeTypeBits) - 1);
		public const ushort HasCustomAttributeTypeMax16BitRows = (0xFFFF >> HasCustomAttributeTypeBits);

		public HasCustomAttributeType Type;
		public MethodDefData MethodDef;
		public FieldData Field;
		public TypeRefData TypeRef;
		public TypeDefData TypeDef;
		public ParamData Param;
		public InterfaceImplData InterfaceImpl;
		public MemberRefData MemberRef;
		public ModuleData Module;
		public DeclSecurityData DeclSecurity;
		public PropertyData Property;
		public EventData Event;
		public StandAloneSigData StandAloneSig;
		public ModuleRefData ModuleRef;
		public TypeSpecData TypeSpec;
		public AssemblyData Assembly;
		public AssemblyRefData AssemblyRef;
		public FileData File;
		public ExportedTypeData ExportedType;
		public ManifestResourceData ManifestResource;
		public GenericParamData GenericParam;
		public GenericParamConstraintData GenericParamConstraint;
		public MethodSpecData MethodSpec;

		public void LoadData(CLIFile pFile)
		{
			int token = 0;
			if (pFile.MethodDefTable.Length > HasCustomAttributeTypeMax16BitRows ||
				pFile.FieldTable.Length > HasCustomAttributeTypeMax16BitRows ||
				pFile.TypeRefTable.Length > HasCustomAttributeTypeMax16BitRows ||
				pFile.TypeDefTable.Length > HasCustomAttributeTypeMax16BitRows ||
				pFile.ParamTable.Length > HasCustomAttributeTypeMax16BitRows ||
				pFile.InterfaceImplTable.Length > HasCustomAttributeTypeMax16BitRows ||
				pFile.MemberRefTable.Length > HasCustomAttributeTypeMax16BitRows ||
				pFile.ModuleTable.Length > HasCustomAttributeTypeMax16BitRows ||
				pFile.DeclSecurityTable.Length > HasCustomAttributeTypeMax16BitRows ||
				pFile.PropertyTable.Length > HasCustomAttributeTypeMax16BitRows ||
				pFile.EventTable.Length > HasCustomAttributeTypeMax16BitRows ||
				pFile.StandAloneSigTable.Length > HasCustomAttributeTypeMax16BitRows ||
				pFile.ModuleRefTable.Length > HasCustomAttributeTypeMax16BitRows ||
				pFile.TypeSpecTable.Length > HasCustomAttributeTypeMax16BitRows ||
				pFile.AssemblyTable.Length > HasCustomAttributeTypeMax16BitRows ||
				pFile.AssemblyRefTable.Length > HasCustomAttributeTypeMax16BitRows ||
				pFile.FileTable.Length > HasCustomAttributeTypeMax16BitRows ||
				pFile.ExportedTypeTable.Length > HasCustomAttributeTypeMax16BitRows ||
				pFile.ManifestResourceTable.Length > HasCustomAttributeTypeMax16BitRows ||
				pFile.GenericParamTable.Length > HasCustomAttributeTypeMax16BitRows ||
				pFile.GenericParamConstraintTable.Length > HasCustomAttributeTypeMax16BitRows ||
				pFile.MethodSpecTable.Length > HasCustomAttributeTypeMax16BitRows) token = pFile.ReadInt32();
			else token = pFile.ReadUInt16();
			Type = (HasCustomAttributeType)(token & HasCustomAttributeTypeMask);
			token = (token >> HasCustomAttributeTypeBits) - 1;
			if (token >= 0)
			{
				switch (Type)
				{
					case HasCustomAttributeType.MethodDef: MethodDef = pFile.MethodDefTable[token]; break;
					case HasCustomAttributeType.Field: Field = pFile.FieldTable[token]; break;
					case HasCustomAttributeType.TypeRef: TypeRef = pFile.TypeRefTable[token]; break;
					case HasCustomAttributeType.TypeDef: TypeDef = pFile.TypeDefTable[token]; break;
					case HasCustomAttributeType.Param: Param = pFile.ParamTable[token]; break;
					case HasCustomAttributeType.InterfaceImpl: InterfaceImpl = pFile.InterfaceImplTable[token]; break;
					case HasCustomAttributeType.MemberRef: MemberRef = pFile.MemberRefTable[token]; break;
					case HasCustomAttributeType.Module: Module = pFile.ModuleTable[token]; break;
					case HasCustomAttributeType.DeclSecurity: DeclSecurity = pFile.DeclSecurityTable[token]; break;
					case HasCustomAttributeType.Property: Property = pFile.PropertyTable[token]; break;
					case HasCustomAttributeType.Event: Event = pFile.EventTable[token]; break;
					case HasCustomAttributeType.StandAloneSig: StandAloneSig = pFile.StandAloneSigTable[token]; break;
					case HasCustomAttributeType.ModuleRef: ModuleRef = pFile.ModuleRefTable[token]; break;
					case HasCustomAttributeType.TypeSpec: TypeSpec = pFile.TypeSpecTable[token]; break;
					case HasCustomAttributeType.Assembly: Assembly = pFile.AssemblyTable[token]; break;
					case HasCustomAttributeType.AssemblyRef: AssemblyRef = pFile.AssemblyRefTable[token]; break;
					case HasCustomAttributeType.File: File = pFile.FileTable[token]; break;
					case HasCustomAttributeType.ExportedType: ExportedType = pFile.ExportedTypeTable[token]; break;
					case HasCustomAttributeType.ManifestResource: ManifestResource = pFile.ManifestResourceTable[token]; break;
					case HasCustomAttributeType.GenericParam: GenericParam = pFile.GenericParamTable[token]; break;
					case HasCustomAttributeType.GenericParamConstraint: GenericParamConstraint = pFile.GenericParamConstraintTable[token]; break;
					case HasCustomAttributeType.MethodSpec: MethodSpec = pFile.MethodSpecTable[token]; break;
					default: throw new BadImageFormatException("HasCustomAttribute Type");
				}
			}
		}
	}


	public struct CustomAttributeTypeIndex
	{
		public enum CustomAttributeTypeType : int
		{
			MethodDef = 2,
			MemberRef = 3
		}
		public const byte CustomAttributeTypeTypeBits = 3;
		public const byte CustomAttributeTypeTypeMask = ((1 << CustomAttributeTypeTypeBits) - 1);
		public const ushort CustomAttributeTypeTypeMax16BitRows = (0xFFFF >> CustomAttributeTypeTypeBits);

		public CustomAttributeTypeType Type;
		public MethodDefData MethodDef;
		public MemberRefData MemberRef;

		public void LoadData(CLIFile pFile)
		{
			int token = 0;
			if (pFile.MethodDefTable.Length > CustomAttributeTypeTypeMax16BitRows ||
				pFile.MemberRefTable.Length > CustomAttributeTypeTypeMax16BitRows) token = pFile.ReadInt32();
			else token = pFile.ReadUInt16();
			Type = (CustomAttributeTypeType)(token & CustomAttributeTypeTypeMask);
			token = (token >> CustomAttributeTypeTypeBits) - 1;
			if (token >= 0)
			{
				switch (Type)
				{
					case CustomAttributeTypeType.MethodDef: MethodDef = pFile.MethodDefTable[token]; break;
					case CustomAttributeTypeType.MemberRef: MemberRef = pFile.MemberRefTable[token]; break;
					default: throw new BadImageFormatException("CustomAttributeType Type");
				}
			}
		}
	}


	public struct HasDeclSecurityIndex
	{
		public enum HasDeclSecurityType : int
		{
			TypeDef = 0,
			MethodDef = 1,
			Assembly = 2
		}
		public const byte HasDeclSecurityTypeBits = 2;
		public const byte HasDeclSecurityTypeMask = ((1 << HasDeclSecurityTypeBits) - 1);
		public const ushort HasDeclSecurityTypeMax16BitRows = (0xFFFF >> HasDeclSecurityTypeBits);

		public HasDeclSecurityType Type;
		public TypeDefData TypeDef;
		public MethodDefData MethodDef;
		public AssemblyData Assembly;

		public void LoadData(CLIFile pFile)
		{
			int token = 0;
			if (pFile.TypeDefTable.Length > HasDeclSecurityTypeMax16BitRows ||
				pFile.MethodDefTable.Length > HasDeclSecurityTypeMax16BitRows ||
				pFile.AssemblyTable.Length > HasDeclSecurityTypeMax16BitRows) token = pFile.ReadInt32();
			else token = pFile.ReadUInt16();
			Type = (HasDeclSecurityType)(token & HasDeclSecurityTypeMask);
			token = (token >> HasDeclSecurityTypeBits) - 1;
			if (token >= 0)
			{
				switch (Type)
				{
					case HasDeclSecurityType.TypeDef: TypeDef = pFile.TypeDefTable[token]; break;
					case HasDeclSecurityType.MethodDef: MethodDef = pFile.MethodDefTable[token]; break;
					case HasDeclSecurityType.Assembly: Assembly = pFile.AssemblyTable[token]; break;
					default: throw new BadImageFormatException("HasDeclSecurity Type");
				}
			}
		}
	}


	public struct TypeDefRefOrSpecIndex
	{
		public enum TypeDefRefOrSpecType : int
		{
			TypeDef = 0,
			TypeRef = 1,
			TypeSpec = 2
		}
		public const byte TypeDefRefOrSpecTypeBits = 2;
		public const byte TypeDefRefOrSpecTypeMask = ((1 << TypeDefRefOrSpecTypeBits) - 1);
		public const ushort TypeDefRefOrSpecTypeMax16BitRows = (0xFFFF >> TypeDefRefOrSpecTypeBits);

		public TypeDefRefOrSpecType Type;
		public TypeDefData TypeDef;
		public TypeRefData TypeRef;
		public TypeSpecData TypeSpec;

		public void LoadData(CLIFile pFile)
		{
			int token = 0;
			if (pFile.TypeDefTable.Length > TypeDefRefOrSpecTypeMax16BitRows ||
				pFile.TypeRefTable.Length > TypeDefRefOrSpecTypeMax16BitRows ||
				pFile.TypeSpecTable.Length > TypeDefRefOrSpecTypeMax16BitRows) token = pFile.ReadInt32();
			else token = pFile.ReadUInt16();
			Type = (TypeDefRefOrSpecType)(token & TypeDefRefOrSpecTypeMask);
			token = (token >> TypeDefRefOrSpecTypeBits) - 1;
			if (token >= 0)
			{
				switch (Type)
				{
					case TypeDefRefOrSpecType.TypeDef: TypeDef = pFile.TypeDefTable[token]; break;
					case TypeDefRefOrSpecType.TypeRef: TypeRef = pFile.TypeRefTable[token]; break;
					case TypeDefRefOrSpecType.TypeSpec: TypeSpec = pFile.TypeSpecTable[token]; break;
					default: throw new BadImageFormatException("TypeDefRefOrSpec Type");
				}
			}
		}
	}


	public struct ImplementationIndex
	{
		public enum ImplementationType : int
		{
			File = 0,
			AssemblyRef = 1,
			ExportedType = 2
		}
		public const byte ImplementationTypeBits = 2;
		public const byte ImplementationTypeMask = ((1 << ImplementationTypeBits) - 1);
		public const ushort ImplementationTypeMax16BitRows = (0xFFFF >> ImplementationTypeBits);

		public ImplementationType Type;
		public FileData File;
		public AssemblyRefData AssemblyRef;
		public ExportedTypeData ExportedType;

		public void LoadData(CLIFile pFile)
		{
			int token = 0;
			if (pFile.FileTable.Length > ImplementationTypeMax16BitRows ||
				pFile.AssemblyRefTable.Length > ImplementationTypeMax16BitRows ||
				pFile.ExportedTypeTable.Length > ImplementationTypeMax16BitRows) token = pFile.ReadInt32();
			else token = pFile.ReadUInt16();
			Type = (ImplementationType)(token & ImplementationTypeMask);
			token = (token >> ImplementationTypeBits) - 1;
			if (token >= 0)
			{
				switch (Type)
				{
					case ImplementationType.File: File = pFile.FileTable[token]; break;
					case ImplementationType.AssemblyRef: AssemblyRef = pFile.AssemblyRefTable[token]; break;
					case ImplementationType.ExportedType: ExportedType = pFile.ExportedTypeTable[token]; break;
					default: throw new BadImageFormatException("Implementation Type");
				}
			}
		}
	}


	public struct HasFieldMarshalIndex
	{
		public enum HasFieldMarshalType : int
		{
			Field = 0,
			Param = 1
		}
		public const byte HasFieldMarshalTypeBits = 1;
		public const byte HasFieldMarshalTypeMask = ((1 << HasFieldMarshalTypeBits) - 1);
		public const ushort HasFieldMarshalTypeMax16BitRows = (0xFFFF >> HasFieldMarshalTypeBits);

		public HasFieldMarshalType Type;
		public FieldData Field;
		public ParamData Param;

		public void LoadData(CLIFile pFile)
		{
			int token = 0;
			if (pFile.FieldTable.Length > HasFieldMarshalTypeMax16BitRows ||
				pFile.ParamTable.Length > HasFieldMarshalTypeMax16BitRows) token = pFile.ReadInt32();
			else token = pFile.ReadUInt16();
			Type = (HasFieldMarshalType)(token & HasFieldMarshalTypeMask);
			token = (token >> HasFieldMarshalTypeBits) - 1;
			if (token >= 0)
			{
				switch (Type)
				{
					case HasFieldMarshalType.Field: Field = pFile.FieldTable[token]; break;
					case HasFieldMarshalType.Param: Param = pFile.ParamTable[token]; break;
					default: throw new BadImageFormatException("HasFieldMarshal Type");
				}
			}
		}
	}


	public struct TypeOrMethodDefIndex
	{
		public enum TypeOrMethodDefType : int
		{
			TypeDef = 0,
			MethodDef = 1
		}
		public const byte TypeOrMethodDefTypeBits = 1;
		public const byte TypeOrMethodDefTypeMask = ((1 << TypeOrMethodDefTypeBits) - 1);
		public const ushort TypeOrMethodDefTypeMax16BitRows = (0xFFFF >> TypeOrMethodDefTypeBits);

		public TypeOrMethodDefType Type;
		public TypeDefData TypeDef;
		public MethodDefData MethodDef;

		public void LoadData(CLIFile pFile)
		{
			int token = 0;
			if (pFile.TypeDefTable.Length > TypeOrMethodDefTypeMax16BitRows ||
				pFile.MethodDefTable.Length > TypeOrMethodDefTypeMax16BitRows) token = pFile.ReadInt32();
			else token = pFile.ReadUInt16();
			Type = (TypeOrMethodDefType)(token & TypeOrMethodDefTypeMask);
			token = (token >> TypeOrMethodDefTypeBits) - 1;
			if (token >= 0)
			{
				switch (Type)
				{
					case TypeOrMethodDefType.TypeDef: TypeDef = pFile.TypeDefTable[token]; break;
					case TypeOrMethodDefType.MethodDef: MethodDef = pFile.MethodDefTable[token]; break;
					default: throw new BadImageFormatException("TypeOrMethodDef Type");
				}
			}
		}
	}


	public struct MemberForwardedIndex
	{
		public enum MemberForwardedType : int
		{
			Field = 0,
			MethodDef = 1
		}
		public const byte MemberForwardedTypeBits = 1;
		public const byte MemberForwardedTypeMask = ((1 << MemberForwardedTypeBits) - 1);
		public const ushort MemberForwardedTypeMax16BitRows = (0xFFFF >> MemberForwardedTypeBits);

		public MemberForwardedType Type;
		public FieldData Field;
		public MethodDefData MethodDef;

		public void LoadData(CLIFile pFile)
		{
			int token = 0;
			if (pFile.FieldTable.Length > MemberForwardedTypeMax16BitRows ||
				pFile.MethodDefTable.Length > MemberForwardedTypeMax16BitRows) token = pFile.ReadInt32();
			else token = pFile.ReadUInt16();
			Type = (MemberForwardedType)(token & MemberForwardedTypeMask);
			token = (token >> MemberForwardedTypeBits) - 1;
			if (token >= 0)
			{
				switch (Type)
				{
					case MemberForwardedType.Field: Field = pFile.FieldTable[token]; break;
					case MemberForwardedType.MethodDef: MethodDef = pFile.MethodDefTable[token]; break;
					default: throw new BadImageFormatException("MemberForwarded Type");
				}
			}
		}
	}


	public struct MemberRefParentIndex
	{
		public enum MemberRefParentType : int
		{
			TypeDef = 0,
			TypeRef = 1,
			ModuleRef = 2,
			MethodDef = 3,
			TypeSpec = 4
		}
		public const byte MemberRefParentTypeBits = 3;
		public const byte MemberRefParentTypeMask = ((1 << MemberRefParentTypeBits) - 1);
		public const ushort MemberRefParentTypeMax16BitRows = (0xFFFF >> MemberRefParentTypeBits);

		public MemberRefParentType Type;
		public TypeDefData TypeDef;
		public TypeRefData TypeRef;
		public ModuleRefData ModuleRef;
		public MethodDefData MethodDef;
		public TypeSpecData TypeSpec;

		public void LoadData(CLIFile pFile)
		{
			int token = 0;
			if (pFile.TypeDefTable.Length > MemberRefParentTypeMax16BitRows ||
				pFile.TypeRefTable.Length > MemberRefParentTypeMax16BitRows ||
				pFile.ModuleRefTable.Length > MemberRefParentTypeMax16BitRows ||
				pFile.MethodDefTable.Length > MemberRefParentTypeMax16BitRows ||
				pFile.TypeSpecTable.Length > MemberRefParentTypeMax16BitRows) token = pFile.ReadInt32();
			else token = pFile.ReadUInt16();
			Type = (MemberRefParentType)(token & MemberRefParentTypeMask);
			token = (token >> MemberRefParentTypeBits) - 1;
			if (token >= 0)
			{
				switch (Type)
				{
					case MemberRefParentType.TypeDef: TypeDef = pFile.TypeDefTable[token]; break;
					case MemberRefParentType.TypeRef: TypeRef = pFile.TypeRefTable[token]; break;
					case MemberRefParentType.ModuleRef: ModuleRef = pFile.ModuleRefTable[token]; break;
					case MemberRefParentType.MethodDef: MethodDef = pFile.MethodDefTable[token]; break;
					case MemberRefParentType.TypeSpec: TypeSpec = pFile.TypeSpecTable[token]; break;
					default: throw new BadImageFormatException("MemberRefParent Type");
				}
			}
		}
	}


	public struct MethodDefOrRefIndex
	{
		public enum MethodDefOrRefType : int
		{
			MethodDef = 0,
			MemberRef = 1
		}
		public const byte MethodDefOrRefTypeBits = 1;
		public const byte MethodDefOrRefTypeMask = ((1 << MethodDefOrRefTypeBits) - 1);
		public const ushort MethodDefOrRefTypeMax16BitRows = (0xFFFF >> MethodDefOrRefTypeBits);

		public MethodDefOrRefType Type;
		public MethodDefData MethodDef;
		public MemberRefData MemberRef;

		public void LoadData(CLIFile pFile)
		{
			int token = 0;
			if (pFile.MethodDefTable.Length > MethodDefOrRefTypeMax16BitRows ||
				pFile.MemberRefTable.Length > MethodDefOrRefTypeMax16BitRows) token = pFile.ReadInt32();
			else token = pFile.ReadUInt16();
			Type = (MethodDefOrRefType)(token & MethodDefOrRefTypeMask);
			token = (token >> MethodDefOrRefTypeBits) - 1;
			if (token >= 0)
			{
				switch (Type)
				{
					case MethodDefOrRefType.MethodDef: MethodDef = pFile.MethodDefTable[token]; break;
					case MethodDefOrRefType.MemberRef: MemberRef = pFile.MemberRefTable[token]; break;
					default: throw new BadImageFormatException("MethodDefOrRef Type");
				}
			}
		}
	}


	public struct HasSemanticsIndex
	{
		public enum HasSemanticsType : int
		{
			Event = 0,
			Property = 1
		}
		public const byte HasSemanticsTypeBits = 1;
		public const byte HasSemanticsTypeMask = ((1 << HasSemanticsTypeBits) - 1);
		public const ushort HasSemanticsTypeMax16BitRows = (0xFFFF >> HasSemanticsTypeBits);

		public HasSemanticsType Type;
		public EventData Event;
		public PropertyData Property;

		public void LoadData(CLIFile pFile)
		{
			int token = 0;
			if (pFile.EventTable.Length > HasSemanticsTypeMax16BitRows ||
				pFile.PropertyTable.Length > HasSemanticsTypeMax16BitRows) token = pFile.ReadInt32();
			else token = pFile.ReadUInt16();
			Type = (HasSemanticsType)(token & HasSemanticsTypeMask);
			token = (token >> HasSemanticsTypeBits) - 1;
			if (token >= 0)
			{
				switch (Type)
				{
					case HasSemanticsType.Event: Event = pFile.EventTable[token]; break;
					case HasSemanticsType.Property: Property = pFile.PropertyTable[token]; break;
					default: throw new BadImageFormatException("HasSemantics Type");
				}
			}
		}
	}


	public struct ResolutionScopeIndex
	{
		public enum ResolutionScopeType : int
		{
			Module = 0,
			ModuleRef = 1,
			AssemblyRef = 2,
			TypeRef = 3
		}
		public const byte ResolutionScopeTypeBits = 2;
		public const byte ResolutionScopeTypeMask = ((1 << ResolutionScopeTypeBits) - 1);
		public const ushort ResolutionScopeTypeMax16BitRows = (0xFFFF >> ResolutionScopeTypeBits);

		public ResolutionScopeType Type;
		public ModuleData Module;
		public ModuleRefData ModuleRef;
		public AssemblyRefData AssemblyRef;
		public TypeRefData TypeRef;

		public bool IsNull
		{
			get
			{
				switch (Type)
				{
					case ResolutionScopeType.Module: return Module == null;
					case ResolutionScopeType.ModuleRef: return ModuleRef == null;
					case ResolutionScopeType.AssemblyRef: return AssemblyRef == null;
					case ResolutionScopeType.TypeRef: return TypeRef == null;
					default: break;
				}
				return true;
			}
		}
		public void LoadData(CLIFile pFile)
		{
			int token = 0;
			if (pFile.ModuleTable.Length > ResolutionScopeTypeMax16BitRows ||
				pFile.ModuleRefTable.Length > ResolutionScopeTypeMax16BitRows ||
				pFile.AssemblyRefTable.Length > ResolutionScopeTypeMax16BitRows ||
				pFile.TypeRefTable.Length > ResolutionScopeTypeMax16BitRows) token = pFile.ReadInt32();
			else token = pFile.ReadUInt16();
			Type = (ResolutionScopeType)(token & ResolutionScopeTypeMask);
			token = (token >> ResolutionScopeTypeBits) - 1;
			if (token >= 0)
			{
				switch (Type)
				{
					case ResolutionScopeType.Module: Module = pFile.ModuleTable[token]; break;
					case ResolutionScopeType.ModuleRef: ModuleRef = pFile.ModuleRefTable[token]; break;
					case ResolutionScopeType.AssemblyRef: AssemblyRef = pFile.AssemblyRefTable[token]; break;
					case ResolutionScopeType.TypeRef: TypeRef = pFile.TypeRefTable[token]; break;
					default: throw new BadImageFormatException("ResolutionScope Type");
				}
			}
		}
	}
}
