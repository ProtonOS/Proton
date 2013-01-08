using System;
using System.Collections.Generic;
using System.Text;

namespace Proton.Metadata.Tables
{
	public sealed class ModuleData
	{
		public static void Initialize(CLIFile pFile)
		{
			if ((pFile.CLIMetadataTables.PresentTables & (1ul << MetadataTables.Module)) != 0)
			{
				pFile.ModuleTable = new ModuleData[pFile.ReadInt32()];
				for (int index = 0; index < pFile.ModuleTable.Length; ++index) pFile.ModuleTable[index] = new ModuleData() { CLIFile = pFile, TableIndex = index };
			}
			else pFile.ModuleTable = new ModuleData[0];
		}

		public static void Load(CLIFile pFile)
		{
			for (int index = 0; index < pFile.ModuleTable.Length; ++index) pFile.ModuleTable[index].LoadData(pFile);
		}

		public static void Link(CLIFile pFile)
		{
			for (int index = 0; index < pFile.ModuleTable.Length; ++index) pFile.ModuleTable[index].LinkData(pFile);
		}

		public CLIFile CLIFile = null;

		public int TableIndex = 0;
		public ushort Generation = 0;
		public string Name = null;
		public byte[] Mvid = null;
		public byte[] EncId = null;
		public byte[] EncBaseId = null;

		private void LoadData(CLIFile pFile)
		{
			Generation = pFile.ReadUInt16();
			Name = pFile.ReadStringHeap(pFile.ReadHeapIndex(HeapOffsetSizes.Strings32Bit));
			Mvid = pFile.ReadGUIDHeap(pFile.ReadHeapIndex(HeapOffsetSizes.GUID32Bit));
			EncId = pFile.ReadGUIDHeap(pFile.ReadHeapIndex(HeapOffsetSizes.GUID32Bit));
			EncBaseId = pFile.ReadGUIDHeap(pFile.ReadHeapIndex(HeapOffsetSizes.GUID32Bit));
		}

		private void LinkData(CLIFile pFile)
		{
		}
	}
}
