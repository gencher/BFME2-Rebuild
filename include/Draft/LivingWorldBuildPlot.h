// LivingWorldBuildPlot.h: one building plot of a War of the Ring region, 0x38 bytes (the region's xfer allocates it).
// The constructor stores the Snapshot table at +0 and the turn-observer table at +4, constructs the list at +8, then
// copies its id, region and position. The Snapshot table has four slots: the deleting destructor, the shared empty
// loadPostProcess, the snapshot name and xfer. The observer table overrides BeginTurn and keeps OnTurnEnding.
#pragma once

#include "Common/Coord2D.h"
#include "Common/GameType.h"
#include "Common/Snapshot.h"
#include "GameLogic/LivingWorldTurnObserver.h"

class AsciiString;
class LivingWorldBuilding;
class LivingWorldRegion;

class LivingWorldBuildPlot : public Snapshot, public LivingWorldTurnObserver
{
public:
	LivingWorldBuildPlot(Int id, LivingWorldRegion *region, const Coord2D &position);
	virtual ~LivingWorldBuildPlot();
	virtual void BeginTurn(Int turn, Int phase);
	void CreateIcon(const AsciiString &name);

protected:
	virtual void loadPostProcess() {}
	virtual const char *GetSnapshotName() const;
	virtual void xfer(Xfer *xfer);

private:
	friend class LivingWorldRegion;

	UnsignedByte m_unk0x08[0x10];		// +0x08: the list the constructor builds first
	Int m_id;				// +0x18: first constructor argument
	LivingWorldRegion *m_region;		// +0x1C: second constructor argument
	LivingWorldBuilding *m_building;	// +0x20: the building standing on the plot, or NULL
	Int m_unk0x24;				// +0x24: cleared by the constructor
	Coord2D m_position;			// +0x28: third constructor argument
	Int m_unk0x30;				// +0x30: cleared by the constructor
	Bool m_unk0x34;				// +0x34: a set flag hides the plot's building from the region's queries
	Bool m_unk0x35;				// +0x35: cleared by the constructor
};

typedef char LivingWorldBuildPlot_size[(sizeof(LivingWorldBuildPlot) == 0x38) ? 1 : -1];
