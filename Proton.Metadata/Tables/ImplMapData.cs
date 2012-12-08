using System;
using System.Collections.Generic;
using System.Text;

namespace Proton.Metadata.Tables
{
    public sealed class ImplMapData
    {
        public static void Initialize(CLIFile pFile)
        {
            if ((pFile.CLIMetadataTables.PresentTables & (1ul << MetadataTables.ImplMap)) != 0)
            {
                pFile.ImplMapTable = new ImplMapData[pFile.ReadInt32()];
                for (int index = 0; index < pFile.ImplMapTable.Length; ++index) pFile.ImplMapTable[index] = new ImplMapData() { CLIFile = pFile, TableIndex = index };
            }
            else pFile.ImplMapTable = new ImplMapData[0];
        }

        public static void Load(CLIFile pFile)
        {
            for (int index = 0; index < pFile.ImplMapTable.Length; ++index) pFile.ImplMapTable[index].LoadData(pFile);
        }

        public static void Link(CLIFile pFile)
        {
            for (int index = 0; index < pFile.ImplMapTable.Length; ++index) pFile.ImplMapTable[index].LinkData(pFile);
        }

        public CLIFile CLIFile = null;

        public int TableIndex = 0;
        public ushort MappingFlags = 0;
        public MemberForwardedIndex MemberForwarded = new MemberForwardedIndex();
        public string ImportName = null;
        public ModuleRefData ImportScope = null;

        private void LoadData(CLIFile pFile)
        {
            MappingFlags = pFile.ReadUInt16();
            MemberForwarded.LoadData(pFile);
            ImportName = pFile.ReadStringHeap(pFile.ReadHeapIndex(HeapOffsetSizes.Strings32Bit));
            int moduleRefIndex = 0;
            if (pFile.ModuleRefTable.Length >= 0xFFFF) moduleRefIndex = pFile.ReadInt32() - 1;
            else moduleRefIndex = pFile.ReadUInt16() - 1;
            if (moduleRefIndex >= 0) ImportScope = pFile.ModuleRefTable[moduleRefIndex];
        }

        private void LinkData(CLIFile pFile)
        {
        }
    }
}
