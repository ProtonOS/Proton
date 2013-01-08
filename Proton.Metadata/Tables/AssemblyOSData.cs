using System;
using System.Collections.Generic;
using System.Text;

namespace Proton.Metadata.Tables
{
	public sealed class AssemblyOSData
	{
		public static void Initialize(CLIFile pFile)
		{
			if ((pFile.CLIMetadataTables.PresentTables & (1ul << MetadataTables.AssemblyOS)) != 0)
			{
				pFile.AssemblyOSTable = new AssemblyOSData[pFile.ReadInt32()];
				for (int index = 0; index < pFile.AssemblyOSTable.Length; ++index) pFile.AssemblyOSTable[index] = new AssemblyOSData() { CLIFile = pFile, TableIndex = index };
			}
			else pFile.AssemblyOSTable = new AssemblyOSData[0];
		}

		public static void Load(CLIFile pFile)
		{
			for (int index = 0; index < pFile.AssemblyOSTable.Length; ++index) pFile.AssemblyOSTable[index].LoadData(pFile);
		}

		public static void Link(CLIFile pFile)
		{
			for (int index = 0; index < pFile.AssemblyOSTable.Length; ++index) pFile.AssemblyOSTable[index].LinkData(pFile);
		}

		public CLIFile CLIFile = null;

		public int TableIndex = 0;
		public uint OSPlatformId = 0;
		public uint OSMajorVersion = 0;
		public uint OSMinorVersion = 0;

		private void LoadData(CLIFile pFile)
		{
			OSPlatformId = pFile.ReadUInt32();
			OSMajorVersion = pFile.ReadUInt32();
			OSMinorVersion = pFile.ReadUInt32();
		}

		private void LinkData(CLIFile pFile)
		{
		}
	}
}
