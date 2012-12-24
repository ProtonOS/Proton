using System;
using System.Collections.Generic;

namespace System.Kernel
{
	internal unsafe static class HeapAllocator
	{
		internal struct PageOfHeaps
		{
			public ulong PageSize;
			public GC.GCHeap* Heaps;
			public ulong TotalHeaps;
			public PageOfHeaps* Prev;
			public PageOfHeaps* Next;
			public GC.GCHeap* AllocatedFirst;
			public GC.GCHeap* AllocatedLast;
			public GC.GCHeap* AvailableFirst;
			public GC.GCHeap* AvailableLast;

			public void LinkHeapToAllocated(GC.GCHeap* pHeap)
			{
				if (AllocatedLast == null)
				{
					pHeap->AllocatorPrev = null;
					pHeap->AllocatorNext = null;
					AllocatedFirst = pHeap;
					AllocatedLast = pHeap;
				}
				else
				{
					AllocatedLast->AllocatorNext = pHeap;
					pHeap->AllocatorPrev = AllocatedLast;
					pHeap->AllocatorNext = null;
					AllocatedLast = pHeap;
				}
			}
			public void UnlinkHeapFromAllocated(GC.GCHeap* pHeap)
			{
				if (pHeap->AllocatorPrev == null) AllocatedFirst = pHeap->AllocatorNext;
				else pHeap->AllocatorPrev->AllocatorNext = pHeap->AllocatorNext;
				if (pHeap->AllocatorNext == null) AllocatedLast = pHeap->AllocatorPrev;
				else pHeap->AllocatorNext->AllocatorPrev = pHeap->AllocatorPrev;
			}
			public void LinkHeapToAvailable(GC.GCHeap* pHeap)
			{
				if (AvailableLast == null)
				{
					pHeap->AllocatorPrev = null;
					pHeap->AllocatorNext = null;
					AvailableFirst = pHeap;
					AvailableLast = pHeap;
				}
				else
				{
					AvailableLast->AllocatorNext = pHeap;
					pHeap->AllocatorPrev = AvailableLast;
					pHeap->AllocatorNext = null;
					AvailableLast = pHeap;
				}
			}
			public void UnlinkHeapFromAvailable(GC.GCHeap* pHeap)
			{
				if (pHeap->AllocatorPrev == null) AvailableFirst = pHeap->AllocatorNext;
				else pHeap->AllocatorPrev->AllocatorNext = pHeap->AllocatorNext;
				if (pHeap->AllocatorNext == null) AvailableLast = pHeap->AllocatorPrev;
				else pHeap->AllocatorNext->AllocatorPrev = pHeap->AllocatorPrev;
			}
		}

		private static PageOfHeaps* PageOfHeapsFirst = null;
		private static PageOfHeaps* PageOfHeapsLast = null;

		private static void Panic() { while (true) ; }

		private static PageOfHeaps* AllocatePageOfHeaps()
		{
			ulong pageSize = PageAllocator.MinimumPageSize;
			ulong pageAddress = PageAllocator.Allocate(ref pageSize);
			PageOfHeaps* pageOfHeaps = (PageOfHeaps*)pageAddress;
			pageOfHeaps->PageSize = pageSize;
			pageOfHeaps->Heaps = (GC.GCHeap*)(pageAddress + (ulong)sizeof(PageOfHeaps));
			pageOfHeaps->TotalHeaps = (pageSize - (ulong)sizeof(PageOfHeaps)) / (ulong)sizeof(GC.GCHeap);

			pageOfHeaps->Next = null;
			pageOfHeaps->Prev = PageOfHeapsLast;
			if (PageOfHeapsLast != null) PageOfHeapsLast->Next = pageOfHeaps;
			else PageOfHeapsFirst = pageOfHeaps;
			PageOfHeapsLast = pageOfHeaps;

			pageOfHeaps->AllocatedFirst = null;
			pageOfHeaps->AllocatedLast = null;
			pageOfHeaps->AvailableFirst = pageOfHeaps->Heaps;
			pageOfHeaps->AvailableLast = &pageOfHeaps->Heaps[pageOfHeaps->TotalHeaps - 1];
			for (ulong heapIndex = 0; heapIndex < pageOfHeaps->TotalHeaps; ++heapIndex)
			{
				bool lastHeap = heapIndex == (pageOfHeaps->TotalHeaps - 1);
				GC.GCHeap* heap = &pageOfHeaps->Heaps[heapIndex];
				heap->PageOfHeaps = pageOfHeaps;
				if (lastHeap) heap->AllocatorNext = null;
				else heap->AllocatorNext = &pageOfHeaps->Heaps[heapIndex + 1];
				if (heapIndex == 0) heap->AllocatorPrev = null;
				else heap->AllocatorPrev = &pageOfHeaps->Heaps[heapIndex - 1];
				heap->HeapSize = 0;
				heap->Heap = null;
				heap->TreeSize = 0;
				heap->Tree = null;
				heap->TreeLevels = 0;
				heap->AllocatedFirst = null;
				heap->AllocatedLast = null;
			}
			return pageOfHeaps;
		}

