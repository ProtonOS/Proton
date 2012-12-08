using System;
using System.Collections.Generic;
using System.Text;

namespace Proton.Metadata.Tables
{
    public sealed class EventData
    {
        public static void Initialize(CLIFile pFile)
        {
            if ((pFile.CLIMetadataTables.PresentTables & (1ul << MetadataTables.Event)) != 0)
            {
                pFile.EventTable = new EventData[pFile.ReadInt32()];
                for (int index = 0; index < pFile.EventTable.Length; ++index) pFile.EventTable[index] = new EventData() { CLIFile = pFile, TableIndex = index };
            }
            else pFile.EventTable = new EventData[0];
        }

        public static void Load(CLIFile pFile)
        {
            for (int index = 0; index < pFile.EventTable.Length; ++index) pFile.EventTable[index].LoadData(pFile);
        }

        public static void Link(CLIFile pFile)
        {
            for (int index = 0; index < pFile.EventTable.Length; ++index) pFile.EventTable[index].LinkData(pFile);
        }

        public CLIFile CLIFile = null;

        public int TableIndex = 0;
        public ushort Flags = 0;
        public string Name = null;
        public TypeDefRefOrSpecIndex EventType = new TypeDefRefOrSpecIndex();

        public EventMapData ParentEventMap = null;

        private void LoadData(CLIFile pFile)
        {
            Flags = pFile.ReadUInt16();
            Name = pFile.ReadStringHeap(pFile.ReadHeapIndex(HeapOffsetSizes.Strings32Bit));
            EventType.LoadData(pFile);
        }

        private void LinkData(CLIFile pFile)
        {
        }
    }
}
