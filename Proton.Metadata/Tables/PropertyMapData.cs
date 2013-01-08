using System;
using System.Collections.Generic;
using System.Text;

namespace Proton.Metadata.Tables
{
	public sealed class PropertyMapData
	{
		public static void Initialize(CLIFile pFile)
		{
			if ((pFile.CLIMetadataTables.PresentTables & (1ul << MetadataTables.PropertyMap)) != 0)
			{
				pFile.PropertyMapTable = new PropertyMapData[pFile.ReadInt32()];
				for (int index = 0; index < pFile.PropertyMapTable.Length; ++index) pFile.PropertyMapTable[index] = new PropertyMapData() { CLIFile = pFile, TableIndex = index };
			}
			else pFile.PropertyMapTable = new PropertyMapData[0];
		}

		public static void Load(CLIFile pFile)
		{
			for (int index = 0; index < pFile.PropertyMapTable.Length; ++index) pFile.PropertyMapTable[index].LoadData(pFile);
		}

		public static void Link(CLIFile pFile)
		{
			for (int index = 0; index < pFile.PropertyMapTable.Length; ++index) pFile.PropertyMapTable[index].LinkData(pFile);
		}

		public CLIFile CLIFile = null;

		public int TableIndex = 0;
		public TypeDefData Parent = null;
		public int PropertyListIndex = 0;
		public List<PropertyData> PropertyList = new List<PropertyData>();

		private void LoadData(CLIFile pFile)
		{
			int typeDefIndex = 0;
			if (pFile.TypeDefTable.Length >= 0xFFFF) typeDefIndex = pFile.ReadInt32() - 1;
			else typeDefIndex = pFile.ReadUInt16() - 1;
			if (typeDefIndex >= 0) Parent = pFile.TypeDefTable[typeDefIndex];
			if (pFile.PropertyTable.Length >= 0xFFFF) PropertyListIndex = pFile.ReadInt32() - 1;
			else PropertyListIndex = pFile.ReadUInt16() - 1;
		}

		private void LinkData(CLIFile pFile)
		{
			int propertyListCount = pFile.PropertyTable.Length - PropertyListIndex;
			if (TableIndex < (pFile.PropertyMapTable.Length - 1)) propertyListCount = pFile.PropertyMapTable[TableIndex + 1].PropertyListIndex - PropertyListIndex;
			for (int index = 0; index < propertyListCount; ++index) { PropertyList.Add(pFile.PropertyTable[PropertyListIndex + index]); pFile.PropertyTable[PropertyListIndex + index].ParentPropertyMap = this; }
		}
	}
}
