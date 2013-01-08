using System;
using System.Collections.Generic;
using System.Text;

namespace Proton.Metadata.Tables
{
	public sealed class AssemblyData
	{
		public static void Initialize(CLIFile pFile)
		{
			if ((pFile.CLIMetadataTables.PresentTables & (1ul << MetadataTables.Assembly)) != 0)
			{
				pFile.AssemblyTable = new AssemblyData[pFile.ReadInt32()];
				for (int index = 0; index < pFile.AssemblyTable.Length; ++index) pFile.AssemblyTable[index] = new AssemblyData() { CLIFile = pFile, TableIndex = index };
			}
			else pFile.AssemblyTable = new AssemblyData[0];
		}

		public static void Load(CLIFile pFile)
		{
			for (int index = 0; index < pFile.AssemblyTable.Length; ++index) pFile.AssemblyTable[index].LoadData(pFile);
		}

		public static void Link(CLIFile pFile)
		{
			for (int index = 0; index < pFile.AssemblyTable.Length; ++index) pFile.AssemblyTable[index].LinkData(pFile);
		}

		public CLIFile CLIFile = null;

		public int TableIndex = 0;
		public AssemblyHashAlgorithm HashAlgId = AssemblyHashAlgorithm.None;
		public ushort MajorVersion = 0;
		public ushort MinorVersion = 0;
		public ushort BuildNumber = 0;
		public ushort RevisionNumber = 0;
		public AssemblyFlags Flags = AssemblyFlags.None;
		public byte[] PublicKey = null;
		public string Name = null;
		public string Culture = null;

		private void LoadData(CLIFile pFile)
		{
			HashAlgId = (AssemblyHashAlgorithm)pFile.ReadUInt32();
			MajorVersion = pFile.ReadUInt16();
			MinorVersion = pFile.ReadUInt16();
			BuildNumber = pFile.ReadUInt16();
			RevisionNumber = pFile.ReadUInt16();
			Flags = (AssemblyFlags)pFile.ReadUInt32();
			PublicKey = pFile.ReadBlobHeap(pFile.ReadHeapIndex(HeapOffsetSizes.Blob32Bit));
			Name = pFile.ReadStringHeap(pFile.ReadHeapIndex(HeapOffsetSizes.Strings32Bit));
			Culture = pFile.ReadStringHeap(pFile.ReadHeapIndex(HeapOffsetSizes.Strings32Bit));
		}

		private void LinkData(CLIFile pFile)
		{
		}
	}
}
