using Proton.Metadata.Signatures;
using System;
using System.Collections.Generic;
using System.Text;

namespace Proton.Metadata.Tables
{
    public sealed class MethodSpecData
    {
        public static void Initialize(CLIFile pFile)
        {
            if ((pFile.CLIMetadataTables.PresentTables & (1ul << MetadataTables.MethodSpec)) != 0)
            {
                pFile.MethodSpecTable = new MethodSpecData[pFile.ReadInt32()];
                for (int index = 0; index < pFile.MethodSpecTable.Length; ++index) pFile.MethodSpecTable[index] = new MethodSpecData() { CLIFile = pFile, TableIndex = index };
            }
            else pFile.MethodSpecTable = new MethodSpecData[0];
        }

        public static void Load(CLIFile pFile)
        {
            for (int index = 0; index < pFile.MethodSpecTable.Length; ++index) pFile.MethodSpecTable[index].LoadData(pFile);
        }

        public static void Link(CLIFile pFile)
        {
            for (int index = 0; index < pFile.MethodSpecTable.Length; ++index) pFile.MethodSpecTable[index].LinkData(pFile);
        }

        public CLIFile CLIFile = null;

        public int TableIndex = 0;
        public MethodDefOrRefIndex Method = new MethodDefOrRefIndex();
        public byte[] Instantiation = null;

        public MethodSpecSig ExpandedInstantiation = null;

        private void LoadData(CLIFile pFile)
        {
            Method.LoadData(pFile);
            Instantiation = pFile.ReadBlobHeap(pFile.ReadHeapIndex(HeapOffsetSizes.Blob32Bit));
        }

        private void LinkData(CLIFile pFile)
        {
            int cursor = 0;
            ExpandedInstantiation = new MethodSpecSig(pFile, Instantiation, ref cursor);
        }
    }
}
