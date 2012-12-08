using System;
using System.Collections.Generic;
using System.Text;

namespace Proton.Metadata.Tables
{
    public sealed class ExportedTypeData
    {
        public static void Initialize(CLIFile pFile)
        {
            if ((pFile.CLIMetadataTables.PresentTables & (1ul << MetadataTables.ExportedType)) != 0)
            {
                pFile.ExportedTypeTable = new ExportedTypeData[pFile.ReadInt32()];
                for (int index = 0; index < pFile.ExportedTypeTable.Length; ++index) pFile.ExportedTypeTable[index] = new ExportedTypeData() { CLIFile = pFile, TableIndex = index };
            }
            else pFile.ExportedTypeTable = new ExportedTypeData[0];
        }

        public static void Load(CLIFile pFile)
        {
            for (int index = 0; index < pFile.ExportedTypeTable.Length; ++index) pFile.ExportedTypeTable[index].LoadData(pFile);
        }

        public static void Link(CLIFile pFile)
        {
            for (int index = 0; index < pFile.ExportedTypeTable.Length; ++index) pFile.ExportedTypeTable[index].LinkData(pFile);
        }

        public CLIFile CLIFile = null;

        public int TableIndex = 0;
        public uint Flags = 0;
        public TypeDefData TypeDef = null;
        public string TypeName = null;
        public string TypeNamespace = null;
        public ImplementationIndex Implementation = new ImplementationIndex();

        private void LoadData(CLIFile pFile)
        {
            Flags = pFile.ReadUInt32();
            int typeDefIndex = 0;
            if (pFile.TypeDefTable.Length >= 0xFFFF) typeDefIndex = pFile.ReadInt32() - 1;
            else typeDefIndex = pFile.ReadUInt16() - 1;
            if (typeDefIndex >= 0) TypeDef = pFile.TypeDefTable[typeDefIndex];
            TypeName = pFile.ReadStringHeap(pFile.ReadHeapIndex(HeapOffsetSizes.Strings32Bit));
            TypeNamespace = pFile.ReadStringHeap(pFile.ReadHeapIndex(HeapOffsetSizes.Strings32Bit));
            Implementation.LoadData(pFile);
        }

        private void LinkData(CLIFile pFile)
        {
        }
    }
}
