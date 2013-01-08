using System;
using System.Collections.Generic;
using System.Text;

namespace Proton.Metadata.Tables
{
	public sealed class DeclSecurityData
	{
		public static void Initialize(CLIFile pFile)
		{
			if ((pFile.CLIMetadataTables.PresentTables & (1ul << MetadataTables.DeclSecurity)) != 0)
			{
				pFile.DeclSecurityTable = new DeclSecurityData[pFile.ReadInt32()];
				for (int index = 0; index < pFile.DeclSecurityTable.Length; ++index) pFile.DeclSecurityTable[index] = new DeclSecurityData() { CLIFile = pFile, TableIndex = index };
			}
			else pFile.DeclSecurityTable = new DeclSecurityData[0];
		}

		public static void Load(CLIFile pFile)
		{
			for (int index = 0; index < pFile.DeclSecurityTable.Length; ++index) pFile.DeclSecurityTable[index].LoadData(pFile);
		}

		public static void Link(CLIFile pFile)
		{
			for (int index = 0; index < pFile.DeclSecurityTable.Length; ++index) pFile.DeclSecurityTable[index].LinkData(pFile);
		}

		public CLIFile CLIFile = null;

		public int TableIndex = 0;
		public ushort Action = 0;
		public HasDeclSecurityIndex Parent = new HasDeclSecurityIndex();
		public byte[] PermissionSet = null;

		private void LoadData(CLIFile pFile)
		{
			Action = (byte)pFile.ReadUInt16();
			Parent.LoadData(pFile);
			PermissionSet = pFile.ReadBlobHeap(pFile.ReadHeapIndex(HeapOffsetSizes.Blob32Bit));
		}

		private void LinkData(CLIFile pFile)
		{
		}
	}
}
