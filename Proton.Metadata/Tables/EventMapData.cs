using System;
using System.Collections.Generic;
using System.Text;

namespace Proton.Metadata.Tables
{
    public sealed class EventMapData
    {
        public static void Initialize(CLIFile pFile)
        {
            if ((pFile.CLIMetadataTables.PresentTables & (1ul << MetadataTables.EventMap)) != 0)
            {
                pFile.EventMapTable = new EventMapData[pFile.ReadInt32()];
                for (int index = 0; index < pFile.EventMapTable.Length; ++index) pFile.EventMapTable[index] = new EventMapData() { CLIFile = pFile, TableIndex = index };
            }
            else pFile.EventMapTable = new EventMapData[0];
        }

        public static void Load(CLIFile pFile)
        {
            for (int index = 0; index < pFile.EventMapTable.Length; ++index) pFile.EventMapTable[index].LoadData(pFile);
        }

        public static void Link(CLIFile pFile)
        {
            for (int index = 0; index < pFile.EventMapTable.Length; ++index) pFile.EventMapTable[index].LinkData(pFile);
        }

        public CLIFile CLIFile = null;

        public int TableIndex = 0;
        public TypeDefData Parent = null;
        public int EventListIndex = 0;
        public List<EventData> EventList = new List<EventData>();

        private void LoadData(CLIFile pFile)
        {
            int typeDefIndex = 0;
            if (pFile.TypeDefTable.Length >= 0xFFFF) typeDefIndex = pFile.ReadInt32() - 1;
            else typeDefIndex = pFile.ReadUInt16() - 1;
            if (typeDefIndex >= 0) Parent = pFile.TypeDefTable[typeDefIndex];
            if (pFile.EventTable.Length >= 0xFFFF) EventListIndex = pFile.ReadInt32() - 1;
            else EventListIndex = pFile.ReadUInt16() - 1;
        }

        private void LinkData(CLIFile pFile)
        {
            int eventListCount = pFile.EventTable.Length - EventListIndex;
            if (TableIndex < (pFile.EventMapTable.Length - 1)) eventListCount = pFile.EventMapTable[TableIndex + 1].EventListIndex - EventListIndex;
            for (int index = 0; index < eventListCount; ++index) { EventList.Add(pFile.EventTable[EventListIndex + index]); pFile.EventTable[EventListIndex + index].ParentEventMap = this; }
        }
    }
}
