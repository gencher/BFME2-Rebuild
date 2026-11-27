// LivingWorldRegion.h: one region of the War of the Ring map. The layout is partly recovered; the rest is unnamed padding.
// A Snapshot-derived class of SIZE 0x1B4. The plot vector at +0x170 holds pointers to plot records (the count of those whose
// int at +0x20 is zero is rva003F1053); the region's two placement lists sit at +0x180 and +0x18C. The three strings at
// +0x120..+0x128 name the region's fortress: its portrait image and the text keys of its display name and description.
// Not recovered: the 0x10-byte listener base at +0x04 and the 0x118-byte definition at +0x14.
#pragma once
#ifndef BFME2_LIVINGWORLDREGION_H
#define BFME2_LIVINGWORLDREGION_H

#include "Common/AsciiString.h"
#include "Common/Snapshot.h"
#include "Common/Science.h"
#include "Common/Vector.h"

class Xfer;
class LivingWorldBuilding;
class Image;
class UnicodeString;

// One plot record as the region holds it. The class is LivingWorldBuildPlot in the constructor;
// only the building at +0x20 and the flag at +0x34 are recovered.
struct RegionPlotRecord
{
	unsigned char m_unk0x00[0x20];
	LivingWorldBuilding *m_building;			// +0x20: GetBuildingByIndex returns it
	unsigned char m_unk0x24[0x34 - 0x24];
	bool m_unk0x34;					// +0x34: a set flag hides the plot's building
};

// A 12-byte placement list (begin, end, end of storage). Its element is not recovered.
struct PlacementRange
{
	unsigned char m_unk0x00[0xC];
};

// The argument of 0x003F0FA3: a record whose AsciiString at +0x18 selects the list.
struct PlacementRequest
{
	unsigned char m_unk0x00[0x18];
	AsciiString m_name;					// +0x18
};

class LivingWorldRegion : public Snapshot
{
public:
	virtual ~LivingWorldRegion();				// (deleting 0x003F36EC)
	virtual void loadPostProcess();
	virtual char const *GetSnapshotName() const;
	virtual void xfer(Xfer *xfer);			// 0x003F3BFF

	Bool IsOwnedByTeam(Int teamID) const;
	const Image *GetFortressPortrait();
	UnicodeString GetFortressDisplayName();
	UnicodeString GetFortressDisplayDescription();

	LivingWorldBuilding *GetBuildingByIndex(int index) const;
	bool rva003F02E4();
	int rva003F036E();
	int rva003F05CE();					// Plots with a visible building
	void rva003F20B0();
	void rva003F2739(ScienceType science);		// Appends to +0x164
	int rva003F1053();					// Plots without a building
	void rva003F0F48(PlacementRange *list, void *request);	// (body not written)
	void rva003F0FA3(void *request);			// Picks the list by the request's name

private:
	unsigned char m_unk0x04[0x11C - 0x04];			// +0x04 base listener and definition, not recovered
	bool m_unk0x11C;					// +0x11C: a set flag makes rva003F02E4 true
	unsigned char m_unk0x11D[0x120 - 0x11D];
	AsciiString m_fortressPortrait;				// +0x120: the image name GetFortressPortrait looks up
	AsciiString m_fortressDisplayName;			// +0x124: the text key of the fortress name
	AsciiString m_fortressDisplayDescription;		// +0x128: the text key of the fortress description
public:
	int m_regionID;					// +0x12C (LivingWorldRegionManager::findPendingBattleByRegionID 0x0020E53D compares it)
	unsigned char m_unk0x130[0x13C - 0x130];		// +0x130 not recovered
	int m_ownerPlayerID;					// +0x13C (GetNumRegionsOwned 0x002E1001 compares it to the player ID)
private:
	unsigned char m_unk0x140[0x14C - 0x140];		// +0x140 not recovered
	int m_unk0x14C;					// +0x14C: rva003F20B0 clears it, xfer transfers it
	int m_unk0x150;					// +0x150
	int m_unk0x154;					// +0x154
	_STL::vector<ScienceType> m_unk0x158;			// +0x158: xfer transfers it after +0x164
	_STL::vector<ScienceType> m_unk0x164;			// +0x164
	_STL::vector<RegionPlotRecord *> m_plots;		// +0x170
	unsigned char m_unk0x17C[0x180 - 0x17C];		// +0x17C not recovered
	PlacementRange m_placementsA;				// +0x180
	PlacementRange m_placementsB;				// +0x18C
	unsigned char m_unk0x198[0x1A0 - 0x198];		// +0x198 not recovered
	bool m_unk0x1A0;					// +0x1A0: rva003F20B0 clears it, xfer transfers it
	bool m_unk0x1A1;					// +0x1A1
	unsigned char m_unk0x1A2[0x1B4 - 0x1A2];		// +0x1A2 not recovered
};

#endif
