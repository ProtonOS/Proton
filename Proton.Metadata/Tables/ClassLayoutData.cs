using System;
using System.Collections.Generic;
using System.Text;

namespace Proton.Metadata.Tables
{
	public sealed class ClassLayoutData
	{
		public static void Initialize(CLIFile pFile)
		{
			if ((pFile.CLIMetadataTables.PresentTables & (1ul << MetadataTables.ClassLayout)) != 0)
			{
				pFile.ClassLayoutTable = new ClassLayoutData[pFile.ReadInt32()];
				for (int index = 0; index < pFile.ClassLayoutTable.Length; ++index) pFile.ClassLayoutTable[index] = new ClassLayoutData() { CLIFile = pFile, TableIndex = index };
			}
			else pFile.ClassLayoutTable = new ClassLayoutData[0];
		}

		public static void Load(CLIFile pFile)
		{
			for (int index = 0; index < pFile.ClassLayoutTable.Length; ++index) pFile.ClassLayoutTable[index].LoadData(pFile);
		}

		public static void Link(CLIFile pFile)
		{
			for (int index = 0; index < pFile.ClassLayoutTable.Length; ++index) pFile.ClassLayoutTable[index].LinkData(pFile);
		}

		public CLIFile CLIFile = null;

		public int TableIndex = 0;
		public ushort PackingSize = 0;
		public uint ClassSize = 0;
		public TypeDefData Parent = null;

		private void LoadData(CLIFile pFile)
		{
			PackingSize = pFile.ReadUInt16();
			ClassSize = pFile.ReadUInt32();
			int typeDefIndex = 0;
			if (pFile.TypeDefTable.Length >= 0xFFFF) typeDefIndex = pFile.ReadInt32() - 1;
			else typeDefIndex = pFile.ReadUInt16() - 1;
			if (typeDefIndex >= 0) Parent = pFile.TypeDefTable[typeDefIndex];
		}

		private void LinkData(CLIFile pFile)
		{
		}
	}
}
