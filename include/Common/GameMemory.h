// GameMemory.h: BFME 2's allocator front end, the MemoryPool namespace over EA's
// general allocator (EA::Allocator::GeneralAllocator), and how game objects are
// allocated and freed.
//
// What retail has (game.dat 1.06):
//  - No Zero Hour memory pools. There is no MemoryPoolObject vtable, no
//    getObjectMemoryPool() slot and no class-specific operator new/delete in game
//    code (only Apt defines its own). Weapon's single vftable 0x0080214C is
//    Snapshot's four slots; GameMessage's 0x00809840 holds only the destructor.
//  - Every `new` goes through the global operators below, which call through the
//    function pointers MemoryPool::InitFunctionPointers (0x00030730) fills with
//    GetProcAddress on the game's own exports (the 13 MemoryPool:: names below are
//    retail export names).
//  - Zero Hour's `p->deleteInstance()` sites compile as `::delete p`: the delete
//    site null-tests p, calls the virtual destructor slot with flag 0 and passes
//    the returned pointer to the global operator delete 0x0002FD60 (436 sites,
//    e.g. FireWeaponWhenDamagedBehavior::~ 0x00482551 and GameMessage::~
//    0x0030F4AF). Write `::delete p;` there; MessageStream.cpp's rows already
//    MATCH that way. A plain `delete p` calls the destructor slot with flag 1.
//
// Inline against out of line:
//  - operator new 0x0002FDA0 (3,400 call sites), operator delete 0x0002FD60
//    (5,832), operator new[] 0x0002FDE0 and delete[] 0x0002FD80 are out of line.
//  - The MemoryPool:: functions are only reached through the pointers.
//
// AllocType values (from the callers): 1 operator new, 2 operator new[];
// 0 and 3 are the two C-runtime replacement families (0x0002FC60/0x0002FC10 pass
// 0; allocateBlock 0x000307F0, reallocateBlock 0x00030810 and freeBlock
// 0x00030830 pass 3). Enumerator names are structural; retail only fixes the type
// name MemoryPool::AllocType.
#pragma once
#ifndef BFME2_GAMEMEMORY_H
#define BFME2_GAMEMEMORY_H

#include <stdio.h>

namespace EA
{
namespace Allocator
{

// One heap-report record (ReportNext's result). Offsets from
// MemoryPool::_DumpFragmentation; this older allocator has no block pointer at +4.
struct BlockInfo
{
	enum BlockType
	{
		kBlockTypeAllocated = 2,
		kBlockTypeFree = 4,
		kBlockTypeCore = 8,
		kBlockTypeAll = 0x1F
	};

	const void *mpCore;									// +0x00
	unsigned int mnBlockSize;							// +0x04
	void *mpData;										// +0x08
	unsigned int mnDataSize;							// +0x0C
	char mBlockType;									// +0x10 a BlockType value
	bool mbMemoryMapped;								// +0x11
};

// EA's general-purpose heap (PPMalloc), an older version than the public 1.26.11:
// not virtual, no hooks, two pointers per bin. Layout from the constructor's
// default stores (0x00033B90) and the accessors; member names follow 1.26.11's
// where the use matches ("(?)" marks a name the code does not pin down).
// Most rows of the library are still spelled rvaXXXXXXXX in the map.
class GeneralAllocator
{
public:
	typedef bool (*HeapReportFunction)(const BlockInfo *info, void *context);
	typedef unsigned int (*CoreFreeFunction)(GeneralAllocator *allocator, void *core, unsigned int size, void *context);
	typedef bool (*MallocFailureFunction)(GeneralAllocator *allocator, unsigned int requestedSize,
		unsigned int allocatorRequestedSize, void *context);
	typedef void (*TraceFunction)(const char *text, void *context);
	typedef void (*AssertionFailureFunction)(const char *expression, void *context);

	struct Chunk
	{
		unsigned int mnPriorSize;						// +0x0
		unsigned int mnSize;							// +0x4 size | flags: 1 prior in use, 2 mmapped, 4 internal, 0x80000000 fast bin
		Chunk *mpNextChunk;								// +0x8
		Chunk *mpPrevChunk;								// +0xC
	};

