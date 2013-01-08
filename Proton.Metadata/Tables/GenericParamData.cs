using System;
using System.Collections.Generic;
using System.Text;

namespace Proton.Metadata.Tables
{
	public sealed class GenericParamData
	{
		public static void Initialize(CLIFile pFile)
		{
			if ((pFile.CLIMetadataTables.PresentTables & (1ul << MetadataTables.GenericParam)) != 0)
			{
				pFile.GenericParamTable = new GenericParamData[pFile.ReadInt32()];
				for (int index = 0; index < pFile.GenericParamTable.Length; ++index) pFile.GenericParamTable[index] = new GenericParamData() { CLIFile = pFile, TableIndex = index };
			}
			else pFile.GenericParamTable = new GenericParamData[0];
		}

		public static void Load(CLIFile pFile)
		{
			for (int index = 0; index < pFile.GenericParamTable.Length; ++index) pFile.GenericParamTable[index].LoadData(pFile);
		}

		public static void Link(CLIFile pFile)
		{
			for (int index = 0; index < pFile.GenericParamTable.Length; ++index) pFile.GenericParamTable[index].LinkData(pFile);
		}

		public CLIFile CLIFile = null;

		public int TableIndex = 0;
		public ushort Number = 0;
		public GenericParamAttributes Flags = GenericParamAttributes.None;
		public TypeOrMethodDefIndex Owner = new TypeOrMethodDefIndex();
		public string Name = null;

		private void LoadData(CLIFile pFile)
		{
			Number = pFile.ReadUInt16();
			Flags = (GenericParamAttributes)pFile.ReadUInt16();
			Owner.LoadData(pFile);
			Name = pFile.ReadStringHeap(pFile.ReadHeapIndex(HeapOffsetSizes.Strings32Bit));
		}

		private void LinkData(CLIFile pFile)
		{
		}
	}
}
