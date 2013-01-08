using System;
using System.Collections.Generic;
using System.Text;

namespace Proton.Metadata.Tables
{
	public sealed class FieldMarshalData
	{
		public static void Initialize(CLIFile pFile)
		{
			if ((pFile.CLIMetadataTables.PresentTables & (1ul << MetadataTables.FieldMarshal)) != 0)
			{
				pFile.FieldMarshalTable = new FieldMarshalData[pFile.ReadInt32()];
				for (int index = 0; index < pFile.FieldMarshalTable.Length; ++index) pFile.FieldMarshalTable[index] = new FieldMarshalData() { CLIFile = pFile, TableIndex = index };
			}
			else pFile.FieldMarshalTable = new FieldMarshalData[0];
		}

		public static void Load(CLIFile pFile)
		{
			for (int index = 0; index < pFile.FieldMarshalTable.Length; ++index) pFile.FieldMarshalTable[index].LoadData(pFile);
		}

		public static void Link(CLIFile pFile)
		{
			for (int index = 0; index < pFile.FieldMarshalTable.Length; ++index) pFile.FieldMarshalTable[index].LinkData(pFile);
		}

		public CLIFile CLIFile = null;

		public int TableIndex = 0;
		public HasFieldMarshalIndex Parent = new HasFieldMarshalIndex();
		public byte[] NativeType = null;

		private void LoadData(CLIFile pFile)
		{
			Parent.LoadData(pFile);
			NativeType = pFile.ReadBlobHeap(pFile.ReadHeapIndex(HeapOffsetSizes.Blob32Bit));
		}

		private void LinkData(CLIFile pFile)
		{
		}
	}
}
