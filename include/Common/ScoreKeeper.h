// ScoreKeeper.h: a player's score tallies (Player embeds one at +0x3BC). SIZE 0x338.
// Target facts (code RVAs, tables VAs):
//  - Snapshot base: table 0x00C1AD98 is the deleting dtor 0x0039C5E1, then
//    loadPostProcess 0x0039B78D, GetSnapshotName 0x0039C3A6 and xfer 0x0039C7B8
//    (the map spells them LoadPostProcess and DoXfer). The ctor 0x0039CAE1
//    stores it at +0 and ends by calling reset(0), which fills the scalars.
//  - Size 0x338: the span Player gives it (+0x3BC..+0x6F4).
//  - +0x04 money earned: addMoneyEarned 0x003B0CA9 adds to it. StatsCollector::
//    collectScoreKeeperStats 0x00437634 copies +0x08, +0x04, the units total,
//    +0x70, +0x74, the buildings total, +0xC8 and +0xCC into its money
//    spent/earned and units/buildings destroyed/built/lost fields, in that order.
//  - Twenty-int arrays: +0x20 (getTotalUnitsDestroyed 0x0039B769 sums it),
//    +0x78 (getTotalBuildingsDestroyed 0x0039B73F), +0x118 (0x0039B749) and
//    +0x170 (0x0039B773). +0x20 units destroyed, two words, then +0x78
//    buildings destroyed is Zero Hour's order, which supplies the names.
//  - Maps (sizeof 0xC each, the STLport tree in this build): +0x1C8, +0x1D4,
//    +0x1E4, +0x1F0, twenty at +0x1FC, +0x2EC (objects lost, operator[] at
//    0x002077D6 is map<unsigned, int>::operator[]) and +0x2F8. The find helper
//    0x00357180 and clear 0x00357416 are folded names of the void* tree.
//  - Vectors: +0x304 (vector<unsigned>, copied by unhookAll 0x0039C09C),
//    +0x31C (the per-frame stats vector, element class not yet named) and
//    +0x328. A three-word record at +0x310 (0, 0, 3) set by the ctor.
//  - +0x110 is the enabled flag: addObjectsDestroyed 0x0039CF1D returns when it
//    is clear, and LifetimeUpdate reaches it through setEnabled.
#pragma once
#ifndef BFME2_SCOREKEEPER_H
#define BFME2_SCOREKEEPER_H

#include <stddef.h>
#include <map>
#include <vector>
#include "Common/GameType.h"
#include "Common/Snapshot.h"
#include "Common/UnicodeString.h"
#include "Common/AsciiString.h"
#include "Common/BitFlags.h"

class Object;
class ThingTemplate;
class ModuleData;
class Player;
class Xfer;

// Count maps keyed by an unsigned id, valued by an int (retail's operator[] is
// map<unsigned, int>::operator[] at 0x002077D6).
typedef _STL::map<unsigned int, Int> ScoreCountMap;

class ScoreKeeper : public Snapshot
{
public:
	// The 0x14-byte per-frame record at +0x31C's vector: vtable 0x00C1AD6C (the Snapshot
	// base's four slots; slot 2 returns the literal at 0x00C1AD7C, slot 3 is xfer
	// 0x0039B823, which the map names ScoreKeeper::PerFrameStats::xfer). Default constructor
	// 0x0039B7FB zeroes the fields; the copy constructor 0x0039B893 copies them without
	// calling a base. updateFrame 0x0039D1F1 fills one per frame.
	class PerFrameStats : public Snapshot
	{
	public:
		PerFrameStats();			// 0x0039B7FB
		virtual void loadPostProcess() {}
		virtual const char *GetSnapshotName() const { return "ScoreKeeper::PerFrameStats"; }
	protected:
		virtual void xfer(Xfer *xfer);			// 0x0039B823
	public:

		Int m_money;				// +0x04 xfer slot 0x7C (Int); updateFrame stores the player's money
		float m_score;				// +0x08 xfer slot 0x70 (Real, version 2 and up)
		short m_unitsBuilt;			// +0x0C updateFrame stores the keeper's +0x108
		short m_buildingsBuilt;			// +0x0E updateFrame stores the keeper's +0x10C
		unsigned short m_unk0x10;		// +0x10 xfer slot 0x80 (version 3 and up)
	};

	ScoreKeeper();				// 0x0039CAE1
	virtual ~ScoreKeeper();