	// One block of core memory; the list head is mHeadCoreBlock.
	struct CoreBlock
	{
		char *mpCore;									// +0x00
		unsigned int mnSize;							// +0x04
		unsigned int mnReservedSize;					// +0x08 FreeCore 0x000316B0 frees this many bytes
		bool mbMMappedMemory;							// +0x0C (?)
		bool mbShouldFree;								// +0x0D FreeCore
		bool mbShouldFreeOnShutdown;					// +0x0E FreeCore when shutting down
		bool mbShouldTrim;								// +0x0F (?)
		CoreFreeFunction mpCoreFreeFunction;			// +0x10
		void *mpCoreFreeFunctionContext;				// +0x14
		CoreBlock *mpPrevCoreBlock;						// +0x18
		CoreBlock *mpNextCoreBlock;						// +0x1C
	};

	// 0x00033B90 (comdat). Not virtual: no vtable store.
	GeneralAllocator(void *core, unsigned int coreSize, bool shouldFreeCore, bool shouldTrimCore,
		CoreFreeFunction coreFreeFunction, void *coreFreeContext);
	void SetAssertionFailureFunction(AssertionFailureFunction function, void *context);	// 0x000313E0
	void *ReportBegin(void *context, int blockTypeFlags, bool makeCopy, void *storage,
		unsigned int storageSize);												// 0x000353B0
	const BlockInfo *ReportNext(void *context, int blockTypeFlags);				// 0x00032F60
	void ReportEnd(void *context);												// 0x00033E90
	static unsigned int GetLargeBinIndexFromChunkSize(unsigned int chunkSize);		// 0x00030F10

	void *Malloc(unsigned int size, int flags);									// 0x00035080
	void *Realloc(void *p, unsigned int size, int flags);						// 0x00035190
	void *Calloc(unsigned int count, unsigned int size, int flags);				// 0x00035210; _Allocate passes (size, 1, 0)
	void Free(void *p);															// 0x000338F0
	bool ValidateAddress(const void *p, int addressType);						// 0x00032830
	bool containsAddress(const void *p);										// 0x00032920; name: structural
	bool ValidateHeap(int level);												// 0x000329E0
	unsigned int GetUsableSize(const void *p);									// 0x00032A20
	bool reportHeap(HeapReportFunction report, void *context, int blockTypes,
		bool makeCopy, char *storage, unsigned int storageSize);				// 0x00035540

	bool mbInitialized;									// +0x000
	unsigned int mnMaxFastBinChunkSize;				// +0x004 bit 0: fast bin chunks exist
	Chunk *mpFastBinArray[10];							// +0x008
	Chunk *mpBinArray[256];								// +0x030 two pointers per bin (GetBin 0x00030BF0)
	unsigned int mBinBitmap[4];							// +0x430
	Chunk *mpTopChunk;									// +0x440
	Chunk *mpLastRemainderChunk;						// +0x444
	CoreBlock mHeadCoreBlock;							// +0x448
	void *mpHighFence;									// +0x468
	bool mbHighFenceInternallyDisabled;				// +0x46C (?)
	bool mbSystemAllocEnabled;							// +0x46D (?) constructor default true
	int mnCheckChunkReentrancyCount;					// +0x470 (?)
	unsigned char mcTraceFieldDelimiter;				// +0x474 default 9 (tab)
	unsigned char mcTraceRecordDelimiter;				// +0x475 default 10 (line feed)
	int mAutoHeapValidationLevel;						// +0x478
	unsigned int mnAutoHeapValidationFrequency;			// +0x47C
	unsigned int mnAutoHeapValidationEventCount;		// +0x480
	bool mbHeapValidationActive;						// +0x484
	int mnMMapCount;										// +0x488
	unsigned int mnMMapMallocTotal;						// +0x48C
	int mnMMapMaxAllowed;								// +0x490
	unsigned int mnMMapThreshold;						// +0x494
	bool mbMMapTopDown;									// +0x498 MEM_TOP_DOWN for mapped blocks
	Chunk mHeadMMapChunk;								// +0x49C
	MallocFailureFunction mpMallocFailureFunction;		// +0x4AC
	void *mpMallocFailureFunctionContext;				// +0x4B0
	unsigned int mnMaxMallocFailureCount;				// +0x4B4 default 0x100
	AssertionFailureFunction mpAssertionFailureFunction;	// +0x4B8 default 0x000327D0
	void *mpAssertionFailureFunctionContext;			// +0x4BC
	TraceFunction mpTraceFunction;						// +0x4C0 default 0x00031420
	void *mpTraceFunctionContext;						// +0x4C4
	unsigned int mnTrimThreshold;						// +0x4C8
	unsigned int mnTopPad;								// +0x4CC
	char *mpInitialTopChunk;							// +0x4D0
	unsigned int mnPageSize;							// +0x4D4 default 0x1000
	unsigned int mnNewCoreSize;							// +0x4D8 default 16 MB
	unsigned int mnCoreIncrementSize;					// +0x4DC default 4 MB
	bool mbTraceInternalMemory;							// +0x4E0
	void *mpMutex;										// +0x4E4 locked by every public call
	unsigned int mpMutexData[8];						// +0x4E8 CRITICAL_SECTION and lock count
	unsigned char mnFillFree;							// +0x508 0xDD
	unsigned char mnFillDelayedFree;					// +0x509 0xDE
	unsigned char mnFillNew;							// +0x50A 0xCD
	unsigned char mnFillGuard;							// +0x50B 0xAB
	unsigned char mnFillUnusedCore;						// +0x50C 0xFE
};
// SIZE 0x510 (MemoryPool::_Init allocates this much for each allocator)
typedef char GeneralAllocatorSize[(sizeof(GeneralAllocator) == 0x510) ? 1 : -1];

}
}

