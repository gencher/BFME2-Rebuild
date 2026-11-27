// PathfindZoneManager.h: the pathfinder's zone bookkeeping (Pathfinder +0x460; unit
// Pathfinder/pathfinder_zonemanager.cpp).
// Target facts (retail game.dat):
// - Ctor 0x00532361: a member at +0 sized by 0x5DC0, 0xFA1 dwords zeroed from
//   +0x1770C, an array of 0x14 elements of 0x38 bytes at +0x1B594 (ctor 0x00531FBE,
//   dtor 0x00531FCF), members at +0x1B9F4, two BooleanBitmapSets at +0x1BA08 and
//   +0x1BA1C, then +0x1BA30 = false, +0x1BA31 = true, zero +0x1BA34/+0x1BA38 and the
//   ICoord2D +0x1BA3C. SIZE is at least 0x1BA44 (no allocation site: it is embedded).
// - The zone blocks: +0x1BA38 is an array of column pointers, each column an array of
//   0x44-byte blocks; +0x1BA3C/+0x1BA40 are the block counts in x and y. A block covers
//   16x16 cells (IsPassable 0x00531512 and MarkDirty 0x00531431 divide by 16).
//   Block +0x34 is the passable byte IsPassable returns; +0x35 the block's dirty byte.
// - +0x1BA30: set by both MarkDirty forms and 0x00531481 (the manager is dirty).
// The members before +0x1BA30 are not recovered yet.
#pragma once
#ifndef BFME2_PATHFINDZONEMANAGER_H
#define BFME2_PATHFINDZONEMANAGER_H

#include <stddef.h>
#include "Common/GameType.h"
#include "Common/ICoord2D.h"
#include "Common/IRegion2D.h"

// One 16x16-cell zone block (0x44 bytes: IsPassable steps y by 0x44; the map's
// placeholder class Rva00531132). +0 counts the portal IDs held from +4 (at most 12:
// setPortal 0x00531132 stops adding at 12). Method names are provisional.
class ZoneBlock
{
public:
	enum { MAX_PORTALS = 12 };

	void setPortal(Bool add, Int portalID);					// 0x00531132
	Int getPortal(Int index);								// 0x0053117F

	Int m_numPortals;							// +0x00
	Int m_portals[MAX_PORTALS];					// +0x04
	UnsignedByte m_passable;					// +0x34, IsPassable's answer (an unsigned char: IsPassable and setPassable pass it through)
	Bool m_dirty;								// +0x35, set by MarkDirty
	UnsignedByte m_unk0x36[0x0E];				// +0x36
};

typedef char ZoneBlockSize[(sizeof(ZoneBlock) == 0x44) ? 1 : -1];

class PathfindZoneManager
{
public:
	enum { ZONE_BLOCK_SIZE = 16 };	// cells per block side

	UnsignedByte IsPassable(Int cellX, Int cellY);			// 0x00531512
	void MarkDirty(IRegion2D *cells);
	void MarkDirty(Int cellX, Int cellY);					// 0x00531431
	void clearPassableFlags();								// 0x005312BE (provisional name): every block's passable byte = false
	void markAllDirty();									// 0x00531481 (provisional name): the manager and every block dirty
	void markZonesDirty();									// 0x0052E112 (donor name)
	void setAllPassable();									// 0x00531300 (provisional name): every block's passable byte = true
	void setPassable(Int cellX, Int cellY, UnsignedByte passable);	// 0x005314C6 (provisional name)
	void setPortal(Int cellX, Int cellY, Bool add, Int portalID);	// 0x0053155E (provisional name): the block's setPortal
	Bool hasPortals(Int cellX, Int cellY);					// 0x005315C8 (provisional name): the block holds a portal
	Int GetPortal(Int cellX, Int cellY, Int index);			// 0x0053161A

	UnsignedByte m_unk0x00[0x1BA30];			// +0x00, not recovered
	Bool m_dirty;								// +0x1BA30
	Bool m_needToCalculateZones;				// +0x1BA31, true after construction; markZonesDirty 0x0052E112 sets it (donor name)
	UnsignedInt m_unk0x1BA34;					// +0x1BA34
	ZoneBlock **m_zoneBlocks;					// +0x1BA38, columns
	ICoord2D m_zoneBlockExtent;					// +0x1BA3C, block counts
};

typedef char PathfindZoneManager_off_m_dirty[(offsetof(PathfindZoneManager, m_dirty) == 0x1BA30) ? 1 : -1];
typedef char PathfindZoneManager_off_m_zoneBlockExtent[(offsetof(PathfindZoneManager, m_zoneBlockExtent) == 0x1BA3C) ? 1 : -1];

#endif
