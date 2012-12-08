using Proton.Metadata.Signatures;
using System;
using System.Collections.Generic;
using System.Text;

namespace Proton.Metadata.Tables
{
    public sealed class PropertyData
    {
        public static void Initialize(CLIFile pFile)
        {
            if ((pFile.CLIMetadataTables.PresentTables & (1ul << MetadataTables.Property)) != 0)
            {
                pFile.PropertyTable = new PropertyData[pFile.ReadInt32()];
                for (int index = 0; index < pFile.PropertyTable.Length; ++index) pFile.PropertyTable[index] = new PropertyData() { CLIFile = pFile, TableIndex = index };
            }
            else pFile.PropertyTable = new PropertyData[0];
        }

        public static void Load(CLIFile pFile)
        {
            for (int index = 0; index < pFile.PropertyTable.Length; ++index) pFile.PropertyTable[index].LoadData(pFile);
        }

        public static void Link(CLIFile pFile)
        {
            for (int index = 0; index < pFile.PropertyTable.Length; ++index) pFile.PropertyTable[index].LinkData(pFile);
        }

        public CLIFile CLIFile = null;

        public int TableIndex = 0;
        public ushort Flags = 0;
        public string Name = null;
        public byte[] Type = null;

        public PropertyMapData ParentPropertyMap = null;
        public PropertySig ExpandedType = null;

        private void LoadData(CLIFile pFile)
        {
            Flags = pFile.ReadUInt16();
            Name = pFile.ReadStringHeap(pFile.ReadHeapIndex(HeapOffsetSizes.Strings32Bit));
            Type = pFile.ReadBlobHeap(pFile.ReadHeapIndex(HeapOffsetSizes.Blob32Bit));
        }

        private void LinkData(CLIFile pFile)
        {
            int cursor = 0;
            ExpandedType = new PropertySig(pFile, Type, ref cursor);
        }
    }
}
