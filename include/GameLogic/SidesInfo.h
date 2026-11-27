// SidesInfo: one side (player slot) of a map's SidesList: its build list, its player
// dictionary, its scripts and a list of strings. SIZE 0x60.
// Retail evidence: SidesList's constructor 0x0032EE24 builds 20 of them with stride 0x60
// (eh vector constructor with 0x0032C667 / 0x0032D4C0). The constructor clears the
// build-list head at +0, builds a Dict(0) at +4, a ScriptList at +8 and an empty vector
// at +0x54; the destructor 0x0032D4C0 calls init(NULL), then destroys the vector
// (vector<AsciiString>::~vector 0x0002CC70), the ScriptList and the Dict (releaseData);
// init 0x0032CDAA deletes the build list, clears the Dict, the scripts and the vector
// (vector<AsciiString>::erase 0x0002CCFC) and copies the given Dict. The build list is a
// chain through BuildListInfo +0x2C (addToBuildList 0x003297FB). No vtable.
#pragma once
#ifndef BFME2_SIDESINFO_H
#define BFME2_SIDESINFO_H

#include "Common/GameType.h"
#include "Common/AsciiString.h"
#include "Common/Dict.h"
#include "Common/Vector.h"
#include <map>
#include <utility>
#include "GameLogic/ScriptList.h"
#include "Common/StaticNameKey.h"

class BuildListInfo;

class SidesInfo
{
public:
	SidesInfo();												// 0x0032C667
	SidesInfo(const SidesInfo &that);							// 0x0032C6AD
	~SidesInfo();
	SidesInfo &operator=(const SidesInfo &that);				// 0x0032D510

	void init(const Dict *dict);
	void clear();
	void swap(SidesInfo *other);
	void addToBuildList(BuildListInfo *info, Int position);
	Int removeFromBuildList(BuildListInfo *info);
	void reorderInBuildList(BuildListInfo *info, Int position);
	Dict *getDict() { return &m_dict; }

	BuildListInfo *m_pBuildList;								// +0x00
	Dict m_dict;												// +0x04
	ScriptList m_scripts;										// +0x08
	_STL::vector<AsciiString> m_unk0x54;						// +0x54
};
// SIZE 0x60
typedef char SidesInfoSize[(sizeof(SidesInfo) == 0x60) ? 1 : -1];

// The team Dict keys (retail's StaticNameKey objects for "teamOwner" and "teamName").
extern const StaticNameKey TheKey_teamOwner;
extern const StaticNameKey TheKey_teamName;

// TeamsInfoRec: a map's team records (SidesList keeps two, +0xF44 and +0xF60). SIZE 0x1C.
// Retail evidence: its constructor 0x0032CD6D builds the index map at +0 and a one-slot team
// vector at +0x0C (vector(size_type) 0x0032BDE7) and zeroes the shorts at +0x18/+0x1A; clear
// 0x0032C9C6 swaps the vector with a fresh one-slot one (swap 0x00567ECD), clears the map and
// the two shorts; swap 0x0032B651 swaps the map (0x0032AC92), the vector and the two shorts.
// The index is keyed by (owner, name) AsciiString pairs: its tree erase 0x0032B4ED destroys the
// pair<AsciiString, AsciiString> key, and 0x0032D103 inserts make_pair(pair, int) entries.
typedef _STL::pair<AsciiString, AsciiString> TeamNameKey;
typedef _STL::map<TeamNameKey, Int> TeamIndex;

// TeamsInfo: one team's Dict (Zero Hour's wrapper). SidesList::isPlayerDefaultTeam reads the
// Dict at +0 of its TeamsInfo argument; the team slot below embeds one at +0xC, and the slot's
// destructor only destroys that Dict.
class TeamsInfo
{
public:
	TeamsInfo() : m_dict(0) {}
	void init(const Dict *d);
	void clear() { m_dict.clear(); }
	Dict *getDict() { return &m_dict; }

private:
	Dict m_dict;
};

// One team slot (16 bytes): links of the slot lists (+0/+2), the slot it overrides and the one
// overriding it (+4/+6), its index entry (+8) and the team (+0xC). Slot 0 is the active list
// sentinel. The original name is unknown; the ledger's vector instantiations spell the class
// BfmeThingUBB, and its default constructor is out of line.
class BfmeThingUBB
{
public:
	BfmeThingUBB();
	Dict *getDict() { return m_team.getDict(); }
	short m_next;					// +0x00
	short m_prev;					// +0x02
	short m_overridesID;			// +0x04
	short m_overriddenByID;			// +0x06
	TeamIndex::iterator m_indexIt;	// +0x08
	TeamsInfo m_team;				// +0x0C
};

class TeamsInfoRec
{
public:
	TeamsInfoRec();										// 0x0032CD6D
	TeamsInfoRec(const TeamsInfoRec &that);				// 0x0032D9A4
	~TeamsInfoRec();
	TeamsInfoRec &operator=(const TeamsInfoRec &that);	// 0x0032DDB3
	void clear();
	void swap(TeamsInfoRec *other);
	void removeFromIndex(Int index);					// Unlinks the slot from its index entry's override chain
	void removeTeam(Int index);							// removeFromIndex, clear the Dict, move the slot to the free list
	void removeOverriddenTeams();						// 0x0032C2C4: removeTeam on every active slot that another overrides
	Int addTeam(const Dict *d);							// Fills a free slot (growing the vector if none) and files it
	void addToIndex(Int index);							// Files the slot under its (owner, name) key
	void renameTeam(Int index, const AsciiString &owner, const AsciiString &name);
	void moveTeamsToNewOwner(const AsciiString &oldOwner, const AsciiString &newOwner);

	TeamIndex m_index;					// +0x00
	_STL::vector<BfmeThingUBB> m_teams;	// +0x0C
	short m_numActive;					// +0x18
	short m_freeHead;					// +0x1A
};
typedef char TeamsInfoRecSize[(sizeof(TeamsInfoRec) == 0x1C) ? 1 : -1];

#endif
