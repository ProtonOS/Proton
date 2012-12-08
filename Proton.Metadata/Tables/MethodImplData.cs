using System;
using System.Collections.Generic;
using System.Text;

namespace Proton.Metadata.Tables
{
    public sealed class MethodImplData
    {
        public static void Initialize(CLIFile pFile)
        {
            if ((pFile.CLIMetadataTables.PresentTables & (1ul << MetadataTables.MethodImpl)) != 0)
            {
                pFile.MethodImplTable = new MethodImplData[pFile.ReadInt32()];
                for (int index = 0; index < pFile.MethodImplTable.Length; ++index) pFile.MethodImplTable[index] = new MethodImplData() { CLIFile = pFile, TableIndex = index };
            }
            else pFile.MethodImplTable = new MethodImplData[0];
        }

        public static void Load(CLIFile pFile)
        {
            for (int index = 0; index < pFile.MethodImplTable.Length; ++index) pFile.MethodImplTable[index].LoadData(pFile);
        }

        public static void Link(CLIFile pFile)
        {
            for (int index = 0; index < pFile.MethodImplTable.Length; ++index) pFile.MethodImplTable[index].LinkData(pFile);
        }

        public CLIFile CLIFile = null;

        public int TableIndex = 0;
        public TypeDefData Class = null;
        public MethodDefOrRefIndex MethodBody = new MethodDefOrRefIndex();
        public MethodDefOrRefIndex MethodDeclaration = new MethodDefOrRefIndex();

        private void LoadData(CLIFile pFile)
        {
            int typeDefIndex = 0;
            if (pFile.TypeDefTable.Length >= 0xFFFF) typeDefIndex = pFile.ReadInt32() - 1;
            else typeDefIndex = pFile.ReadUInt16() - 1;
            if (typeDefIndex >= 0) Class = pFile.TypeDefTable[typeDefIndex];
            MethodBody.LoadData(pFile);
            MethodDeclaration.LoadData(pFile);
        }

        private void LinkData(CLIFile pFile)
        {
        }
    }
}
