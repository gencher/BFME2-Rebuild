// Debug.h: the debug library's manager (Code\Libraries\Source\debug, the library
// Zero Hour ships as debug_debug.h). BFME 2 keeps one heap instance, theDebug
// (RVA 0x009E0880): PreStaticInit 0x000385F0 allocates 0x9F68 bytes through
// DebugAllocMemory, runs the constructor 0x00038440 and bumps +0x9C7C.
//
// Unlike Zero Hour, every stream operator and most entry points are virtual:
// vftable VA 0x00BBE810, 46 slots. MSVC 7.1 lays out same-name overloads in
// reverse declaration order, so the operator<< overloads below are declared
// in reverse slot order (slot 0 is HResult, slot 15 RepeatChar). Slots 10/12
// (unsigned int/unsigned long) and 11/13 (int/long) share one body each.
// Names: Zero Hour where the slot's body does what that member does; the
// others are marked structural.
//
// Layout (constructor 0x00038440 stores, the REP STOSD that clears 10007
// frame-hash pointers at +0x18 and seven 16-byte output buffers at +0x9C84,
// and the accesses listed per member). Allocation 0x9F68.
#pragma once
#ifndef BFME2_DEBUG_H
#define BFME2_DEBUG_H

#include "debug/DebugStackwalk.h"
#include "debug/DebugIOInterface.h"

class DebugCmdInterface;

class Debug
{
	friend class DebugCmdInterfaceDebug;
	friend class DebugExceptionhandler;

public:
	enum { MAX_CHECK_HITS = 20 };

	typedef bool (*HResultTranslator)(Debug &debug, long hresult, void *user);

	// Zero Hour's memory dump descriptor (debug_debug.h): operator<<(const MemDump &) 0x000389C0 reads it at +0 (start), +4 (items), +8 (bytes per
	// item), +0xC (absolute addresses) and +0xD (characters on the right).
	class MemDump
	{
		friend class Debug;

		const unsigned char *m_startPtr;
		unsigned m_numItems;
		unsigned m_bytePerItem;
		bool m_absAddr;
		bool m_withChars;

		// private on purpose: instances come from the static functions below
		MemDump(const void *ptr, unsigned num, unsigned bpi, bool absAddr, bool withChars) :
			m_startPtr((const unsigned char *)ptr), m_numItems(num), m_bytePerItem(bpi), m_absAddr(absAddr), m_withChars(withChars) {}

	public:
		static MemDump Raw(const void *startPtr, unsigned numItems, unsigned bytePerItem = 1) { return MemDump(startPtr, numItems, bytePerItem, true, false); }
		static MemDump RawRel(const void *startPtr, unsigned numItems, unsigned bytePerItem = 1) { return MemDump(startPtr, numItems, bytePerItem, false, false); }
		static MemDump Char(const void *startPtr, unsigned numItems, unsigned bytePerItem = 1) { return MemDump(startPtr, numItems, bytePerItem, true, true); }
	};
	// Zero Hour's fill-character stream descriptor (slot 15 takes it by reference; the fields are read at +0 and +4).
	class RepeatChar
	{
		friend class Debug;
		char m_char;
		int m_count;
	public:
		explicit RepeatChar(char ch, int count) : m_char(ch), m_count(count) {}
	};

	// Zero Hour's HRESULT stream descriptor (slot 0 takes it by value).
	class HResult
	{
		friend class Debug;
		long m_hresult;
	public:
		explicit HResult(long hresult) : m_hresult(hresult) {}
	};

	// printf-style formatter: ctor 0x000386E0 formats into m_buffer with limit
	// 0x1FF; callers reserve 0x200 bytes. Not a vtable slot.
	class Format
	{
		friend class Debug;
		Format(const Format &);
		Format &operator=(const Format &);
		char m_buffer[512];
	public:
		explicit Format(const char *format, ...);
		operator const char *() const { return m_buffer; }
	};