	virtual void loadPostProcess();			// slot 1, 0x0039B78D
	virtual const char *GetSnapshotName() const;	// slot 2, 0x0039C3A6
	virtual void xfer(Xfer *xfer);			// slot 3, 0x0039C7B8

private:
	int bfmeSumBuildingsDestroyed20(int const *);	// 0x0039B72B
	int bfmeSumUnitsDestroyed20(int const *);	// 0x0039B756
public:
	int getTotalBuildingsDestroyed();	// 0x0039B73F
	int rva0039B749();	// 0x0039B749
	int getTotalUnitsDestroyed();	// 0x0039B769
	int rva0039B773();	// 0x0039B773
	// 0x0039BF67: the kills of the kind-of mask pair, by value (ScriptConditions::
	// evaluatePlayerHasKilledKindOfUnits 0x003E5C04 passes a one-bit mustBeSet and an empty
	// mustBeClear on Player +0x3BC and compares the count with its parameter).
	int rva0039BF67(KindOfMaskType mustBeSet, KindOfMaskType mustBeClear);
	void unhookAllScoredKillTrackers();	// 0x0039C09C
	void reset(int);	// 0x0039C5FD
	void addObjectBuilt(Object *, int);	// 0x0039CBCE
	void addObjectCaptured(const Object *obj);	// Zero Hour's; Object::onCapture 0x002966A0 calls the shared `ret 4` body 0x0047A69C (BFME 2 keeps no capture count)
	void setEnabled(Bool enabled);		// 0x0039B780 (the ledger names the folded body W3DBridge::setEnabled; stores the flag at +0x110; LifetimeUpdate::update 0x003A4BFD)
	void addObjectDestroyed(Object const *);	// 0x0039CDF0
	void addObjectsDestroyed(unsigned int, Player *, unsigned int);	// 0x0039CF1D
	void addObjectLost(Object const *);	// 0x0039CF73
	void addObjectsLost(unsigned int, int);	// 0x0039D0FB
	void updateFrame(int);	// 0x0039D1F1
	// Members whose map names carry an address-named class (the map spells the class Rva0039B668, Rva0039B683, Rva0039B709, ...);
	// each reads and writes only ScoreKeeper members, and all but the getters stop when scoring is off (TheGameLogic +0x98).
	void rva0039B668(Int amount);	// 0x0039B668 +0xD8 += amount
	void rva0039B683(Real amount);	// 0x0039B683 +0xF8 += amount
	void rva0039B6A8(Real amount);	// 0x0039B6A8 +0xFC += amount
	void rva0039B6CD(Int a, Int b, Int c);	// 0x0039B6CD stores +0xE0, +0xE4, +0xE8
	UnsignedInt rva0039B6EE();	// 0x0039B6EE seconds: (+0xF0, else the current frame) / LogicFramesPerSecond
	UnsignedInt rva0039B709();	// 0x0039B709 seconds: +0xF4 / LogicFramesPerSecond
	UnsignedInt rva0039B718();	// 0x0039B718 +0xF0, else the current frame
	void rva0039B795(Int amount);	// 0x0039B795 +0x08 += amount
	void rva0039B7AD(Int amount);	// 0x0039B7AD +0x04 and +0x114 += amount
	void rva0039B7CB(Int amount);	// 0x0039B7CB +0x0C += amount
	void rva0039B7E3(Int amount);	// 0x0039B7E3 +0x10 += amount
	Int rva0039B9D1();	// 0x0039B9D1 0x0039B6EE / 60
	Int getTimeTakenScore();	// 0x0039B9DD the time bonus from the GlobalData multipliers (map: ?getTimeTakenScore@Rva0039B709@@QAEHXZ)
	Int rva0039BFFD(const ThingTemplate *thingTemplate);	// 0x0039BFFD how many keys of the +0x1F0 map are equivalent to the template
	Int rva0039C010(const ThingTemplate *thingTemplate);	// 0x0039C010 the same over the +0x1C8 map
	Int rva0039C0F4(const AsciiString &templateName);	// 0x0039C0F4 the destroyed counts, over the twenty maps, of the template with this name
	// void add(const ModuleData *tracker);	// 0x0039C7A5 (not written yet) pushes a ScoredKillTracker handle onto the +0x304 vector (map: ?add@Rva0039C7A5Holder@@QAEXPBVModuleData@@@Z)
	void addMoneyEarned(int);	// 0x003B0CA9
	void rva0039BAD2(const ThingTemplate *thingTemplate, Int amount);	// adds amount to one of three spending totals chosen by the template's KindOf, and to +0x1E0

