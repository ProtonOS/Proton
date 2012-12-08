using System;
using System.Collections.Generic;
using System.Text;

namespace Proton.Metadata.Tables
{
    public sealed class TypeRefData
    {
        public static void Initialize(CLIFile pFile)
        {
            if ((pFile.CLIMetadataTables.PresentTables & (1ul << MetadataTables.TypeRef)) != 0)
            {
                pFile.TypeRefTable = new TypeRefData[pFile.ReadInt32()];
                for (int index = 0; index < pFile.TypeRefTable.Length; ++index) pFile.TypeRefTable[index] = new TypeRefData() { CLIFile = pFile, TableIndex = index };
            }
            else pFile.TypeRefTable = new TypeRefData[0];
        }

        public static void Load(CLIFile pFile)
        {
            for (int index = 0; index < pFile.TypeRefTable.Length; ++index) pFile.TypeRefTable[index].LoadData(pFile);
        }

        public static void Link(CLIFile pFile)
        {
            for (int index = 0; index < pFile.TypeRefTable.Length; ++index) pFile.TypeRefTable[index].LinkData(pFile);
        }

        public CLIFile CLIFile = null;

        public int TableIndex = 0;
        public ResolutionScopeIndex ResolutionScope = new ResolutionScopeIndex();
        public string TypeName = null;
        public string TypeNamespace = null;

        public ExportedTypeData ExportedType = null;

        private void LoadData(CLIFile pFile)
        {
            ResolutionScope.LoadData(pFile);
            TypeName = pFile.ReadStringHeap(pFile.ReadHeapIndex(HeapOffsetSizes.Strings32Bit));
            TypeNamespace = pFile.ReadStringHeap(pFile.ReadHeapIndex(HeapOffsetSizes.Strings32Bit));
        }

        private void LinkData(CLIFile pFile)
        {
            if (ResolutionScope.IsNull) ExportedType = Array.Find(CLIFile.ExportedTypeTable, e => e.TypeNamespace == TypeNamespace && e.TypeName == TypeName);
        }
    }
}
