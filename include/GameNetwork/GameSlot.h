// GameSlot.h: one player slot of a game lobby or skirmish setup. Member and method names follow
// Zero Hour's GameSlot by role (evidence for names only); every offset below is retail's.
// Target facts (code RVAs, VA 0x00BE7420 table):
//  - Snapshot-derived, SIZE 0x1AC: SkirmishGameInfo's ctor 0x0022C45B builds eight of them at
//    +0xDC with a 0x1AC element stride (vector constructor iterator over 0x003FFB4C and
//    destructor 0x002294FD). Table (7 slots): 0 deleting dtor 0x002296EC, 1 the shared empty
//    loadPostProcess 0x000B3FD0, 2 typeName 0x002296E6 (the Snapshot name getter), 3 xfer
//    0x003FF616, 4 reset 0x003FF50C, 5 and 6 the shared `xor eax,eax; ret` 0x000D43D0
//    (LANGameSlot's table replaces slot 5 with the shared `mov eax,ecx; ret` 0x0047A6A9).
//    Slots 5 and 6 have no evidenced name.
//  - Constructor 0x003FFB4C stores +0x30/+0x34 = 0, +0x38 = 0, the word at +0x3C = 0, builds the
//    CreateAHeroData at +0x64 (ctor 0x00809C3D), clears +0x1A8 and calls reset. Copy constructor
//    0x002295D7 copies every member in order; destructor 0x002294FD releases +0x1A8, the hero
//    data, +0x34 and +0x30.
//  - reset 0x003FF50C: +4 = 1 (closed), +8 = 0, +9 = 1, +0xA = 0, +0xC/+0x10/+0x14 = -1,
//    +0x18 = -2 (observer), +0x1C = -1, +0x20 = 0, +0x24/+0x28/+0x2C = -1, the strings at +0x30
//    and +0x34 released, +0x38 = 0, +0x3C = 0, +0x40 = 1, +0x44 = 0, +0x48 = 0, +0x4C = -1,
//    +0x50 = +0x54 = +0x58 = 0, +0x5C = -1, +0x60 = 0, the hero data reassigned from a default
//    CreateAHeroData, +0x1A4 = 1, +0x1A8 set to the empty string.
//  - xfer 0x003FF616 (version 2; skipped on a light CRC): +4 through XferSlotState (0x003FF0B4),
//    bytes +8/+9/+0xA, Ints +0xC..+0x20, then (loading) +0x2C = +0x18, +0x28 = +0x10,
//    +0x24 = +0xC, Ints +0x24/+0x28/+0x2C, UnicodeString +0x30, AsciiString +0x34, the
//    LivingWorldPlayer id at +0x4C (0x006034C4), UnsignedInts +0x54/+0x58, Int +0x5C, byte +0x60,
//    (version >= 2) Int +0x50, the hero data's own xfer (its slot 3), byte +0x1A4, AsciiString
//    +0x1A8. That makes +0x24/+0x28/+0x2C the "original" color / start position / player
//    template that saveOffOriginalInfo 0x003FF0D4 records; getApparentColor 0x003FF838,
//    getApparentStartPos 0x003FF870 and getApparentPlayerTemplate 0x003FF813 return the original
//    values for a slot that is not a local ally while the game's flags at +0x1D/+0x1F are set.
//  - State values from isOpen 0x0029B257 (+4 == 0), isHuman 0x003FF0F1 (== 6), isAI 0x003FF127
//    (2..5), isOccupied 0x003FF0FB (6 or 2..5, and +0x1A4 set), isObserver 0x003FF165
//    (+0x18 == -2). +0x38 is a BfmeNetAddress (an address word and a port word; isPlayer
//    0x003FF145 compares it through 0x00248CBF).
#pragma once
#ifndef BFME2_GAMESLOT_H
#define BFME2_GAMESLOT_H

#include <stddef.h>
#include "Common/GameType.h"
#include "Common/AsciiString.h"
#include "Common/UnicodeString.h"
#include "GameLogic/CreateAHeroData.h"
#include "Common/Snapshot.h"
#include "GameNetwork/BfmeNetAddress.h"

class Xfer;
class LANGameSlot;
class GameSpyGameSlot;
class CreateAHeroData;

// Zero Hour's SlotState order. Retail distinguishes 2..5 only as "AI" (isAI); the level names
// are Zero Hour's.
enum SlotState
{
	SLOT_OPEN = 0,
	SLOT_CLOSED = 1,
	SLOT_EASY_AI = 2,
	SLOT_MED_AI = 3,
	SLOT_HARD_AI = 4,
	SLOT_BRUTAL_AI = 5,
	SLOT_PLAYER = 6
};

// 0x0056BD91: packs four small values into one hero byte (the map spells it Rva0056BD91Pack).
UnsignedByte Rva0056BD91Pack(UnsignedByte first, UnsignedByte second, char third, char fourth);

// The address a slot's state change is told about (setState 0x003FFC28's third argument; an address word and a port word).
struct GameSlotConnectInfo
{
	UnsignedInt m_ip;
	UnsignedShort m_port;
};

// The hero data is a Snapshot; slot 3 is its xfer.
struct HeroSnapshotView
{
	virtual void slot0();
	virtual void slot1();
	virtual void slot2();
	virtual void xfer(Xfer *xfer);
};

