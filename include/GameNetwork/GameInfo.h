// GameInfo.h: the shared state of a game being set up or played. Member and method names follow
// Zero Hour's GameInfo by role (evidence for names only); every offset below is retail's.
// Target facts (code RVAs, VA 0x00C193C8 table):
//  - Snapshot-derived, SIZE 0xDC (SkirmishGameInfo's constructor 0x0022C45B calls 0x00400A07 and
//    puts its eight GameSlots at +0xDC). Table (23 slots): 0 deleting dtor 0x003830CE, 1 the
//    empty loadPostProcess 0x000B3FD0, 2 the name getter 0x0037BB4D, 3 xfer 0x003FFA3C (the map
//    spells it SkirmishGameInfo::xfer; the table is GameInfo's), then the slots listed below.
//  - Constructor 0x00400A07: table, +4 = 0, +0x38 = 0, +0x3C = 0, +0x40 = 0, a Version (0x00638852)
//    at +0x90, 16 zero bytes at +0xCC, +0x18..+0x37 cleared (8 words), +0xC8 = 0, then reset
//    0x003FFF8F; destructor 0x00400A7F deletes the owned object at +0xC8 and runs the Version's
//    and the strings' destructors.
//  - reset 0x003FFF8F: +4 released, the Version rebuilt (0x0063870E), +0xC0/+0xC4 = 0,
//    +0xC = the global at VA 0x00DBC800 (the crc interval), +0x10/+0x11/+0x14 = 0, +0x40 = the
//    empty-map string (VA 0x00C381C4), +0x58 = -1, +0x4C = 0, +0x50 = a random seed (the call at
//    VA 0x00BBA294), +0x54 = -1, +0x12 = 0, +0x44 = +0x48 = 0, every slot reset through its
//    slot 4, +0x5C = -1, +8 = 0, +0x60 filled by MpGameRules::GetDefaults(-1, +0x60) (0x00959FAC,
//    ten Ints), +0x88 = the call at VA 0x00BBA50C, +0x8C = 0, the +0xC8 object deleted, +0xCC
//    16 bytes cleared.
//  - xfer 0x003FFA3C (version 1, skipped on a light CRC): Int +0xC, bytes +0x10/+0x11/+0x12,
//    Int +0x14, a count of 8 (Int), each slot's own xfer through its slot 3, the map name
//    (xferMapName 0x00706327), UnsignedInts +0x44/+0x48, Ints +0x4C/+0x50/+0x54/+0x5C and the
//    ten Ints at +0x60, UnsignedInt +0x88.
//  - Member names are Zero Hour's by role (preorder mask, crc interval, in game, in progress,
//    surrendered, game id, slots, local address, map name/CRC/size/mask, seed); the rest are
//    structural. Bit 9 of m_mapMask (+0x4D bit 1) is tested by 0x00300E42.
#pragma once
#ifndef BFME2_GAMEINFO_H
#define BFME2_GAMEINFO_H

#include <stddef.h>
#include "Common/GameType.h"
#include "Common/AsciiString.h"
#include "Common/UnicodeString.h"
#include "Common/Snapshot.h"
#include "GameNetwork/GameSlot.h"
#include "Common/Version.h"

class Image;
class LANGameInfo;
class GameSpyStagingRoom;
class Xfer;

enum { MAX_SLOTS = 8 };

// 0x003FF796: the index of a slot in TheGameInfo, -1 when it is not one of its slots.
Int getSlotIndex(const GameSlot *slot);

// The object GameInfo owns through +0xC8 (the destructor 0x00229840 is all the map knows of it).
// The rule set provider (0x00959FAC fills ten Ints with the defaults of a mode; -1 asks for the standard rules).
class MpGameRules
{
public:
	static void GetDefaults(Int mode, void *rules);	// 0x00559FAC
	static UnicodeString GetRuleString(Int rule);	// 0x0055A04C
};

class GameInfoOwned
{
public:
	~GameInfoOwned();							// 0x00229840