	Int getTotalMoneyEarned() const { return m_totalMoneyEarned; }
	Int getTotalMoneySpent() const { return m_totalMoneySpent; }
	Int getTotalUnitsBuilt() const { return m_totalUnitsBuilt; }
	Int getTotalUnitsLost() const { return m_totalUnitsLost; }
	Int getTotalBuildingsBuilt() const { return m_totalBuildingsBuilt; }
	Int getTotalBuildingsLost() const { return m_totalBuildingsLost; }

	Int m_totalMoneyEarned;			// +0x04
	Int m_totalMoneySpent;			// +0x08
	Int m_unk0x0C;				// +0x0C set by reset 0x0039C5FD
	Int m_unk0x10;				// +0x10
	Int m_unk0x14;				// +0x14
	Int m_unk0x18;				// +0x18
	Int m_unk0x1C;				// +0x1C
	Int m_totalUnitsDestroyed[20];		// +0x20
	Int m_totalUnitsBuilt;			// +0x70
	Int m_totalUnitsLost;			// +0x74
	Int m_totalBuildingsDestroyed[20];	// +0x78
	Int m_totalBuildingsBuilt;		// +0xC8
	Int m_totalBuildingsLost;		// +0xCC
	Int m_unk0xD0;				// +0xD0
	Int m_unk0xD4;				// +0xD4
	Int m_unk0xD8;				// +0xD8
	Int m_unk0xDC;				// +0xDC
	Int m_unk0xE0;				// +0xE0
	Int m_unk0xE4;				// +0xE4
	Int m_unk0xE8;				// +0xE8
	Int m_unk0xEC;				// +0xEC
	UnsignedInt m_unk0xF0;			// +0xF0
	UnsignedInt m_unk0xF4;			// +0xF4
	float m_unk0xF8;			// +0xF8 set by reset
	float m_unk0xFC;			// +0xFC set by reset
	Int m_myPlayerIndex;			// +0x100 the reset argument
	Int m_unk0x104;				// +0x104 -1 after reset
	Int m_unk0x108;				// +0x108
	Int m_unk0x10C;				// +0x10C
	Bool m_enabled;				// +0x110 addObjectsDestroyed tests it
	unsigned char m_unk0x111[3];		// +0x111 padding
	Int m_unk0x114;				// +0x114
	Int m_unk0x118[20];			// +0x118 summed by 0x0039B749
	Int m_unk0x168;				// +0x168
	Int m_unk0x16C;				// +0x16C
	Int m_unk0x170[20];			// +0x170 summed by 0x0039B773
	Int m_unk0x1C0;				// +0x1C0
	Int m_unk0x1C4;				// +0x1C4 added to by addObjectsLost
	ScoreCountMap m_unk0x1C8;		// +0x1C8
	ScoreCountMap m_unk0x1D4;		// +0x1D4
	Int m_unk0x1E0;				// +0x1E0
	ScoreCountMap m_unk0x1E4;		// +0x1E4
	ScoreCountMap m_objectsBuilt;		// +0x1F0
	ScoreCountMap m_objectsDestroyed[20];	// +0x1FC
	ScoreCountMap m_objectsLost;		// +0x2EC
	ScoreCountMap m_objectsCaptured;	// +0x2F8
	_STL::vector<unsigned int> m_trackedKills;	// +0x304 ScoredKillTracker handles
	// +0x310..+0x31C: a text and two words. The unwind releases the text through the folded
	// ~UnicodeString body 0x005B804E, so the dtor sees a plain UnicodeString member here; the ctor
	// stores 0, 0 and 3 through one lea and 0x0039BD7D copies the three through 0x002B4DC1, which
	// suggests a small record type that is not yet named.
	UnicodeString m_unk0x310;		// +0x310 the dtor releases it (releaseBuffer<G> 0x00036E70)
	Int m_unk0x314;				// +0x314 0 after the ctor
	Int m_unk0x318;				// +0x318 3 after the ctor
	_STL::vector<PerFrameStats> m_frameStats;	// +0x31C the per-frame records (stride 0x14); reset clears it
	_STL::vector<Int> m_unk0x328;		// +0x328 addObjectBuilt pushes the frame-stats count; the unwind destroys it with the vector<int> body 0x0007FAB3
	unsigned char m_unk0x334[0x4];		// +0x334 padding to SIZE
};
// SIZE 0x338
typedef char ScoreKeeperSize[(sizeof(ScoreKeeper) == 0x338) ? 1 : -1];
typedef char ScoreKeeperUnitsOffset[(offsetof(ScoreKeeper, m_totalUnitsDestroyed) == 0x20) ? 1 : -1];
typedef char ScoreKeeperBuildingsOffset[(offsetof(ScoreKeeper, m_totalBuildingsDestroyed) == 0x78) ? 1 : -1];

#endif