		private static void DeallocatePageOfHeaps(PageOfHeaps* pPage)
		{
			if (pPage->Prev == null) PageOfHeapsFirst = pPage->Next;
			else pPage->Prev->Next = pPage->Next;
			if (pPage->Next == null) PageOfHeapsLast = pPage->Prev;
			else pPage->Next->Prev = pPage->Prev;
			PageAllocator.Deallocate((ulong)pPage, pPage->PageSize);
		}

		internal static GC.GCHeap* AllocateHeap(ulong pHeapSize, bool pSingleObject)
		{
			// TODO: Make thread-safe
			GC.GCHeap* heap = null;
			PageOfHeaps* pageOfHeaps = PageOfHeapsLast;
			while (pageOfHeaps != null)
			{
				if (pageOfHeaps->AvailableFirst != null)
				{
					heap = pageOfHeaps->AvailableFirst;
					break;
				}
				pageOfHeaps = pageOfHeaps->Prev;
			}
			if (heap == null)
			{
				pageOfHeaps = AllocatePageOfHeaps();
				heap = pageOfHeaps->AvailableFirst;
			}
			pageOfHeaps->UnlinkHeapFromAvailable(heap);
			pageOfHeaps->LinkHeapToAllocated(heap);

			heap->Heap = (byte*)PageAllocator.Allocate(ref pHeapSize);
			heap->HeapSize = pHeapSize;
			if (pSingleObject)
			{
				heap->TreeSize = 0;
				heap->Tree = null;
				heap->TreeLevels = 1;
			}
			else
			{
				int shiftsForHeapSize = 0;
				while ((pHeapSize & ((ulong)1 << shiftsForHeapSize)) == 0) ++shiftsForHeapSize;
				heap->TreeLevels = (byte)((shiftsForHeapSize - GC.ShiftsForMinimumObjectSize) + 1);
				ulong bytesRequiredForTree = ((ulong)1 << (byte)heap->TreeLevels) >> 3;
				heap->Tree = (uint*)PageAllocator.Allocate(ref bytesRequiredForTree);
				heap->TreeSize = bytesRequiredForTree;
				ulong treeElementCount = bytesRequiredForTree >> 2;
				for (ulong index = 0; index < treeElementCount; ++index) heap->Tree[index] = 0;
			}
			heap->AllocatedFirst = null;
			heap->AllocatedLast = null;

			return heap;
		}

		internal static void DeallocateHeap(GC.GCHeap* pHeap)
		{
			// TODO: Make thread-safe
			PageAllocator.Deallocate((ulong)pHeap->Heap, pHeap->HeapSize);
			pHeap->HeapSize = 0;
			pHeap->Heap = null;
			if (pHeap->Tree != null)
			{
				PageAllocator.Deallocate((ulong)pHeap->Tree, pHeap->TreeSize);
				pHeap->TreeSize = 0;
				pHeap->Tree = null;
				pHeap->TreeLevels = 0;
			}
			pHeap->AllocatedFirst = null;
			pHeap->AllocatedLast = null;
			pHeap->PageOfHeaps->UnlinkHeapFromAllocated(pHeap);
			pHeap->PageOfHeaps->LinkHeapToAvailable(pHeap);
			if (pHeap->AllocatedFirst == null)
			{
				// TODO: Threshold for DeallocatePageOfHeaps
			}
		}
	}
}
