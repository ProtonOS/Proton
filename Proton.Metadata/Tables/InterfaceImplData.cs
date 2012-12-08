using System;
using System.Collections.Generic;
using System.Text;

namespace Proton.Metadata.Tables
{
    public sealed class InterfaceImplData
    {
        public static void Initialize(CLIFile pFile)
        {
            if ((pFile.CLIMetadataTables.PresentTables & (1ul << MetadataTables.InterfaceImpl)) != 0)
            {
                pFile.InterfaceImplTable = new InterfaceImplData[pFile.ReadInt32()];
                for (int index = 0; index < pFile.InterfaceImplTable.Length; ++index) pFile.InterfaceImplTable[index] = new InterfaceImplData() { CLIFile = pFile, TableIndex = index };
            }
            else pFile.InterfaceImplTable = new InterfaceImplData[0];
        }

        public static void Load(CLIFile pFile)
        {
            for (int index = 0; index < pFile.InterfaceImplTable.Length; ++index) pFile.InterfaceImplTable[index].LoadData(pFile);
        }

        public static void Link(CLIFile pFile)
        {
            for (int index = 0; index < pFile.InterfaceImplTable.Length; ++index) pFile.InterfaceImplTable[index].LinkData(pFile);
        }

        public CLIFile CLIFile = null;

        public int TableIndex = 0;
        public TypeDefData Class = null;
        public TypeDefRefOrSpecIndex Interface = new TypeDefRefOrSpecIndex();

        private void LoadData(CLIFile pFile)
        {
            int typeDefIndex = 0;
            if (pFile.TypeDefTable.Length >= 0xFFFF) typeDefIndex = pFile.ReadInt32() - 1;
            else typeDefIndex = pFile.ReadUInt16() - 1;
            if (typeDefIndex >= 0) Class = pFile.TypeDefTable[typeDefIndex];
            Interface.LoadData(pFile);
        }

        private void LinkData(CLIFile pFile)
        {
            Class.InterfaceList.Add(Interface);
        }
    }
}
