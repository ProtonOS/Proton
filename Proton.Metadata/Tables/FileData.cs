using System;
using System.Collections.Generic;
using System.Text;

namespace Proton.Metadata.Tables
{
	public sealed class FileData
	{
		public static void Initialize(CLIFile pFile)
		{
			if ((pFile.CLIMetadataTables.PresentTables & (1ul << MetadataTables.File)) != 0)
			{
				pFile.FileTable = new FileData[pFile.ReadInt32()];
				for (int index = 0; index < pFile.FileTable.Length; ++index) pFile.FileTable[index] = new FileData() { CLIFile = pFile, TableIndex = index };
			}
			else pFile.FileTable = new FileData[0];
		}

		public static void Load(CLIFile pFile)
		{
			for (int index = 0; index < pFile.FileTable.Length; ++index) pFile.FileTable[index].LoadData(pFile);
		}

		public static void Link(CLIFile pFile)
		{
			for (int index = 0; index < pFile.FileTable.Length; ++index) pFile.FileTable[index].LinkData(pFile);
		}

		public CLIFile CLIFile = null;

		public int TableIndex = 0;
		public FileAttributes Flags = FileAttributes.None;
		public string Name = null;
		public byte[] HashValue = null;

		private void LoadData(CLIFile pFile)
		{
			Flags = (FileAttributes)pFile.ReadUInt32();
			Name = pFile.ReadStringHeap(pFile.ReadHeapIndex(HeapOffsetSizes.Strings32Bit));
			HashValue = pFile.ReadBlobHeap(pFile.ReadHeapIndex(HeapOffsetSizes.Blob32Bit));
		}

		private void LinkData(CLIFile pFile)
		{
		}
	}
}
