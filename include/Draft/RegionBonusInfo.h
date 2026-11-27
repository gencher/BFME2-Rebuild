// RegionBonusInfo.h: a region's bonus totals, 0x1C bytes. The inline constructor 0x0020E42C stores the table
// 0x00BE4324 and clears the six counters. Table slots: the shared scalar deleting destructor, the shared empty
// loadPostProcess, GetSnapshotName and xfer. The INI RegionBonus names six kinds after None (the table at
// 0x00C36880: Resource, Army, Legendary, Attack, Defense, Experience); which counter holds which is not recovered.
#pragma once

#include "Common/Snapshot.h"

class Xfer;

class RegionBonusInfo : public Snapshot
{
public:
	RegionBonusInfo() : m_unk0x04(0), m_unk0x08(0), m_unk0x0C(0), m_unk0x10(0), m_unk0x14(0), m_unk0x18(0) {}

protected:
	virtual void loadPostProcess() {}
	virtual const char *GetSnapshotName() const;
	virtual void xfer(Xfer *xfer);

private:
	int m_unk0x04;
	int m_unk0x08;
	int m_unk0x0C;
	int m_unk0x10;
	int m_unk0x14;
	int m_unk0x18;
};

typedef char RegionBonusInfo_size[(sizeof(RegionBonusInfo) == 0x1C) ? 1 : -1];
