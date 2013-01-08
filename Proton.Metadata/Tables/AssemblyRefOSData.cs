using System;
using System.Collections.Generic;
using System.Text;

namespace Proton.Metadata.Tables
{
	public sealed class AssemblyRefOSData
	{
		public static void Initialize(CLIFile pFile)
		{
			if ((pFile.CLIMetadataTables.PresentTables & (1ul << MetadataTables.AssemblyRefOS)) != 0)
			{
				pFile.AssemblyRefOSTable = new AssemblyRefOSData[pFile.ReadInt32()];
				for (int index = 0; index < pFile.AssemblyRefOSTable.Length; ++index) pFile.AssemblyRefOSTable[index] = new AssemblyRefOSData() { CLIFile = pFile, TableIndex = index };
			}
			else pFile.AssemblyRefOSTable = new AssemblyRefOSData[0];
		}

		public static void Load(CLIFile pFile)
		{
			for (int index = 0; index < pFile.AssemblyRefOSTable.Length; ++index) pFile.AssemblyRefOSTable[index].LoadData(pFile);
		}

		public static void Link(CLIFile pFile)
		{
			for (int index = 0; index < pFile.AssemblyRefOSTable.Length; ++index) pFile.AssemblyRefOSTable[index].LinkData(pFile);
		}

		public CLIFile CLIFile = null;

		public int TableIndex = 0;
		public uint OSPlatformId = 0;
		public uint OSMajorVersion = 0;
		public uint OSMinorVersion = 0;
		public AssemblyRefData AssemblyRef = null;

		private void LoadData(CLIFile pFile)
		{
			OSPlatformId = pFile.ReadUInt32();
			OSMajorVersion = pFile.ReadUInt32();
			OSMinorVersion = pFile.ReadUInt32();
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
