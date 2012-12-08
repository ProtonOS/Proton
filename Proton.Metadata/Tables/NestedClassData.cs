using System;
using System.Collections.Generic;
using System.Text;

namespace Proton.Metadata.Tables
{
    public sealed class NestedClassData
    {
        public static void Initialize(CLIFile pFile)
        {
            if ((pFile.CLIMetadataTables.PresentTables & (1ul << MetadataTables.NestedClass)) != 0)
            {
                pFile.NestedClassTable = new NestedClassData[pFile.ReadInt32()];
                for (int index = 0; index < pFile.NestedClassTable.Length; ++index) pFile.NestedClassTable[index] = new NestedClassData() { CLIFile = pFile, TableIndex = index };
            }
            else pFile.NestedClassTable = new NestedClassData[0];
        }

        public static void Load(CLIFile pFile)
        {
            for (int index = 0; index < pFile.NestedClassTable.Length; ++index) pFile.NestedClassTable[index].LoadData(pFile);
        }

        public static void Link(CLIFile pFile)
        {
            for (int index = 0; index < pFile.NestedClassTable.Length; ++index) pFile.NestedClassTable[index].LinkData(pFile);
        }

        public CLIFile CLIFile = null;

        public int TableIndex = 0;
        public TypeDefData NestedClass = null;
        public TypeDefData EnclosingClass = null;

        private void LoadData(CLIFile pFile)
        {
            int typeDefIndex = 0;
            if (pFile.TypeDefTable.Length >= 0xFFFF) typeDefIndex = pFile.ReadInt32() - 1;
            else typeDefIndex = pFile.ReadUInt16() - 1;
            if (typeDefIndex >= 0) NestedClass = pFile.TypeDefTable[typeDefIndex];
            typeDefIndex = 0;
            if (pFile.TypeDefTable.Length >= 0xFFFF) typeDefIndex = pFile.ReadInt32() - 1;
            else typeDefIndex = pFile.ReadUInt16() - 1;
            if (typeDefIndex >= 0) EnclosingClass = pFile.TypeDefTable[typeDefIndex];
        }

        private void LinkData(CLIFile pFile)
        {
            EnclosingClass.NestedClassList.Add(NestedClass);
        }
    }
}
