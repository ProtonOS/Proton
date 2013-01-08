using System;
using System.Collections.Generic;
using System.Text;

namespace Proton.Metadata.Tables
{
	public sealed class ParamData
	{
		public static void Initialize(CLIFile pFile)
		{
			if ((pFile.CLIMetadataTables.PresentTables & (1ul << MetadataTables.Param)) != 0)
			{
				pFile.ParamTable = new ParamData[pFile.ReadInt32()];
				for (int index = 0; index < pFile.ParamTable.Length; ++index) pFile.ParamTable[index] = new ParamData() { CLIFile = pFile, TableIndex = index };
			}
			else pFile.ParamTable = new ParamData[0];
		}

		public static void Load(CLIFile pFile)
		{
			for (int index = 0; index < pFile.ParamTable.Length; ++index) pFile.ParamTable[index].LoadData(pFile);
		}

		public static void Link(CLIFile pFile)
		{
			for (int index = 0; index < pFile.ParamTable.Length; ++index) pFile.ParamTable[index].LinkData(pFile);
		}

		public CLIFile CLIFile = null;

		public int TableIndex = 0;
		public ParamAttributes Flags = ParamAttributes.None;
		public ushort Sequence = 0;
		public string Name = null;

		public MethodDefData ParentMethodDef = null;

		private void LoadData(CLIFile pFile)
		{
			Flags = (ParamAttributes)pFile.ReadUInt16();
			Sequence = pFile.ReadUInt16();
			Name = pFile.ReadStringHeap(pFile.ReadHeapIndex(HeapOffsetSizes.Strings32Bit));
		}

		private void LinkData(CLIFile pFile)
		{
		}
	}
}
