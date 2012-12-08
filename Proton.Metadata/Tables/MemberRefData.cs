using Proton.Metadata.Signatures;
using System;
using System.Collections.Generic;
using System.Text;

namespace Proton.Metadata.Tables
{
    public sealed class MemberRefData
    {
        public static void Initialize(CLIFile pFile)
        {
            if ((pFile.CLIMetadataTables.PresentTables & (1ul << MetadataTables.MemberRef)) != 0)
            {
                pFile.MemberRefTable = new MemberRefData[pFile.ReadInt32()];
                for (int index = 0; index < pFile.MemberRefTable.Length; ++index) pFile.MemberRefTable[index] = new MemberRefData() { CLIFile = pFile, TableIndex = index };
            }
            else pFile.MemberRefTable = new MemberRefData[0];
        }

        public static void Load(CLIFile pFile)
        {
            for (int index = 0; index < pFile.MemberRefTable.Length; ++index) pFile.MemberRefTable[index].LoadData(pFile);
        }

        public static void Link(CLIFile pFile)
        {
            for (int index = 0; index < pFile.MemberRefTable.Length; ++index) pFile.MemberRefTable[index].LinkData(pFile);
        }

        public CLIFile CLIFile = null;

        public int TableIndex = 0;
        public MemberRefParentIndex Class = new MemberRefParentIndex();
        public string Name = null;
        public byte[] Signature = null;
        public MethodSig ExpandedMethodSignature = null;
        public FieldSig ExpandedFieldSignature = null;

        private void LoadData(CLIFile pFile)
        {
            Class.LoadData(pFile);
            Name = pFile.ReadStringHeap(pFile.ReadHeapIndex(HeapOffsetSizes.Strings32Bit));
            Signature = pFile.ReadBlobHeap(pFile.ReadHeapIndex(HeapOffsetSizes.Blob32Bit));
        }

        private void LinkData(CLIFile pFile)
        {
            int cursor = 0;
            if (Signature[0] == 0x06) ExpandedFieldSignature = new FieldSig(pFile, Signature, ref cursor);
            else ExpandedMethodSignature = new MethodSig(pFile, Signature, ref cursor);
        }

        public bool IsMethodRef { get { return ExpandedMethodSignature != null; } }
        public bool IsFieldRef { get { return ExpandedFieldSignature != null; } }
    }
}
