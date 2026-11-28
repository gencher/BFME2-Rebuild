// Portions derived from Command & Conquer Generals and Command & Conquer Generals Zero Hour, (c) Electronic Arts Inc., GPL-3.0-or-later; see LICENSE and NOTICE.
// 0x00305933 anchors the native 9-slot Snapshot-derived table 0x008078F0.
#pragma once

#include "Common/GameType.h"
#include "Common/Snapshot.h"

class GhostObject;
class Object;

class GhostObjectManager : public Snapshot
{
public:
	GhostObjectManager();
	virtual ~GhostObjectManager();

	virtual void reset();
	virtual void setLocalPlayerIndex(Int index);
	Int getLocalPlayerIndex() const { return m_localPlayer; }
	virtual void updateOrphanedObjects(Int *playerIndexList, Int numNonLocalPlayers) = 0;
	virtual GhostObject *addGhostObject(Object *object) = 0;
	virtual GhostObject *getFirstGhostObject() const = 0;

protected:
	virtual void loadPostProcess();
	virtual const char *GetSnapshotName() const;
	virtual void xfer(Xfer *xfer);

	Int m_localPlayer;
	Bool m_lockGhostObjects;
	Bool m_saveLockGhostObjects;
};

extern GhostObjectManager *TheGhostObjectManager;

typedef char GhostObjectManagerSize[(sizeof(GhostObjectManager) == 0xC) ? 1 : -1];
