using System;
using System.Collections.Generic;
using System.Text;
using System.Diagnostics;

namespace Proton.Metadata.Tables
{
    public sealed class GenericParamData
    {
        public static void Initialize(CLIFile pFile)
        {
            if ((pFile.CLIMetadataTables.PresentTables & (1ul << MetadataTables.GenericParam)) != 0)
            {
                pFile.GenericParamTable = new GenericParamData[pFile.ReadInt32()];
                for (int index = 0; index < pFile.GenericParamTable.Length; ++index) pFile.GenericParamTable[index] = new GenericParamData() { CLIFile = pFile, TableIndex = index };
            }
            else pFile.GenericParamTable = new GenericParamData[0];
        }

        public static void Load(CLIFile pFile)
        {
            GenericParamData[] table = pFile.GenericParamTable;
            int size = table.Length;
            for (int index = 0; index < size; ++index) table[index].LoadData(pFile);

            Stopwatch sw = new Stopwatch();
            sw.Start();
            /* A single quickswap step in O(n) */
            /* Sets MethoDef first */
            int left = 0, right = size - 1;
            while(left <= right)
            {
                while (table[left].Owner.Type == TypeOrMethodDefIndex.TypeOrMethodDefType.MethodDef && left < size)
                    ++left;

                while (table[right].Owner.Type == TypeOrMethodDefIndex.TypeOrMethodDefType.TypeDef && right > 0)
                    --right;

                if (left <= right) // swap
                {
                    GenericParamData temp = table[left];
                    table[left] = table[right];
                    table[right] = temp;
                    ++left;
                    --right;
                }
            } while (left < right);
            pFile.GenericParamTablePivot = left;
            sw.Stop();
            Console.WriteLine("Time to process GenericParamData: {0} ticks.", sw.ElapsedTicks);
        }

        public static void Link(CLIFile pFile)
        {
            for (int index = 0; index < pFile.GenericParamTable.Length; ++index) pFile.GenericParamTable[index].LinkData(pFile);
        }

        public CLIFile CLIFile = null;

        public int TableIndex = 0;
        public ushort Number = 0;
		public GenericParamAttributes Flags = GenericParamAttributes.None;
        public TypeOrMethodDefIndex Owner = new TypeOrMethodDefIndex();
        public string Name = null;

        private void LoadData(CLIFile pFile)
        {
            Number = pFile.ReadUInt16();
            Flags = (GenericParamAttributes)pFile.ReadUInt16();
            Owner.LoadData(pFile);
            Name = pFile.ReadStringHeap(pFile.ReadHeapIndex(HeapOffsetSizes.Strings32Bit));
        }

        private void LinkData(CLIFile pFile)
        {
        }
    }
}