class GameSlot : public Snapshot
{
public:
	GameSlot();									// 0x003FFB4C
	GameSlot(const GameSlot &that);				// 0x002295D7 (COMDAT)
	GameSlot &operator=(const GameSlot &that);	// 0x002DBAB9 (out of line; the map has no better name)
	virtual ~GameSlot();						// 0x002294FD (deleting 0x002296EC)

protected:
	virtual void loadPostProcess() {}			// slot 1, 0x000B3FD0
public:
	virtual const char *GetSnapshotName() const;	// slot 2, 0x002296E6
public:
	virtual void xfer(Xfer *xfer);				// slot 3, 0x003FF616
public:
	virtual void reset();						// slot 4, 0x003FF50C
	virtual LANGameSlot *slot5() { return 0; }			// slot 5, 0x000D43D0 (name not evidenced)
	virtual GameSpyGameSlot *slot6() { return 0; }			// slot 6, 0x000D43D0 (name not evidenced)

	Bool isOpen() const { return m_state == SLOT_OPEN; }			// 0x0029B257
	Bool isHuman() const;											// 0x003FF0F1 (out of line in GameInfo.cpp: other units call it, the callers in that file inline it)
	Bool isOccupied() const;										// 0x003FF0FB
	Bool isAI() const;												// 0x003FF127
	Bool isObserver() const;										// 0x003FF165 (out of line, like isHuman)
	Bool isPlayer(const BfmeNetAddress *address) const;				// 0x003FF145
	Bool isPlayer(UnicodeString userName) const;					// 0x003FFF62
	Bool isPlayer(AsciiString userName) const;						// 0x003FFEF5
	void setState(SlotState state, UnicodeString name, const GameSlotConnectInfo *info);	// 0x003FFC28
	Bool hasMap() const { return m_hasMap; }						// +9, the map-availability flag
	Int getStartPos() const { return m_startPos; }
	Int getColor() const { return m_color; }
	Int getPlayerTemplate() const { return m_playerTemplate; }
	Int getTeamNumber() const { return m_teamNumber; }
	Int getHandicap() const { return m_unk0x20; }
	Int getLivingWorldPlayerID() const { return m_livingWorldPlayerID; }
	const UnicodeString &getName() const { return m_name; }
	void saveOffOriginalInfo();										// 0x003FF0D4
	UnsignedByte encodeHero() const;								// 0x003FF16F (packs the hero kind and its two ids into one byte)
	UnicodeString getApparentPlayerTemplateDisplayName() const;	// 0x003FFBB4 (the template name the lobby shows: Random or Observer where the slot hides it)
	Int getApparentPlayerTemplate() const;							// 0x003FF813
	Int getApparentColor() const;									// 0x003FF838
	Int getApparentStartPos() const;								// 0x003FF870
	void setPlayerTemplate(Int playerTemplate);						// 0x00400E33
	void rva0037AD8D(const CreateAHeroData &hero);				// 0x0037AD8D: copies the hero choice into the slot (AptSkirmish::InitCreateAHeroOnStartGame 0x005218AF)
	void setSerial(const AsciiString &serial);						// 0x003FF5F2 (stores the serial trimmed and upper-cased at +0x1A8)
	void unAccept();												// 0x003FF895
	void setMapAvailability(Bool hasMap);							// 0x003FF8A0

	SlotState m_state;							// +0x04 (reset: closed)
	Bool m_isAccepted;							// +0x08
	Bool m_hasMap;								// +0x09 (reset: true)
	Bool m_isMuted;								// +0x0A
	unsigned char m_pad0x0B;					// +0x0B
	Int m_color;								// +0x0C
	Int m_startPos;								// +0x10
	Int m_unk0x14;								// +0x14 (-1 after reset)
	Int m_playerTemplate;						// +0x18 (-2 = observer)
	Int m_teamNumber;							// +0x1C (Zero Hour's member order; -1 after reset)
	Int m_unk0x20;								// +0x20
	Int m_origColor;							// +0x24
	Int m_origStartPos;							// +0x28
	Int m_origPlayerTemplate;					// +0x2C
	UnicodeString m_name;						// +0x30
	AsciiString m_unk0x34;						// +0x34
	BfmeNetAddress m_address;					// +0x38 (the peer's address; two padding bytes at +0x3E)
	Int m_unk0x40;								// +0x40 (1 after reset)
	Int m_unk0x44;								// +0x44
	Bool m_unk0x48;								// +0x48
	unsigned char m_pad0x49[3];					// +0x49
	Int m_livingWorldPlayerID;					// +0x4C (XferLivingWorldPlayerID; -1 after reset)
	Int m_heroKind;								// +0x50 (version 2; encodeHero 0x003FF16F switches on 0..3)
	UnsignedInt m_unk0x54;						// +0x54
	UnsignedInt m_unk0x58;						// +0x58
	Int m_unk0x5C;								// +0x5C (-1 after reset)
	Bool m_unk0x60;								// +0x60
	unsigned char m_pad0x61[3];					// +0x61
	CreateAHeroData m_heroData;				// +0x64 (constructed by 0x00409C3D in GameSlot::GameSlot 0x003FFB4C, assigned via 0x00409359 in reset 0x003FF50C)
	Bool m_unk0x1A4;							// +0x1A4 (1 after reset; isOccupied needs it)
	unsigned char m_pad0x1A5[3];				// +0x1A5
	AsciiString m_unk0x1A8;						// +0x1A8
};
// SIZE 0x1AC
typedef char GameSlotSize[(sizeof(GameSlot) == 0x1AC) ? 1 : -1];
typedef char GameSlotNameOffset[(offsetof(GameSlot, m_name) == 0x30) ? 1 : -1];
typedef char GameSlotHeroOffset[(offsetof(GameSlot, m_heroData) == 0x64) ? 1 : -1];
typedef char GameSlotTailOffset[(offsetof(GameSlot, m_unk0x1A8) == 0x1A8) ? 1 : -1];

#endif
