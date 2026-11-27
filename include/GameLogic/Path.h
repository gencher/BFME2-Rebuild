// Path.h: Path, a pathfinder result (unit Pathfinder/pathfinder_path.cpp; WorldBuilder
// shows the pathfinder headers beside it, e.g. pathfinder_cell.h). Target facts:
// - SIZE 0x28: AIUpdateInterface::xfer 0x00267EDD allocates 0x28 for m_path (+0x140)
//   and runs the ctor 0x00363DC8: zero +0/+4/+8, false +0xC/+0xD, zero +0x10 and the
//   Reals +0x14..+0x20, -1 at +0x24. No vtable.
// - prependNode 0x00265596: a pooled PathNode(pos, layer) with id at +0x20, then
//   +4 = node->prependToList(+4), +8 = node when empty, +0xC = false; appendNode
//   0x002655E3 is the tail-side twin. So +4 is the head, +8 the tail and +0xC
//   Zero Hour's m_isOptimized.
// - DoXfer 0x003661FC (non-virtual, Xfer *), DoAdvanceAlongPath 0x00363930,
//   optimizeGroundPath 0x00363FA4, computeFlightDistToGoal 0x00363EC0.
// - GetNextPortalID 0x00363AA8 walks +0x10's node through the nodes' +8 links to the
//   first node whose +0x20 id is not 0x7FFFFFFF; SetLastNodePortal 0x00363B24 sets
//   the tail's id.
#pragma once
#ifndef BFME2_PATH_H
#define BFME2_PATH_H

#include <stddef.h>
#include "Common/GameType.h"
#include "Common/Coord3D.h"

class PathNode;
enum PathfindLayerEnum;

class Xfer;
class Object;

class Path
{
public:
	Path();																	// 0x00363DC8
	~Path();																	// Frees every node through FreePooledNode
	void prependNode(const Coord3D *pos, PathfindLayerEnum layer, Int id);
	void appendNode(const Coord3D *pos, PathfindLayerEnum layer, Int id);
	Bool DoAdvanceAlongPath(const Coord3D *pos, Bool flag);
	void skipNonPortalNodes();												// 0x00363A78 (provisional name): +0x10 moves along +8 links until the next node has a portal id
	Int GetNextPortalID();
	void SetLastNodePortal(Int id);
	Real computeFlightDistToGoal(const Coord3D *pos, Coord3D &goal);
	void optimizeGroundPath(Int flag);
	void rva00364551(Object *obj, Int surfaces, Bool flag, const Real *dist);
	void rva00365E98(Object *obj, const Coord3D *dir, Int surfaces, Bool flag);
	void DoXfer(Xfer *xfer);
	void markOptimized() { m_isOptimized = TRUE; }
	PathNode *getLastNode() const { return m_pathTail; }					// inline (Zero Hour's): AIMoveAwayPanicState::update 0x00347B3D reads +0x08

	UnsignedInt m_unk0x00;						// +0x00
	PathNode *m_path;							// +0x04, head
	PathNode *m_pathTail;						// +0x08
	Bool m_isOptimized;							// +0x0C
	Bool m_unk0x0D;								// +0x0D
	PathNode *m_unk0x10;						// +0x10, the node GetNextPortalID starts from
	Real m_unk0x14;								// +0x14
	Real m_unk0x18;								// +0x18
	Real m_unk0x1C;								// +0x1C
	Real m_unk0x20;								// +0x20
	Int m_unk0x24;								// +0x24, -1
};

typedef char Path_off_m_unk0x24[(offsetof(Path, m_unk0x24) == 0x24) ? 1 : -1];
typedef char PathSize[(sizeof(Path) == 0x28) ? 1 : -1];

#endif
