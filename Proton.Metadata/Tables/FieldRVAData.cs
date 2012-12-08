using System;
using System.Collections.Generic;
using System.Text;

namespace Proton.Metadata.Tables
{
    public sealed class FieldRVAData
    {
        public static void Initialize(CLIFile pFile)
        {
            if ((pFile.CLIMetadataTables.PresentTables & (1ul << MetadataTables.FieldRVA)) != 0)
            {
                pFile.FieldRVATable = new FieldRVAData[pFile.ReadInt32()];
                for (int index = 0; index < pFile.FieldRVATable.Length; ++index) pFile.FieldRVATable[index] = new FieldRVAData() { CLIFile = pFile, TableIndex = index };
            }
            else pFile.FieldRVATable = new FieldRVAData[0];
        }

        public static void Load(CLIFile pFile)
        {
            for (int index = 0; index < pFile.FieldRVATable.Length; ++index) pFile.FieldRVATable[index].LoadData(pFile);
        }

        public static void Link(CLIFile pFile)
        {
            for (int index = 0; index < pFile.FieldRVATable.Length; ++index) pFile.FieldRVATable[index].LinkData(pFile);
        }

        public CLIFile CLIFile = null;

        public int TableIndex = 0;
        public uint InitialValueOffset = 0;
        public FieldData Field = null;

        private void LoadData(CLIFile pFile)
        {
            InitialValueOffset = pFile.ReadUInt32();
            CLIFile.SectionHeader sectionHeader = pFile.GetSection(InitialValueOffset);
            InitialValueOffset = sectionHeader.PointerToRawData + (InitialValueOffset - sectionHeader.VirtualAddress);
            int fieldIndex = 0;
            if (pFile.FieldTable.Length >= 0xFFFF) fieldIndex = pFile.ReadInt32() - 1;
            else fieldIndex = pFile.ReadUInt16() - 1;
            if (fieldIndex >= 0) Field = pFile.FieldTable[fieldIndex];
        }

        private void LinkData(CLIFile pFile)
        {
        }
    }
}
