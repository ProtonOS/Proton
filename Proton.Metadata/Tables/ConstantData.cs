using System;
using System.Collections.Generic;
using System.Text;

namespace Proton.Metadata.Tables
{
    public sealed class ConstantData
    {
        public static void Initialize(CLIFile pFile)
        {
            if ((pFile.CLIMetadataTables.PresentTables & (1ul << MetadataTables.Constant)) != 0)
            {
                pFile.ConstantTable = new ConstantData[pFile.ReadInt32()];
                for (int index = 0; index < pFile.ConstantTable.Length; ++index) pFile.ConstantTable[index] = new ConstantData() { CLIFile = pFile, TableIndex = index };
            }
            else pFile.ConstantTable = new ConstantData[0];
        }

        public static void Load(CLIFile pFile)
        {
            for (int index = 0; index < pFile.ConstantTable.Length; ++index) pFile.ConstantTable[index].LoadData(pFile);
        }

        public static void Link(CLIFile pFile)
        {
            for (int index = 0; index < pFile.ConstantTable.Length; ++index) pFile.ConstantTable[index].LinkData(pFile);
        }

        public CLIFile CLIFile = null;

        public int TableIndex = 0;
        public byte Type = 0;
        public HasConstantIndex Parent = new HasConstantIndex();
        public byte[] Value = null;

        private void LoadData(CLIFile pFile)
        {
            Type = (byte)pFile.ReadUInt16();
            Parent.LoadData(pFile);
            Value = pFile.ReadBlobHeap(pFile.ReadHeapIndex(HeapOffsetSizes.Blob32Bit));
        }

        private void LinkData(CLIFile pFile)
        {
        }
    }
}
