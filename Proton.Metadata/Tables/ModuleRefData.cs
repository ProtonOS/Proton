using System;
using System.Collections.Generic;
using System.Text;

namespace Proton.Metadata.Tables
{
    public sealed class ModuleRefData
    {
        public static void Initialize(CLIFile pFile)
        {
            if ((pFile.CLIMetadataTables.PresentTables & (1ul << MetadataTables.ModuleRef)) != 0)
            {
                pFile.ModuleRefTable = new ModuleRefData[pFile.ReadInt32()];
                for (int index = 0; index < pFile.ModuleRefTable.Length; ++index) pFile.ModuleRefTable[index] = new ModuleRefData() { CLIFile = pFile, TableIndex = index };
            }
            else pFile.ModuleRefTable = new ModuleRefData[0];
        }

        public static void Load(CLIFile pFile)
        {
            for (int index = 0; index < pFile.ModuleRefTable.Length; ++index) pFile.ModuleRefTable[index].LoadData(pFile);
        }

        public static void Link(CLIFile pFile)
        {
            for (int index = 0; index < pFile.ModuleRefTable.Length; ++index) pFile.ModuleRefTable[index].LinkData(pFile);
        }

        public CLIFile CLIFile = null;

        public int TableIndex = 0;
        public string Name = null;

        private void LoadData(CLIFile pFile)
        {
            Name = pFile.ReadStringHeap(pFile.ReadHeapIndex(HeapOffsetSizes.Strings32Bit));
        }

        private void LinkData(CLIFile pFile)
        {
        }
    }
}
