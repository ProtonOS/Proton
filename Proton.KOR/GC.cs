using System.Collections.Generic;
using System.Kernel;

namespace System
{
    public static unsafe class GC
    {
        private const int SmallHeapSize = 0x20000;
        private const int LargeHeapSize = 0x80000;
        private const int ReservedPointer = 0x100000;
        private const int ReservedSize = 0x100000;
        private const int ReservedSmallHeapPointer = 0x100000;
        private const int ReservedLargeHeapPointer = 0x120000;
        private const int ReservedObjectPointer = 0x1F0000;

        private class GCHeap
        {
            public uint InitialPoolSize;
            public List<object> AllocatedObjects;
            public uint[] AllocationTree;
            public byte AllocationTreeLevels;
            public uint Size;
            public uint Available;
            public uint Allocated;
            public void* Bottom;
        }

        private enum GCObjectFlags
        {
            Allocated = 1,
            Marked = 2
        }

        private struct GCObject
        {
            public Type.TypeData* TypeData;
            public GCObjectFlags Flags;
            public uint AllocatedSize;
            public uint ActualSize;
        }

        private struct GCUsableMemory
        {
            public long Address;
            public long Size;
            public long Used;

            public GCUsableMemory(long pAddress, long pSize)
            {
                Address = pAddress;
                Size = pSize;
                Used = 0;
            }
        }

        private static bool sBusy = false;
        private static bool sHeapsReady = false;
        private static IntPtr* sReservedObjectPointers = (IntPtr*)ReservedObjectPointer;
        private static uint sReservedObjectCount = 0;
        private static uint sReservedSmallHeapUsed = 0;
        private static uint sReservedLargeHeapUsed = 0;
        private static GCUsableMemory[] sUsableMemory = null;
        private static int sCurrentUsableMemoryIndex = 0;
        private static List<GCHeap> sSmallHeaps = null;
        private static List<GCHeap> sLargeHeaps = null;

        internal static void Initialize(uint pMultibootMagic, MultibootHeader* pMultibootHeader)
        {
            //sUsableMemory = usableMemory;
        }

        internal static object Allocate(uint objectSize)
        {
            GCObject* gco = null;
            uint requiredSize = (uint)(sizeof(GCObject) + objectSize);
            if (!sHeapsReady)
            {
                if (requiredSize <= SmallHeapSize)
                {
                    if (requiredSize > (SmallHeapSize - sReservedSmallHeapUsed)) while (true) ;
                    gco = (GCObject*)(ReservedSmallHeapPointer + sReservedSmallHeapUsed);
                    sReservedSmallHeapUsed += requiredSize;
                }
                else if (requiredSize <= LargeHeapSize)
                {
                    if (requiredSize > (LargeHeapSize - sReservedLargeHeapUsed)) while (true) ;
                    gco = (GCObject*)(ReservedLargeHeapPointer + sReservedLargeHeapUsed);
                    sReservedLargeHeapUsed += requiredSize;
                }
                else while (true) ;
            }
            gco->TypeData = typeof(object).GetTypeDataPointer();
            gco->Flags = GCObjectFlags.Allocated;
            gco->AllocatedSize = requiredSize;
            gco->ActualSize = objectSize;
            sReservedObjectPointers[sReservedObjectCount++] = new IntPtr((void*)((byte*)gco + sizeof(GCObject)));
            return object.Internal_PointerToReference((void*)((byte*)gco + sizeof(GCObject)));
        }

        internal static object AllocateObject(Type.TypeData* typeData)
        {
            // TODO: Atomic_AquireLock(&sBusy)
            object obj = Allocate(typeData->Size);
            GCObject* gco = (GCObject*)((byte*)obj.Internal_ReferenceToPointer() - sizeof(GCObject));
            gco->TypeData = typeData;
            // TODO: Atomic_ReleaseLock(&sBusy)
            return obj;
        }

        internal static string AllocateString(uint pLength)
        {
            // TODO: Atomic_AquireLock(&sBusy)
            object obj = Allocate((uint)(sizeof(int) + pLength));
            GCObject* gco = (GCObject*)((byte*)obj.Internal_ReferenceToPointer() - sizeof(GCObject));
            gco->TypeData = typeof(string).GetTypeDataPointer();
            // TODO: Atomic_ReleaseLock(&sBusy)
            return (string)obj;
        }

        internal static Array AllocateArray(Type.TypeData* typeData, uint count)
        {
            uint elementSize = typeData->ArrayElementType->Size;
            if (!(new RuntimeType(new RuntimeTypeHandle(new IntPtr((void*)typeData->ArrayElementType))).IsValueType)) elementSize = (uint)sizeof(IntPtr);
            // TODO: Atomic_AquireLock(&sBusy)
            object obj = Allocate((uint)(sizeof(int) + (elementSize * count)));
            GCObject* gco = (GCObject*)((byte*)obj.Internal_ReferenceToPointer() - sizeof(GCObject));
            gco->TypeData = typeData;
            // TODO: Atomic_ReleaseLock(&sBusy)
            return (Array)obj;
        }
    }
}