	// slots 15..0, declared in reverse
	virtual Debug &operator<<(const RepeatChar &value);			// 15: 0x0003A620
	virtual Debug &operator<<(const char *str);					// 14: 0x0003B340
	virtual Debug &operator<<(int val);							// 13: 0x0003B3D0
	virtual Debug &operator<<(unsigned int val);				// 12: 0x0003B420
	virtual Debug &operator<<(long val);						// 11: 0x0003B3D0
	virtual Debug &operator<<(unsigned long val);				// 10: 0x0003B420
	virtual Debug &operator<<(bool val);						//  9: 0x00038890
	virtual Debug &operator<<(float val);						//  8: 0x000388B0
	virtual Debug &operator<<(double val);						//  7: 0x00038900
	virtual Debug &operator<<(short val);						//  6: 0x0003B470
	virtual Debug &operator<<(unsigned short val);				//  5: 0x0003B4C0
	virtual Debug &operator<<(__int64 val);						//  4: 0x0003B510
	virtual Debug &operator<<(unsigned __int64 val);			//  3: 0x0003B570
	virtual Debug &operator<<(const void *ptr);					//  2: 0x00038950
	virtual Debug &operator<<(const MemDump &dump);				//  1: 0x000389C0
	virtual Debug &operator<<(HResult hres);					//  0: 0x00038C30

	virtual ~Debug();											// 16: ??_G 0x00039920, dtor 0x00038710

	virtual bool AssertDone(int mode);							// 17: 0x0003A8A0
	virtual bool LogDone();										// 18: 0x000387F0
	virtual bool CrashDone(int mode);							// 19: 0x0003AED0
	virtual void SetPrefixAndRadix(const char *prefix, int radix);	// 20: 0x00038850 (+0x9E7C, +0x9E8C)
	virtual void WriteBuildInfo();								// 21: 0x000395D0
	virtual void InstallExceptionHandler();						// 22: 0x00038770
	virtual bool frameStatus(unsigned int address, bool reset);	// 23: 0x0003A6A0
	virtual bool SkipNext();									// 24: 0x000387D0 (bumps +0x9DF8)
	virtual Debug &CheckBegin(const char *file, int line, int reserved, const char *expr);	// 25: 0x0003A750
	virtual Debug &LogBegin(const char *fileOrGroup);			// 26: 0x0003AC60
	virtual Debug &CrashBegin(const char *file, int line, int reserved);	// 27: 0x0003AD80
	virtual bool isLogEnabled(unsigned int address);			// 28: 0x0003B5D0
	virtual bool isInAssertDialog();							// 29: 0x00038CB0 returns +0x9F64; name: structural
	virtual void AddHResultTranslator(unsigned int prio, HResultTranslator func, void *user);	// 30: 0x00038CC0
	virtual void RemoveHResultTranslator(HResultTranslator func, void *user);	// 31: 0x00038D60
	virtual bool AddIOFactory(const char *ioID, const char *descr, DebugIOInterface *(*factory)());	// 32: 0x00038DF0
	virtual bool AddCommands(const char *cmdgroup, DebugCmdInterface *cmdif);	// 33: 0x00038E40
	virtual void RemoveCommands(DebugCmdInterface *cmdif);		// 34: 0x00038EA0
	virtual bool Command(const char *cmd);						// 35: 0x0003B640
	virtual void ExecuteFrameCommands(int frame);				// 36: 0x0003B670
	virtual void Update();										// 37: 0x0003B700
	virtual int GetInput(char *buf, int size, bool *structured);	// 38: 0x00038EE0
	virtual void SetBuildInfo(const char *version, const char *internalVersion, const char *buildDate);	// 39: 0x00039510
	virtual const char *GetVersion();							// 40: 0x000395A0 returns m_version; name: structural
	virtual const char *GetInternalVersion();					// 41: 0x000395B0 returns m_intVersion; name: structural
	virtual const char *GetBuildDate();							// 42: 0x000395C0 returns m_buildDate; name: structural
	virtual bool IsWindowed();									// 43: 0x000397D0 reads +0x9F55
	virtual void SetWindowed(bool windowed);					// 44: 0x00039880 stores 1/-1 at +0x9F55; name: structural
	virtual const char *AddLogGroup(const char *fileOrGroup, const char *descr);	// 45: 0x000390D0

