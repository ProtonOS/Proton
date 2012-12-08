using System;
using System.Collections.Generic;
using System.Text;

namespace Proton.Metadata.Tables
{
    public sealed class MethodSemanticsData
    {
        public static void Initialize(CLIFile pFile)
        {
            if ((pFile.CLIMetadataTables.PresentTables & (1ul << MetadataTables.MethodSemantics)) != 0)
            {
                pFile.MethodSemanticsTable = new MethodSemanticsData[pFile.ReadInt32()];
                for (int index = 0; index < pFile.MethodSemanticsTable.Length; ++index) pFile.MethodSemanticsTable[index] = new MethodSemanticsData() { CLIFile = pFile, TableIndex = index };
            }
            else pFile.MethodSemanticsTable = new MethodSemanticsData[0];
        }

        public static void Load(CLIFile pFile)
        {
            for (int index = 0; index < pFile.MethodSemanticsTable.Length; ++index) pFile.MethodSemanticsTable[index].LoadData(pFile);
        }

        public static void Link(CLIFile pFile)
        {
            for (int index = 0; index < pFile.MethodSemanticsTable.Length; ++index) pFile.MethodSemanticsTable[index].LinkData(pFile);
        }

        public CLIFile CLIFile = null;

        public int TableIndex = 0;
        public ushort Semantics = 0;
        public MethodDefData Method = null;
        public HasSemanticsIndex Association = new HasSemanticsIndex();

        private void LoadData(CLIFile pFile)
        {
            Semantics = pFile.ReadUInt16();
            int methodDefIndex = 0;
            if (pFile.MethodDefTable.Length >= 0xFFFF) methodDefIndex = pFile.ReadInt32() - 1;
            else methodDefIndex = pFile.ReadUInt16() - 1;
            if (methodDefIndex >= 0) Method = pFile.MethodDefTable[methodDefIndex];
            Association.LoadData(pFile);
        }

        private void LinkData(CLIFile pFile)
        {
        }
    }
}
