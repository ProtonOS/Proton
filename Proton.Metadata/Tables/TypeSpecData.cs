using Proton.Metadata.Signatures;
using System;
using System.Collections.Generic;
using System.Text;

namespace Proton.Metadata.Tables
{
	public sealed class TypeSpecData
	{
		public static void Initialize(CLIFile pFile)
		{
			if ((pFile.CLIMetadataTables.PresentTables & (1ul << MetadataTables.TypeSpec)) != 0)
			{
				pFile.TypeSpecTable = new TypeSpecData[pFile.ReadInt32()];
				for (int index = 0; index < pFile.TypeSpecTable.Length; ++index) pFile.TypeSpecTable[index] = new TypeSpecData() { CLIFile = pFile, TableIndex = index };
			}
			else pFile.TypeSpecTable = new TypeSpecData[0];
		}

		public static void Load(CLIFile pFile)
		{
			for (int index = 0; index < pFile.TypeSpecTable.Length; ++index) pFile.TypeSpecTable[index].LoadData(pFile);
		}

		public static void Link(CLIFile pFile)
		{
			for (int index = 0; index < pFile.TypeSpecTable.Length; ++index) pFile.TypeSpecTable[index].LinkData(pFile);
		}

		public CLIFile CLIFile = null;

		public int TableIndex = 0;
		public byte[] Signature = null;

		public SigType ExpandedSignature = null;

		private void LoadData(CLIFile pFile)
		{
			Signature = pFile.ReadBlobHeap(pFile.ReadHeapIndex(HeapOffsetSizes.Blob32Bit));
		}

		private void LinkData(CLIFile pFile)
		{
			int cursor = 0;
			ExpandedSignature = new SigType(pFile, Signature, ref cursor);
		}
	}
}
