// GameState.h: the save-game subsystem (global TheGameState): it owns the lists of
// Snapshot objects that a save writes, plus the save-file info record.
// SIZE 0xE1C. Target facts (retail game.dat):
//  - Bases: SubsystemInterface at +0 (ctor 0x002DE4E6 calls 0x001B4E63 first) and
//    Snapshot at +0xC. Primary vtable 0x0080402C (0x00C0402C): slot 0 deleting dtor
//    0x002DF2DA, 1 init 0x002DE641, 9 reset 0x002DEE9A, 10 update (empty), the rest the
//    base's. The Snapshot table 0x0080401C: slot 0 the this-adjusting deleting
//    destructor 0x002DE585, 1 loadPostProcess (empty, folded), 2 GetSnapshotName
//    0x002DE57F, 3 xfer 0x002DBCCA (which transfers the info record at +0x24 as a
//    Snapshot).
//  - +0x10: five lists of SnapshotBlock, one per snapshot type (the ctor builds the
//    array with the vector-constructor iterator, size 4, count 5; xferSaveData
//    0x002DCE24 and findBlockInfoByToken 0x002DCDBF index it by the type).
//  - +0x24: the save-game info record (a Snapshot subclass, constructor 0x002DD173,
//    0xDE8 bytes), +0xE0C: list of snapshots to post-process, +0xE10: list of
//    (snapshot, flag) pairs (addPostProcessSnapshot 0x002DCF6C), +0xE14 an int
//    cleared by the ctor, +0xE18 a flag cleared by the ctor and reset().
#pragma once
#ifndef BFME2_GAMESTATE_H
#define BFME2_GAMESTATE_H

#include "Common/GameType.h"
#include "Common/AsciiString.h"
#include "Common/UnicodeString.h"
#include "Common/SubsystemInterface.h"
#include "Common/Snapshot.h"
#include "Common/StlList.h"

class Xfer;

// Zero Hour's spelling; the map's mangled name `W4SnapshotType` fixes the enum's name.
enum SnapshotType
{
	SNAPSHOT_SAVELOAD = 0,
	SNAPSHOT_TYPE_1 = 1,
	SNAPSHOT_TYPE_2 = 2,
	SNAPSHOT_TYPE_3 = 3,
	SNAPSHOT_TYPE_4 = 4
};

// A save game's timestamp (SaveGameInfo +0x10, 0x10 bytes): the fields of a Win32 SYSTEMTIME.
// isNewerThan 0x002DBB9A compares them most significant first and skips the day of the week.
struct SaveDate
{
	SaveDate();														// 0x002DB9CB (map: Rva002DB9CB): clears the eight fields
	UnsignedShort year;												// 0x0
	UnsignedShort month;											// 0x2
	UnsignedShort day;												// 0x4
	UnsignedShort dayOfWeek;										// 0x6
	UnsignedShort hour;												// 0x8
	UnsignedShort minute;											// 0xA
	UnsignedShort second;											// 0xC
	UnsignedShort milliseconds;										// 0xE

	Bool isNewerThan(SaveDate *other);								// 0x002DBB9A
};
// SIZE 0x10

// The save-file info record at GameState +0x24: a Snapshot subclass of 0xDE8 bytes
// (vtable 0x00BE7560, constructor 0x002DD173, destructor 0x002DD1E9, xfer 0x002DDEC0).
// Only its Snapshot base is established here.
class SaveGameInfo : public Snapshot
{
public:
	SaveGameInfo();													// 0x002DD173 (map: BfmeSubobject0022CE19)
	virtual ~SaveGameInfo();										// 0x002DD1E9

protected:
	virtual void loadPostProcess();
	virtual const char *GetSnapshotName() const;
	virtual void xfer(Xfer *xfer);									// 0x002DDEC0

public:
	// Members from the constructor 0x002DD173 and destructor 0x002DD1E9 (the strings are the
	// ones they release); roles are not established except where a row proves one.
	AsciiString m_unk0x4;											// 0x04
	AsciiString m_unk0x8;											// 0x08 cleared by friend_xferSaveDataForCRC (0x002DDD0B)
	AsciiString m_unk0xC;											// 0x0C
	UnsignedByte m_unk0x10[0x10];									// 0x10 a timestamp (ctor 0x002DB9CB)
	UnicodeString m_unk0x20;										// 0x20 cleared by friend_xferSaveDataForCRC
	Int m_unk0x24;													// 0x24 zeroed by friend_xferSaveDataForCRC
	Int m_unk0x28;													// 0x28
	AsciiString m_unk0x2C;											// 0x2C cleared by friend_xferSaveDataForCRC
	UnicodeString m_unk0x30;										// 0x30
	UnicodeString m_unk0x34;										// 0x34
	UnsignedByte m_unk0x38[0xC];									// 0x38 a vector (destructor 0x0022CAC4)
	UnsignedByte m_unk0x44[0xDA4];									// 0x44 the embedded game setup record (ctor 0x002DC5DD)
};
// SIZE 0xDE8

struct SaveGameEntry
{
	SaveGameEntry();
	SaveGameEntry(const SaveGameEntry &other);
	~SaveGameEntry();
	SaveGameEntry &operator=(const SaveGameEntry &other);

	UnicodeString filename;
	SaveGameInfo info;
	Int unk0xDEC;
	Int unk0xDF0;
};
typedef char SaveGameEntrySize[(sizeof(SaveGameEntry) == 0xDF4) ? 1 : -1];