	UnsignedByte m_unk0x00[0x4C];				// +0x00..+0x4B: save-game metadata prefix before the saved slots
	GameSlot m_slots[MAX_SLOTS];				// +0x4C..+0xDAB: eight saved slots (DoAnyHeroTransfer 0x0044C3D4 restores hero data from here)
};

class GameInfo : public Snapshot
{
public:
	GameInfo();									// 0x00400A07
	virtual ~GameInfo();						// 0x00400A7F (deleting 0x003830CE)

protected:
	virtual void loadPostProcess() {}			// slot 1, 0x000B3FD0
public:
	virtual const char *GetSnapshotName() const;	// slot 2, 0x0037BB4D
protected:
	virtual void xfer(Xfer *xfer);				// slot 3, 0x003FFA3C
public:
	virtual LANGameInfo *slot4() { return 0; }			// slot 4, 0x000D43D0 (name not evidenced)
	virtual GameSpyStagingRoom *slot5() { return 0; }			// slot 5, 0x000D43D0 (name not evidenced)
	virtual const Image *rva00401015(Int kind);	// slot 6, 0x00401015: looks up a lobby status image by name: kind 1 = "AptLobbyNonDefaultSettings" when the rules (+0x60) differ from MpGameRules::GetDefaults; kind 0 = "AptLobbyResumeSavedGame" or "AptUserMapNotConquered" (strings at VA 0x00C3826C / 0x00C38254) by map; name structural (retail has none)
	virtual Int slot7() const { return 2; }		// slot 7, 0x0022C4CB (name not evidenced)
	virtual Int slot8() const;					// slot 8, 0x0022C4CF: tail-calls getNumPlayers
	virtual Int slot9() const;					// slot 9, 0x0022C4D4: tail-calls getNumOpenOrOccupiedSlots
	virtual void reset();						// slot 10, 0x003FFF8F
	virtual void startGame(Int gameID);			// slot 11, 0x003FF271: +0x14 = gameID, closes open slots (slot 17) in mode 3, in progress
	virtual Bool amIHost() const;				// slot 12, 0x003FF312
	virtual Int getLocalSlotNum() const;		// slot 13, 0x003FF2D6
	virtual void resetAccepted();				// slot 14, 0x003FF9B4
	virtual void resetStartSpots();				// slot 15, 0x003FF39E
	virtual void adjustSlotsForMap();			// slot 16, 0x0040052C
	virtual void closeOpenSlots();				// slot 17, 0x003FFE51
	virtual Bool isSkirmish();					// slot 18, 0x003FF9E2
	virtual Bool isMultiPlayer();				// slot 19, 0x003FF3B5
	virtual Bool isSandbox();					// slot 20, 0x003FF3E3
	virtual Int slot21() { return 0; }			// slot 21, 0x000D43D0 (name not evidenced)
	virtual UnicodeString getGameName() const;	// slot 22, 0x0022C4DF: returns the string at +4

