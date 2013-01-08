using System;
using System.Collections.Generic;
using System.Text;

namespace Proton.Metadata.Tables
{
	public sealed class AssemblyRefProcessorData
	{
		public static void Initialize(CLIFile pFile)
		{
			if ((pFile.CLIMetadataTables.PresentTables & (1ul << MetadataTables.AssemblyRefProcessor)) != 0)
			{
				pFile.AssemblyRefProcessorTable = new AssemblyRefProcessorData[pFile.ReadInt32()];
				for (int index = 0; index < pFile.AssemblyRefProcessorTable.Length; ++index) pFile.AssemblyRefProcessorTable[index] = new AssemblyRefProcessorData() { CLIFile = pFile, TableIndex = index };
			}
			else pFile.AssemblyRefProcessorTable = new AssemblyRefProcessorData[0];
		}

		public static void Load(CLIFile pFile)
		{
			for (int index = 0; index < pFile.AssemblyRefProcessorTable.Length; ++index) pFile.AssemblyRefProcessorTable[index].LoadData(pFile);
		}

		public static void Link(CLIFile pFile)
		{
			for (int index = 0; index < pFile.AssemblyRefProcessorTable.Length; ++index) pFile.AssemblyRefProcessorTable[index].LinkData(pFile);
		}

		public CLIFile CLIFile = null;

		public int TableIndex = 0;
		public uint Processor = 0;
		public AssemblyRefData AssemblyRef = null;

		private void LoadData(CLIFile pFile)
		{
			Processor = pFile.ReadUInt32();
			int assemblyRefIndex = 0;
			if (pFile.AssemblyRefTable.Length >= 0xFFFF) assemblyRefIndex = pFile.ReadInt32() - 1;
			else assemblyRefIndex = pFile.ReadUInt16() - 1;
			if (assemblyRefIndex >= 0) AssemblyRef = pFile.AssemblyRefTable[assemblyRefIndex];
		}

		private void LinkData(CLIFile pFile)
		{
		}
	}
}