namespace MemoryPool
{

enum AllocType
{
	ALLOC_TYPE_PLAIN = 0,		// name: structural
	ALLOC_TYPE_NEW = 1,			// operator new 0x0002FDA0
	ALLOC_TYPE_NEW_ARRAY = 2,	// operator new[] 0x0002FDE0
	ALLOC_TYPE_CRT = 3			// name: structural (CRT free/realloc replacements)
};

// Retail exports, Code\Libraries\Source\memorypool\memorypool_allocator.cpp per
// WorldBuilder; game.dat links them in GameMemory.cpp (O00030000).
void _Init();																	// 0x00030000
void _Exit();																	// 0x000301A0
void _DumpFragmentation(FILE *file, bool verbose, const char *title);			// 0x00030250
EA::Allocator::GeneralAllocator *_GetHeapAllocator(unsigned int heapId);		// 0x000304B0
EA::Allocator::GeneralAllocator *_GetHeapAllocatorByIndex(unsigned int index);	// 0x00030500
void *_Allocate(unsigned int size, AllocType type, unsigned int heapId);		// 0x00030520
void *_Reallocate(void *p, unsigned int size, AllocType type, unsigned int heapId);	// 0x00030560
void _Free(void *p, AllocType type);											// 0x00030590
bool _IsValidBlock(void *p, unsigned int heapId);								// 0x000305E0
unsigned int _GetBlockSize(void *p, unsigned int heapId);						// 0x00030600
unsigned int _GetBlockHeap(void *p);											// 0x00030630
void _VerifyIntegrity();														// 0x00030680
void _AddHeap(unsigned int heapId, unsigned int size);							// 0x000306B0
void InitFunctionPointers();													// 0x00030730
// Map placeholder names (O00030000): a heap-report callback summing data sizes,
// the total-allocated walk that uses it, and the switch _Free checks at exit.
bool rva00030900(const EA::Allocator::BlockInfo *info, void *context);
unsigned int rva00030910();
void rva00030970();

// One registered heap: hashed by id into HeapTable::buckets (101 buckets).
struct HeapEntry
{
	HeapEntry *next;									// +0x0
	unsigned int id;									// +0x4
	unsigned int size;									// +0x8 _AddHeap's second argument
	EA::Allocator::GeneralAllocator *allocator;		// +0xC
};

// The front end's state (data symbol MemoryPool::g_heaps, 0x410 bytes at
// RVA 0x009E0414). Offsets from the accesses in GameMemory.cpp.
struct HeapTable
{
	void *heap;											// +0x000 Win32 heap (HeapCreate(5, 0x10000, 0)) holding the allocators
	int lastHeap;										// +0x004 heaps[0..lastHeap] are live
	unsigned int unk0x008;								// +0x008
	HeapEntry entries[30];								// +0x00C _AddHeap stops at 30
	HeapEntry *buckets[101];							// +0x1EC keyed by id % 101
	unsigned int unk0x380;								// +0x380
	EA::Allocator::GeneralAllocator *heaps[31];		// +0x384 heaps[0] is the default heap; heaps[i + 1] is entries[i]'s
	EA::Allocator::GeneralAllocator *defaultHeap;		// +0x400 _Init sets it to heaps[0]; name: structural
	bool clearAllocations;								// +0x404 _Allocate uses Calloc when set
	unsigned char unk0x405[3];							// +0x405
	int refCount;										// +0x408 _Init/_Exit nesting count; name: structural
	bool exiting;										// +0x40C _Free does nothing once set
	bool initialized;									// +0x40D
	unsigned char unk0x40E[2];							// +0x40E
};
// SIZE 0x410
typedef char HeapTableSize[(sizeof(HeapTable) == 0x410) ? 1 : -1];

extern HeapTable g_heaps;					// RVA 0x009E0414
extern unsigned int g_defaultHeapSize;		// RVA 0x009B35A0
extern unsigned long g_heapTlsIndex;		// RVA 0x009B35A4, -1 until _Init

}