	void clearSlotList();						// 0x003FFDB7
	void setSlot(Int slotNum, GameSlot slot);	// 0x003FFDFA
	GameSlot *getSlot(Int slotNum);				// 0x003FF29F
	const GameSlot *getConstSlot(Int slotNum) const;	// 0x003FF2BE
	void setSlotPointer(Int slotNum, GameSlot *slot);	// 0x003FF332
	Int getSlotNum(AsciiString userName) const;	// 0x0040009A
	Int getNumPlayers() const;					// 0x003FF218
	Int getNumNonObserverPlayers() const;		// 0x003FF23D
	Int getNumOpenOrOccupiedSlots() const;		// 0x003FF987
	Bool rva003FF457() const;					// 0x003FF457 (true when every occupied slot with a hero kind has its hero data)
	void setGameRulesPreset(Int preset);						// stores the selector at +0x5C and refills the rules block when it changes
	Bool hasSavedGameId() const;					// true when the 16-byte saved-game id at +0xCC is non-zero
	Bool isHeroDataReadyForSlot(UnsignedShort slotNum) const;	// 0x003FF496
	Bool isColorTaken(Int colorIndex, Int slotToIgnore) const;	// 0x003FF34A
	Bool isStartPositionTaken(Int positionIndex, Int slotToIgnore) const;	// 0x003FF374
	Bool isPlayerPreorder(Int index);			// 0x003FF1DA
	void markPlayerAsPreorder(Int index);		// 0x003FF1FE
	void enterGame();							// 0x003FF268
	void endGame();								// 0x003FF296
	void setSeed(Int seed);						// 0x003FF328
	AsciiString getMap() const;					// 0x0023E943
	void setMap(AsciiString mapName);			// 0x00400126
	void setMapCRC(UnsignedInt crc);				// 0x00400E9F
	void setMapSize(UnsignedInt size);			// 0x00400F5A
	Int getMaxPlayers();						// 0x0040203C
	void setMapForwarder(AsciiString mapName);	// 0x00447B9D (LANGameInfo.cpp's copy of the call, named GameInfo's by the map)
	Bool isGameInProgress() const { return m_inProgress; }	// +0x11
	Int getSeed() const { return m_seed; }					// +0x50

	UnicodeString m_gameName;					// +0x04 (slot 22 returns it)
	UnsignedInt m_preorderMask;					// +0x08
	Int m_crcInterval;							// +0x0C
	Bool m_inGame;								// +0x10
	Bool m_inProgress;							// +0x11
	Bool m_surrendered;							// +0x12
	unsigned char m_pad0x13;					// +0x13
	Int m_gameID;								// +0x14
	GameSlot *m_slot[MAX_SLOTS];				// +0x18
	BfmeNetAddress m_localAddress;				// +0x38 (the local peer's address; two padding bytes at +0x3E)
	AsciiString m_mapName;						// +0x40
	UnsignedInt m_mapCRC;						// +0x44
	UnsignedInt m_mapSize;						// +0x48
	UnsignedInt m_mapMask;						// +0x4C
	Int m_seed;									// +0x50
	Int m_unk0x54;								// +0x54 (-1 after reset)
	Int m_unk0x58;								// +0x58 (-1 after reset)
	Int m_unk0x5C;								// +0x5C (-1 after reset; selects the image in slot 6)
	Int m_gameRules[10];						// +0x60 filled by MpGameRules::GetDefaults(-1, ...)
	Int m_unk0x88;								// +0x88
	Bool m_unk0x8C;								// +0x8C
	unsigned char m_pad0x8D[3];					// +0x8D
	Version m_version;							// +0x90 (ctor 0x00238852; Common/Version.h)
	Int m_unk0xC0;								// +0xC0
	Int m_unk0xC4;								// +0xC4
	GameInfoOwned *m_unk0xC8;					// +0xC8 an owned object (0x00229840 destroys it)
	unsigned char m_unk0xCC[0x10];				// +0xCC
};
// SIZE 0xDC
typedef char GameInfoSize[(sizeof(GameInfo) == 0xDC) ? 1 : -1];
typedef char GameInfoSlotOffset[(offsetof(GameInfo, m_slot) == 0x18) ? 1 : -1];
typedef char GameInfoMapNameOffset[(offsetof(GameInfo, m_mapName) == 0x40) ? 1 : -1];
typedef char GameInfoRulesOffset[(offsetof(GameInfo, m_gameRules) == 0x60) ? 1 : -1];
typedef char GameInfoVersionOffset[(offsetof(GameInfo, m_version) == 0x90) ? 1 : -1];
typedef char GameInfoTailOffset[(offsetof(GameInfo, m_unk0xCC) == 0xCC) ? 1 : -1];

extern GameInfo *TheGameInfo;
extern Int NET_CRC_INTERVAL;	// frames between the network CRC checks

AsciiString GameInfoToAsciiString(const GameInfo *game, Bool saveMapName);	// 0x00400AF8
Bool ParseAsciiStringToGameInfo(GameInfo *game, AsciiString options, Bool saveMapName);	// 0x0040116B

#endif
