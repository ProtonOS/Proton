using System;
using System.Collections.Generic;
using System.Text;

namespace Proton.Metadata.Tables
{
    public sealed class GenericParamConstraintData
    {
        public static void Initialize(CLIFile pFile)
        {
            if ((pFile.CLIMetadataTables.PresentTables & (1ul << MetadataTables.GenericParamConstraint)) != 0)
            {
                pFile.GenericParamConstraintTable = new GenericParamConstraintData[pFile.ReadInt32()];
                for (int index = 0; index < pFile.GenericParamConstraintTable.Length; ++index) pFile.GenericParamConstraintTable[index] = new GenericParamConstraintData() { CLIFile = pFile, TableIndex = index };
            }
            else pFile.GenericParamConstraintTable = new GenericParamConstraintData[0];
        }

        public static void Load(CLIFile pFile)
        {
            for (int index = 0; index < pFile.GenericParamConstraintTable.Length; ++index) pFile.GenericParamConstraintTable[index].LoadData(pFile);
        }

        public static void Link(CLIFile pFile)
        {
            for (int index = 0; index < pFile.GenericParamConstraintTable.Length; ++index) pFile.GenericParamConstraintTable[index].LinkData(pFile);
        }

        public CLIFile CLIFile = null;

        public int TableIndex = 0;
        public GenericParamData Owner = null;
        public TypeDefRefOrSpecIndex Constraint = new TypeDefRefOrSpecIndex();

        private void LoadData(CLIFile pFile)
        {
            int genericParamIndex = 0;
            if (pFile.GenericParamTable.Length >= 0xFFFF) genericParamIndex = pFile.ReadInt32() - 1;
            else genericParamIndex = pFile.ReadUInt16() - 1;
            if (genericParamIndex >= 0) Owner = pFile.GenericParamTable[genericParamIndex];
            Constraint.LoadData(pFile);
        }

        private void LinkData(CLIFile pFile)
        {
        }
    }
}