// The heaps' clear-allocations switch (the game report prints it as zeroFillMemory).
unsigned char Rva000308D0GetByte();

// The function pointers InitFunctionPointers fills (one per export). Names are
// the data ledger's; the allocate and free pointers carry C linkage in retail
// (___gameMemAllocatePtr, ___gameMemFreePtr).
extern "C" void *(__cdecl *__gameMemAllocatePtr)(unsigned int, MemoryPool::AllocType, unsigned int);	// 0x009E0404
extern "C" void (__cdecl *__gameMemFreePtr)(void *, MemoryPool::AllocType);							// 0x009E03FC
extern void (__cdecl *__gameMemInitPtr)();																// 0x009E0410
extern void (__cdecl *__gameMemExitPtr)();																// 0x009E040C
extern void (__cdecl *__gameMemDumpFragmentationPtr)(FILE *, bool, const char *);						// 0x009E0408
extern void *(__cdecl *__gameMemReallocatePtr)(void *, unsigned int, MemoryPool::AllocType, unsigned int);	// 0x009E0400
extern bool (__cdecl *__gameMemIsValidBlockPtr)(void *, unsigned int);								// 0x009E03F8
extern unsigned int (__cdecl *__gameMemGetBlockSizePtr)(void *, unsigned int);						// 0x009E03F4
extern unsigned int (__cdecl *__gameMemGetBlockHeapPtr)(void *);										// 0x009E03F0
extern void (__cdecl *__gameMemVerifyIntegrityPtr)();
// The ledger names this one g_Va00DE03E8; spelled like its neighbours here.
extern void (__cdecl *__gameMemAddHeapPtr)(unsigned int, unsigned int);	// 0x009E03E8
extern EA::Allocator::GeneralAllocator *(__cdecl *__gameMemGetHeapAllocatorPtr)(unsigned int);		// 0x009E03E4
extern EA::Allocator::GeneralAllocator *(__cdecl *__gameMemGetHeapAllocatorByIndexPtr)(unsigned int);	// 0x009E03E0

// The C runtime's malloc, realloc and free replaced (GameMemory.cpp): blocks of the CRT allocation
// type, from the given heap (0 is the default heap). They are ordinary game functions, not the
// runtime's, so they carry their own names; msvcr71.dll still supplies malloc and free themselves.
void *allocateBlock(unsigned int size, unsigned int heap);
void *reallocateBlock(void *block, unsigned int size, unsigned int heap);
void freeBlock(void *block);
// The C runtime's strdup replaced (no null test): the copy comes from allocateBlock.
char *duplicateString(const char *string);

#endif
