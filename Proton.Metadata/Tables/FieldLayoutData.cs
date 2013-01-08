using System;
using System.Collections.Generic;
using System.Text;

namespace Proton.Metadata.Tables
{
	public sealed class FieldLayoutData
	{
		public static void Initialize(CLIFile pFile)
		{
			if ((pFile.CLIMetadataTables.PresentTables & (1ul << MetadataTables.FieldLayout)) != 0)
			{
				pFile.FieldLayoutTable = new FieldLayoutData[pFile.ReadInt32()];
				for (int index = 0; index < pFile.FieldLayoutTable.Length; ++index) pFile.FieldLayoutTable[index] = new FieldLayoutData() { CLIFile = pFile, TableIndex = index };
			}
			else pFile.FieldLayoutTable = new FieldLayoutData[0];
		}

		public static void Load(CLIFile pFile)
		{
			for (int index = 0; index < pFile.FieldLayoutTable.Length; ++index) pFile.FieldLayoutTable[index].LoadData(pFile);
		}

		public static void Link(CLIFile pFile)
		{
			for (int index = 0; index < pFile.FieldLayoutTable.Length; ++index) pFile.FieldLayoutTable[index].LinkData(pFile);
		}

		public CLIFile CLIFile = null;

		public int TableIndex = 0;
		public uint Offset = 0;
		public FieldData Field = null;

		private void LoadData(CLIFile pFile)
		{
			Offset = pFile.ReadUInt32();
			int fieldIndex = 0;
			if (pFile.FieldTable.Length >= 0xFFFF) fieldIndex = pFile.ReadInt32() - 1;
			else fieldIndex = pFile.ReadUInt16() - 1;
			if (fieldIndex >= 0) Field = pFile.FieldTable[fieldIndex];
		}

		private void LinkData(CLIFile pFile)
		{
		}
	}
}
