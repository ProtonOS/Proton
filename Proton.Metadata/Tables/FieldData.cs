using Proton.Metadata.Signatures;
using System;
using System.Collections.Generic;
using System.Text;

namespace Proton.Metadata.Tables
{
	public sealed class FieldData
	{
		public static void Initialize(CLIFile pFile)
		{
			if ((pFile.CLIMetadataTables.PresentTables & (1ul << MetadataTables.Field)) != 0)
			{
				pFile.FieldTable = new FieldData[pFile.ReadInt32()];
				for (int index = 0; index < pFile.FieldTable.Length; ++index) pFile.FieldTable[index] = new FieldData() { CLIFile = pFile, TableIndex = index };
			}
			else pFile.FieldTable = new FieldData[0];
		}

		public static void Load(CLIFile pFile)
		{
			for (int index = 0; index < pFile.FieldTable.Length; ++index) pFile.FieldTable[index].LoadData(pFile);
		}

		public static void Link(CLIFile pFile)
		{
			for (int index = 0; index < pFile.FieldTable.Length; ++index) pFile.FieldTable[index].LinkData(pFile);
		}

		public CLIFile CLIFile = null;

		public int TableIndex = 0;
		public FieldAttributes Flags = FieldAttributes.None;
		public string Name = null;
		public byte[] Signature = null;

		public TypeDefData ParentTypeDef = null;
		public FieldSig ExpandedSignature = null;

		private void LoadData(CLIFile pFile)
		{
			Flags = (FieldAttributes)pFile.ReadUInt16();
			Name = pFile.ReadStringHeap(pFile.ReadHeapIndex(HeapOffsetSizes.Strings32Bit));
			Signature = pFile.ReadBlobHeap(pFile.ReadHeapIndex(HeapOffsetSizes.Blob32Bit));
		}

		private void LinkData(CLIFile pFile)
		{
			int cursor = 0;
			ExpandedSignature = new FieldSig(pFile, Signature, ref cursor);
		}
	}
}
