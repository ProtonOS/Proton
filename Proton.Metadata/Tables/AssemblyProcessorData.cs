using System;
using System.Collections.Generic;
using System.Text;

namespace Proton.Metadata.Tables
{
    public sealed class AssemblyProcessorData
    {
        public static void Initialize(CLIFile pFile)
        {
            if ((pFile.CLIMetadataTables.PresentTables & (1ul << MetadataTables.AssemblyProcessor)) != 0)
            {
                pFile.AssemblyProcessorTable = new AssemblyProcessorData[pFile.ReadInt32()];
                for (int index = 0; index < pFile.AssemblyProcessorTable.Length; ++index) pFile.AssemblyProcessorTable[index] = new AssemblyProcessorData() { CLIFile = pFile, TableIndex = index };
            }
            else pFile.AssemblyProcessorTable = new AssemblyProcessorData[0];
        }

        public static void Load(CLIFile pFile)
        {
            for (int index = 0; index < pFile.AssemblyProcessorTable.Length; ++index) pFile.AssemblyProcessorTable[index].LoadData(pFile);
        }

        public static void Link(CLIFile pFile)
        {
            for (int index = 0; index < pFile.AssemblyProcessorTable.Length; ++index) pFile.AssemblyProcessorTable[index].LinkData(pFile);
        }

        public CLIFile CLIFile = null;

        public int TableIndex = 0;
        public uint Processor = 0;

        private void LoadData(CLIFile pFile)
        {
            Processor = pFile.ReadUInt32();
        }

        private void LinkData(CLIFile pFile)
        {
        }
    }
}
