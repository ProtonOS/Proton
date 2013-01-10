using System;
using System.Collections.Generic;
using System.Text;

namespace Proton.Metadata.Tables
{
	public sealed class TypeDefData
	{
		public static void Initialize(CLIFile pFile)
		{
			if ((pFile.CLIMetadataTables.PresentTables & (1ul << MetadataTables.TypeDef)) != 0)
			{
				pFile.TypeDefTable = new TypeDefData[pFile.ReadInt32()];
				for (int index = 0; index < pFile.TypeDefTable.Length; ++index) pFile.TypeDefTable[index] = new TypeDefData() { CLIFile = pFile, TableIndex = index };
			}
			else pFile.TypeDefTable = new TypeDefData[0];
		}

		public static void Load(CLIFile pFile)
		{
			for (int index = 0; index < pFile.TypeDefTable.Length; ++index) pFile.TypeDefTable[index].LoadData(pFile);
		}

		public static void Link(CLIFile pFile)
		{
			for (int index = 0; index < pFile.TypeDefTable.Length; ++index) pFile.TypeDefTable[index].LinkData(pFile);
		}

		public CLIFile CLIFile = null;

		public int TableIndex = 0;
		public TypeAttributes Flags = 0;
		public string TypeName = null;
		public string TypeNamespace = null;
		public TypeDefRefOrSpecIndex Extends = new TypeDefRefOrSpecIndex();
		public int FieldListIndex = 0;
		public List<FieldData> FieldList = new List<FieldData>();
		public int MethodListIndex = 0;
		public List<MethodDefData> MethodList = new List<MethodDefData>();

		public List<TypeDefRefOrSpecIndex> InterfaceList = new List<TypeDefRefOrSpecIndex>();
		public List<TypeDefData> NestedClassList = new List<TypeDefData>();

		public List<GenericParamData> GenericParamList = new List<GenericParamData>();
		public List<MethodImplData> MethodImplList = new List<MethodImplData>();

		private void LoadData(CLIFile pFile)
		{
			Flags = (TypeAttributes)pFile.ReadUInt32();
			TypeName = pFile.ReadStringHeap(pFile.ReadHeapIndex(HeapOffsetSizes.Strings32Bit));
			TypeNamespace = pFile.ReadStringHeap(pFile.ReadHeapIndex(HeapOffsetSizes.Strings32Bit));
			Extends.LoadData(pFile);
			if (pFile.FieldTable.Length >= 0xFFFF) FieldListIndex = pFile.ReadInt32() - 1;
			else FieldListIndex = pFile.ReadUInt16() - 1;
			if (pFile.MethodDefTable.Length >= 0xFFFF) MethodListIndex = pFile.ReadInt32() - 1;
			else MethodListIndex = pFile.ReadUInt16() - 1;
		}

		private void LinkData(CLIFile pFile)
		{
			int fieldListCount = pFile.FieldTable.Length - FieldListIndex;
			if (TableIndex < (pFile.TypeDefTable.Length - 1)) fieldListCount = pFile.TypeDefTable[TableIndex + 1].FieldListIndex - FieldListIndex;
			for (int index = 0; index < fieldListCount; ++index) { FieldList.Add(pFile.FieldTable[FieldListIndex + index]); pFile.FieldTable[FieldListIndex + index].ParentTypeDef = this; }
			int methodListCount = pFile.MethodDefTable.Length - MethodListIndex;
			if (TableIndex < (pFile.TypeDefTable.Length - 1)) methodListCount = pFile.TypeDefTable[TableIndex + 1].MethodListIndex - MethodListIndex;
			for (int index = 0; index < methodListCount; ++index) { MethodList.Add(pFile.MethodDefTable[MethodListIndex + index]); pFile.MethodDefTable[MethodListIndex + index].ParentTypeDef = this; }
		}
	}
}
