using System;
using System.Collections.Generic;
using System.Text;

namespace Proton.Metadata.Tables
{
    public sealed class AssemblyRefData
    {
        public static void Initialize(CLIFile pFile)
        {
            if ((pFile.CLIMetadataTables.PresentTables & (1ul << MetadataTables.AssemblyRef)) != 0)
            {
                pFile.AssemblyRefTable = new AssemblyRefData[pFile.ReadInt32()];
                for (int index = 0; index < pFile.AssemblyRefTable.Length; ++index) pFile.AssemblyRefTable[index] = new AssemblyRefData() { CLIFile = pFile, TableIndex = index };
            }
            else pFile.AssemblyRefTable = new AssemblyRefData[0];
        }

        public static void Load(CLIFile pFile)
        {
            for (int index = 0; index < pFile.AssemblyRefTable.Length; ++index) pFile.AssemblyRefTable[index].LoadData(pFile);
        }

        public static void Link(CLIFile pFile)
        {
            for (int index = 0; index < pFile.AssemblyRefTable.Length; ++index) pFile.AssemblyRefTable[index].LinkData(pFile);
        }

        public CLIFile CLIFile = null;

        public int TableIndex = 0;
        public ushort MajorVersion = 0;
        public ushort MinorVersion = 0;
        public ushort BuildNumber = 0;
        public ushort RevisionNumber = 0;
        public uint Flags = 0;
        public byte[] PublicKeyOrToken = null;
        public string Name = null;
        public string Culture = null;
        public byte[] HashValue = null;

        private void LoadData(CLIFile pFile)
        {
            MajorVersion = pFile.ReadUInt16();
            MinorVersion = pFile.ReadUInt16();
            BuildNumber = pFile.ReadUInt16();
            RevisionNumber = pFile.ReadUInt16();
            Flags = pFile.ReadUInt32();
            PublicKeyOrToken = pFile.ReadBlobHeap(pFile.ReadHeapIndex(HeapOffsetSizes.Blob32Bit));
            Name = pFile.ReadStringHeap(pFile.ReadHeapIndex(HeapOffsetSizes.Strings32Bit));
            Culture = pFile.ReadStringHeap(pFile.ReadHeapIndex(HeapOffsetSizes.Strings32Bit));
            HashValue = pFile.ReadBlobHeap(pFile.ReadHeapIndex(HeapOffsetSizes.Blob32Bit));
        }

        private void LinkData(CLIFile pFile)
        {
        }
    }
}
