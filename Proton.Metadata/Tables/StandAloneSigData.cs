using Proton.Metadata.Signatures;
using System;
using System.Collections.Generic;
using System.Text;

namespace Proton.Metadata.Tables
{
    public sealed class StandAloneSigData
    {
        public static void Initialize(CLIFile pFile)
        {
            if ((pFile.CLIMetadataTables.PresentTables & (1ul << MetadataTables.StandAloneSig)) != 0)
            {
                pFile.StandAloneSigTable = new StandAloneSigData[pFile.ReadInt32()];
                for (int index = 0; index < pFile.StandAloneSigTable.Length; ++index) pFile.StandAloneSigTable[index] = new StandAloneSigData() { CLIFile = pFile, TableIndex = index };
            }
            else pFile.StandAloneSigTable = new StandAloneSigData[0];
        }

        public static void Load(CLIFile pFile)
        {
            for (int index = 0; index < pFile.StandAloneSigTable.Length; ++index) pFile.StandAloneSigTable[index].LoadData(pFile);
        }

        public static void Link(CLIFile pFile)
        {
            for (int index = 0; index < pFile.StandAloneSigTable.Length; ++index) pFile.StandAloneSigTable[index].LinkData(pFile);
        }

        public CLIFile CLIFile = null;

        public int TableIndex = 0;
        public byte[] Signature = null;

        public FieldSig ExpandedFieldSignature = null;
        public MethodSig ExpandedMethodSignature = null;
        public LocalVarSig ExpandedLocalVarSignature = null;

        private void LoadData(CLIFile pFile)
        {
            Signature = pFile.ReadBlobHeap(pFile.ReadHeapIndex(HeapOffsetSizes.Blob32Bit));
        }

        private void LinkData(CLIFile pFile)
        {
            int cursor = 0;
            if (Signature[0] == 0x06) ExpandedFieldSignature = new FieldSig(pFile, Signature, ref cursor);
            else if (Signature[0] == 0x07) ExpandedLocalVarSignature = new LocalVarSig(pFile, Signature, ref cursor);
            else ExpandedMethodSignature = new MethodSig(pFile, Signature, ref cursor);
        }
    }
}