	Debug &operator<<(const Format &value) { return operator<<(value.m_buffer); }

	// Zero Hour's stream manipulators (debug_debug.h): each is an inline operator<< that sets the radix, the width or the fill character.
	class Hex {};
	Debug &operator<<(Hex &) { SetPrefixAndRadix("0x", 16); return *this; }
	class Dec {};
	Debug &operator<<(Dec &) { SetPrefixAndRadix("", 10); return *this; }
	class Bin {};
	Debug &operator<<(Bin &) { SetPrefixAndRadix("%", 2); return *this; }
	class Width
	{
		friend class Debug;
		int m_widthValue;
	public:
		explicit Width(int width) : m_widthValue(width) {}
	};
	Debug &operator<<(Width &w) { m_width = w.m_widthValue; return *this; }
	class FillChar
	{
		friend class Debug;
		char m_fill;
	public:
		explicit FillChar(char ch = ' ') : m_fill(ch) {}
	};
	Debug &operator<<(FillChar &c) { m_fillChar = c.m_fill; return *this; }

	static bool SkipNext(bool set);								// 0x00038790
	static bool SimpleMatch(const char *str, const char *pattern);	// 0x00039480

	enum { FRAME_HASH_SIZE = 10007, FRAME_HASH_ALLOC_COUNT = 100 };
	enum { FrameTypeAssert = 1, FrameTypeCheck = 2, FrameTypeLog = 4 };
	enum FrameStatus { Unknown, Skip, NoSkip };

	struct HResultTranslatorEntry							// 0xC bytes
	{
		unsigned int prio;
		HResultTranslator func;
		void *user;
	};
	struct FrameHashEntry										// 0x1C bytes (AddFrameEntry 0x00038FF0)
	{
		FrameHashEntry *next;
		unsigned int frameAddr;
		unsigned int frameType;
		const char *fileOrGroup;
		int line;
		int hits;
		int status;
	};
	struct FrameCommand											// 8 bytes (ScheduleFrameCommand 0x00039720)
	{
		int frame;
		const char *command;
	};
	struct IOFactoryListEntry
	{
		IOFactoryListEntry *next;
		const char *ioID;
		const char *descr;
		DebugIOInterface *(*factory)();
		DebugIOInterface *io;
		char *input;
		unsigned int inputUsed;
		unsigned int inputAlloc;
	};
	struct CmdInterfaceListEntry
	{
		CmdInterfaceListEntry *next;
		const char *group;
		DebugCmdInterface *cmdif;
	};
	struct KnownLogGroupList
	{
		KnownLogGroupList *next;
		char *nameGroup;
		const char *descr;
	};
	struct PatternListEntry
	{
		PatternListEntry *next;
		unsigned int frameTypes;
		bool isActive;
		char *pattern;
	};
	struct IOBuffer
	{
		char *buffer;
		unsigned int used;
		unsigned int alloc;
		bool lastWasCR;
	};