// The label-and-size XferEnum call GameState's save-info xfer makes for the auto-save flag (0x002DB9EF).
Xfer &XferIsAutoSaveOrNot(Xfer *xfer, void *value);					// 0x002DB9EF (map: Rva002DB9EFGet)

class GameState;
// Callbacks the load transfer calls with its GameState and a sequence number (0x002DC66F, 0x002DD908).
void removePostProcessSnapshotsCallback(void *unused, GameState *state, UnsignedInt sequence);	// 0x002DC66F
void addPostProcessSnapshotCallback(Snapshot *snapshot, GameState *state, UnsignedInt sequence);	// 0x002DD908

class GameState : public SubsystemInterface, public Snapshot
{
public:
	// One registered snapshot: the object and the token it is saved under.
	struct SnapshotBlock
	{
		Snapshot *snapshot;											// 0x0
		AsciiString name;											// 0x4
	};

	GameState();													// 0x002DE4E6
	virtual ~GameState();											// 0x002DE58D (scalar-deleting 0x002DF2DA)

	virtual void init();											// slot 1, 0x002DE641
	virtual void reset();											// slot 9, 0x002DEE9A
	virtual void update() {}										// slot 10, empty (folded)

	void rva002DE311();												// 0x002DE311 (map: Rva002DE311Owner), called by reset and the dtor
	AsciiString realMapPathToPortableMapPath(const AsciiString &in) const;	// 0x002DC833
	AsciiString portableMapPathToRealMapPath(const AsciiString &in) const;	// 0x002DC9F7
	AsciiString packPortableMapPath(const AsciiString &in) const;			// 0x002DCB9C
	AsciiString unpackPortableMapPath(const AsciiString &in) const;			// 0x002DC340
	UnicodeString getSaveDirectory() const;									// 0x002DC267 (map: Rva002DC267): the user data directory plus "Save\\"
	UnicodeString getFilePathInSaveDirectory(const UnicodeString &leaf) const;	// 0x002DC74A (map: Rva002DC74A)
	Bool doesSaveGameExist(UnicodeString fileName);							// 0x002DCCFB (map: Rva002DCCFB): the save file exists in the save directory
	Bool isInSaveDirectory(const UnicodeString &path) const;				// 0x002DC7C1 (map: Rva002DC7C1)
	SaveGameInfo *getSaveGameInfo() { return &m_saveGameInfo; }	// Zero Hour's accessor (GameLogic::loadMapINI reads +0x24 + 8)
	const UnsignedShort *getSaveFileExtension(Int mode);					// 0x002DBC97 (map: rva002DBC97Get@GameState): the file name suffix of a save mode

	void xferSaveData(Xfer *xfer, SnapshotType which);				// 0x002DCE24
	AsciiString getMapLeafName(const AsciiString &path) const;		// 0x002DC802
	void friend_xferSaveDataForCRC(Xfer *xfer, SnapshotType which);	// 0x002DDD0B
	void addPostProcessSnapshot(Snapshot *snapshot, UnsignedInt flag);	// 0x002DCF6C
	void removePostProcessSnapshotsSince(void *unused, UnsignedInt sequence);	// 0x002DC4B8 (map: rva002DC4B8@GameState): drops the entries recorded at or after the sequence
	void gameStatePostProcessLoad();								// 0x002DC52B
	Int saveGame(UnicodeString filename, const UnicodeString &description, Int which, Bool showMessage, Int isAutoSave);	// 0x002DD38D
	Int doLoadGame(SaveGameEntry entry);
	void saveGameManual(const UnicodeString &filename, const UnicodeString &description, Int which, Bool showMessage);	// 0x002DDE43 (map: rva002DDE43@GameState): saveGame with isAutoSave 0
	void populateSaveGameListbox(class GameWindow *saveList, class GameWindow *autoSaveList, bool saving, int kind);	// 0x002DF3B0 (map: ?populateSaveGameListbox@GameState@@QAEXPAVGameWindow@@0_NH@Z)

protected:
	virtual void loadPostProcess() {}								// Snapshot slot 1, empty (folded)
	virtual const char *GetSnapshotName() const;					// Snapshot slot 2, 0x002DE57F
	virtual void xfer(Xfer *xfer);									// Snapshot slot 3, 0x002DBCCA

private:
	SnapshotBlock *findBlockInfoByToken(AsciiString token, SnapshotType which);	// 0x002DCDBF
	void addSnapshotBlock(AsciiString token, Snapshot *snapshot, SnapshotType which);	// 0x002DE34B

	struct PostProcessEntry
	{
		Snapshot *snapshot;											// 0x0
		UnsignedInt flag;											// 0x4
	};

	_STL::list<SnapshotBlock> m_snapshotLists[5];					// 0x10 one list per snapshot type
	SaveGameInfo m_saveGameInfo;									// 0x24 the save-file info record
	_STL::list<Snapshot *> m_postProcessList;						// 0xE0C snapshots to post-process (addPostProcessSnapshot)
	_STL::list<PostProcessEntry> m_postProcessPairs;				// 0xE10 the same snapshots with their flag
	Int m_unk0xE14;													// 0xE14
	Bool m_unk0xE18;												// 0xE18
};
// SIZE 0xE1C
extern GameState *TheGameState;										// VA 0x00DFF08C

#endif
