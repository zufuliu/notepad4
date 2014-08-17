#define NP2_USE_HEAP_FUNC	1
#define NP2_USE_GLOBAL_FUNC	1

#if NP2_USE_HEAP_FUNC
#define NP2MemAlloc(size)			HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size)
#define NP2MemReAlloc(hMem, size)	HeapReAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, hMem, size)
#define NP2MemAllocM				NP2MemAlloc
#define NP2MemReAllocM				NP2MemReAlloc
#define NP2MemFree(hMem)			HeapFree(GetProcessHeap(), 0, hMem)
#define NP2MemLock(hMem)			HeapLock(hMem)
#define NP2MemUnlock(hMem)			HeapUnlock(hMem)
#elif NP2_USE_GLOBAL_FUNC
#define NP2MemAlloc(size)			GlobalAlloc(GPTR, size)	// fixed pointer
#define NP2MemReAlloc(hMem, size)	GlobalReAlloc(hMem, size, LPTR)
#define NP2MemAllocM(size)			GlobalAlloc(GHND, size)	// movable handle
#define NP2MemReAllocM(hMem, size)	GlobalReAlloc(hMem, size, GHND)
#define NP2MemFree(hMem)			GlobalFree(hMem)
#define NP2MemLock(hMem)			GlobalLock(hMem)
#define NP2MemUnlock(hMem)			GlobalUnlock(hMem)
#else
#define NP2MemAlloc(size)			LocalAlloc(LPTR, size)	// fixed pointer
#define NP2MemReAlloc(hMem, size)	LocalReAlloc(hMem, size, LPTR)
#define NP2MemAllocM(size)			LocalAlloc(LHND, size)	// movable handle
#define NP2MemReAllocM(hMem, size)	LocalReAlloc(hMem, size, LHND)
#define NP2MemFree(hMem)			LocalFree(hMem)
#define NP2MemLock(hMem)			LocalLock(hMem)
#define NP2MemUnlock(hMem)			LocalUnlock(hMem)
#endif