	HResultTranslatorEntry *hrTranslators;			// +0x0004 name: donor (ZH); 12-byte entries (AddHResultTranslator 0x00038CC0, RemoveHResultTranslator 0x00038D60)
	unsigned int numHrTranslators;					// +0x0008
	IOFactoryListEntry *firstIOFactory;				// +0x000C
	CmdInterfaceListEntry *firstCmdGroup;			// +0x0010
	unsigned int curStackFrame;						// +0x0014 (a static in Zero Hour)
	FrameHashEntry *frameHash[FRAME_HASH_SIZE];		// +0x0018
	FrameHashEntry *nextUnusedFrameHash;			// +0x9C74
	unsigned int numAvailableFrameHash;				// +0x9C78
	int refCount;									// +0x9C7C PreStaticInit increments, StaticExit decrements
	KnownLogGroupList *firstLogGroup;				// +0x9C80
	IOBuffer ioBuffer[7];							// +0x9C84
	int curType;									// +0x9CF4 7 (no string type) when idle
	char curSource[256];							// +0x9CF8
	int disableAssertsEtc;							// +0x9DF8
	FrameHashEntry *curFrameEntry;					// +0x9DFC
	PatternListEntry *firstPatternEntry;			// +0x9E00
	PatternListEntry *lastPatternEntry;				// +0x9E04
	FrameCommand *frameCommands;					// +0x9E08 ScheduleFrameCommand/ExecuteFrameCommands
	unsigned int numFrameCommands;					// +0x9E0C
	unsigned int frameCommandsAlloc;				// +0x9E10
	char curCommandGroup[100];						// +0x9E14
	bool alwaysFlush;								// +0x9E78
	bool noStackWalk;								// +0x9E79 set from 0x0003E3E0 in the constructor
	bool timeStamp;									// +0x9E7A
	DebugStackwalk m_stackWalk;						// +0x9E7B ctor 0x0003C0E0 runs on it
	char m_prefix[16];								// +0x9E7C
	int m_radix;									// +0x9E8C 10 after construction
	char m_version[64];								// +0x9E90
	char m_intVersion[64];							// +0x9ED0
	char m_buildDate[64];							// +0x9F10
	int m_width;									// +0x9F50
	char m_fillChar;								// +0x9F54 ' ' after construction
	signed char m_windowed;							// +0x9F55 1, -1 or 0 (unknown)
	bool fullMiniDump;								// +0x9F56
	const char *GetOutputBuffer(DebugIOInterface::StringType type) const;			// 0x000392A0 (reads the current output buffer; the type is not used)
	bool logErrorsToFile;							// +0x9F57
	bool noAssertDialog;							// +0x9F58
	void *m_mutex;									// +0x9F5C LogBegin waits on it
	unsigned int m_threadId;						// +0x9F60
	bool inAssertDialog;							// +0x9F64

private:
	FrameHashEntry *LookupFrame(unsigned int addr);									// 0x00038330
	FrameHashEntry *GetFrameEntry(unsigned int addr, unsigned int type, const char *fileOrGroup, int line);	// 0x0003A1B0
	FrameHashEntry *AddFrameEntry(unsigned int addr, unsigned int type, const char *fileOrGroup, int line);	// 0x00038FF0
	void UpdateFrameStatus(FrameHashEntry &entry);									// 0x00039A60
	void AddPatternEntry(unsigned int types, bool isActive, const char *pattern);	// 0x000393E0
	unsigned int ScheduleFrameCommand(int frame, const char *command);				// 0x00039720
	bool ExecCommand(const char *cmdstart, const char *cmdend);						// 0x00039E20
	void FlushOutput(bool defaultLog = true);									// 0x00039300 (the map spells it private virtual; it is not in the vtable)
	void StartOutput(DebugIOInterface::StringType type, const char *fmt, ...);	// 0x00039B30 (likewise)
	void AddOutput(const char *str, unsigned int len);							// 0x00039B70 (map spells it private virtual: ?AddOutput@Debug@@EAEXPBDI@Z)

	Debug();										// 0x00038440
	Debug(const Debug &);
	Debug &operator=(const Debug &);
	static Debug *PreStaticInit();					// 0x000385F0
	static void PostStaticInit();
	static void StaticExit();						// 0x000399C0
};
// SIZE 0x9F68
typedef char DebugSize[(sizeof(Debug) == 0x9F68) ? 1 : -1];

extern Debug *theDebug;								// RVA 0x009E0880

// Streams a string's text: retail's out-of-line COMDAT 0x000028E8 (??$?6D@@YAAAVDebug@@AAV0@ABV?$StringBase@D@@@Z)
// is `mov eax, [s]; ... debug << s.str()` through slot 14 (operator<<(const char *)); KeepObjectDie's
// constructor 0x00486D73 calls it with the template name.
template <class CharT> class StringBase;
template <class CharT> Debug &operator<<(Debug &debug, const StringBase<CharT> &text) { return debug << text.str(); }

// Streams a position: retail's 0x000050BA (the pathfinder's path dump calls it for each node).
class Coord3D;
Debug &operator<<(Debug &debug, const Coord3D &pos);

#endif
