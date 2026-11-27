// The map's normal and skirmish sides and teams.
// The constructor at 0x0032EE24 builds SubsystemInterface at +0 and Snapshot
// at +0xC. It installs the primary table 0x0080DA98 and the secondary table
// 0x0080DA88. GameEngine's allocation at 0x0022E969 supplies 0x11B0 bytes.
#pragma once

#include "Common/Snapshot.h"
#include "Common/SubsystemInterface.h"
#include "GameLogic/SidesInfo.h"
#include <list>

class DataChunkOutput;

class SidesList : public SubsystemInterface, public Snapshot
{
public:
	SidesList();								// 0x0032EE24
	virtual ~SidesList();
	virtual void init() {}						// Slot 1: bare ret at 0x000B3FD0.
	virtual void reset();						// Slot 9: 0x0032E53D tails to clear().
	virtual void update() {}					// Slot 10: bare ret at 0x000B3FD0.

	virtual const char *GetSnapshotName() const;	// Snapshot slot 2: 0x0032ED53.
	virtual void xfer(Xfer *xfer);				// Snapshot slot 3: 0x00329AEA.

	SidesInfo *getSideInfo(Int index);			// Null for an out-of-range index.
	SidesInfo *getSkirmishSideInfo(Int index);	// 0x002A98D1: null for an out-of-range index.
	Int getNumSides() const { return m_numSides; }
	void emptySides();
	void emptyTeams();
	void duplicateScripts(const SidesList &other);
	void writeScripts(DataChunkOutput &chunkWriter);	// 0x0032996E
	SidesInfo *findSideInfo(AsciiString name, Int *index = 0);
	SidesInfo *findSkirmishSideInfo(AsciiString name, Int *index = 0);
	Bool validateAllyEnemyList(const AsciiString &teamName, AsciiString &allies);
	void clear();
	void discardOverriddenScriptsAndTeams();
	void removeSide(Int index);
	void removeSideAndTeams(Int index);

private:
	virtual void loadPostProcess() {}			// Snapshot slot 1: bare ret at 0x000B3FD0.

	UnsignedByte m_unk0x10[0x10];				// Constructed by 0x00330757, fields unconfirmed.
	_STL::list<Int> m_unk0x20;					// Typed list constructor 0x004EC36C.
	UnsignedByte m_unk0x24[0xC];					// Map constructed by 0x00413727, element type unrecovered.
	UnsignedByte m_unk0x30[0xC];					// Second map constructed by 0x00413727.
public:
	Int m_numSides;								// +0x3C, emptied by emptySides.
	SidesInfo m_sides[20];							// +0x40
	Int m_numSkirmishSides;						// +0x7C0
	SidesInfo m_skirmishSides[20];					// +0x7C4
	TeamsInfoRec m_teamRec;						// +0xF44, emptied by emptyTeams.
	TeamsInfoRec m_skirmishTeamsRec;				// +0xF60, also emptied by emptyTeams.
private:
	Bool m_unk0xF7C;								// Constructor initializes it to true.
	UnsignedInt m_unk0xF80[20][7];				// 20 records of 0x1C bytes, ctor 0x0032C19D.
};

typedef char SidesListSize[(sizeof(SidesList) == 0x11B0) ? 1 : -1];
typedef char SidesListSidesOffset[(offsetof(SidesList, m_sides) == 0x40) ? 1 : -1];
typedef char SidesListTeamsOffset[(offsetof(SidesList, m_teamRec) == 0xF44) ? 1 : -1];

extern SidesList *TheSidesList;					// 0x00E01D58

