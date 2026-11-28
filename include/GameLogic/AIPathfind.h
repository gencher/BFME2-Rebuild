// AIPathfind.h: SAGE Pathfinder call surface (Zero Hour AIPathfind.h names).
// Target facts: the non-virtual members below are every map row of Pathfinder
// with an evidenced (N:ledger) name and no placeholder types, with the
// signature its decorated name states. Pathfinder also has virtuals (xfer
// 0x002F21F9, FindCollisionFreeQuickPath 0x002F64DD) whose slots are not yet
// placed, and its data layout is not recovered: TheAI->pathfinder() (AI +0x10)
// is the only way callers reach it. Placeholder-typed rows (rvaXXXXXXXX) are
// left out except the ones existing callers already use.
#pragma once
class Object;
class LocomotorSet;
class Locomotor;
class PathfindCell;
class Path;
class PathNode;
class PolygonTrigger;
class HierarchicalPath;
class Coord3D;
struct ICoord2D;
struct TCheckMovementInfo;
class Weapon;
class Pathfinder;
class Bridge;
class Xfer;
#include <stddef.h>
#include "Common/GameType.h"
#include "Common/AsciiString.h"
#include "Common/Coord3D.h"
#include "Common/IRegion2D.h"
#include "Common/Snapshot.h"
#include "GameLogic/PathfindLayer.h"
#include "GameLogic/PathfindZoneManager.h"
#include "GameLogic/PathfindCell.h"

#define PATHFIND_CELL_SIZE		10
#define PATHFIND_CELL_SIZE_F	10.0f
#define LAYER_Z_CLOSE_ENOUGH_F	10.0f

// Layers: getCell 0x002E6D62 indexes m_layers only for 1 < layer <= 15 and
// otherwise reads the ground grid.
enum PathfindLayerEnum
{
	LAYER_INVALID = 0,
	LAYER_GROUND = 1,
	LAYER_LAST = 15
};

// PathfindServicesInterface: Pathfinder's primary base. The table at VA
// 0x00C05408 (ctor 0x002F6E22 stores it at +0) has seven slots, in order
// 0x002FD344 FindPath, 0x002FA3F1 FindClosestPath, 0x002FAD13 FindAttackPath,
// 0x002FC9B1 FindAttackPathSideways, 0x002FB558 PatchPath, 0x002FB997
// FindSafePath, 0x002F64DD FindCollisionFreeQuickPath (the map's names for the
// slot targets; the map spells the first six non-virtual). No destructor slot:
// Pathfinder's virtual dtor comes from Snapshot. The weapon parameter type of
// the two attack paths is a role label (the map has a placeholder class).
class PathfindServicesInterface
{
public:
	virtual Path *FindPath(Object *obj, const LocomotorSet &locomotorSet, const Coord3D *from, const Coord3D *to, Bool *) = 0;
	virtual Path *FindClosestPath(Object *obj, const LocomotorSet &locomotorSet, const Coord3D *from, Coord3D *to, Bool, Real, Bool) = 0;
	virtual Path *FindAttackPath(Object *obj, const LocomotorSet &locomotorSet, const Coord3D *from, Object *victim, const Coord3D *victimPos, const Weapon *weapon, Bool, Bool *) = 0;
	virtual Path *FindAttackPathSideways(Object *obj, const LocomotorSet &locomotorSet, const Coord3D *from, Object *victim, const Coord3D *victimPos, const Weapon *weapon) = 0;
	virtual Path *PatchPath(Object *obj, const LocomotorSet &locomotorSet, Path *originalPath, Bool) = 0;
	virtual Path *FindSafePath(Object *obj, const LocomotorSet &locomotorSet, const Coord3D *from, const Coord3D *repulsorPos1, const Coord3D *repulsorPos2, Real repulsorRadius) = 0;
	virtual Path *FindCollisionFreeQuickPath(Object *obj, const Coord3D *to, PathNode *) = 0;
};

// PathfindRequestRing: a fixed ring of queued ObjectIDs. SIZE 0x808. Pathfinder
// holds two (+0x1C1E0 for path requests: queueForPath 0x002EBD31; +0x1C9E8 for
// collision responses: QueueForCollisionResponsePath 0x002EBD65); its ctor zeroes
// both rings' +0x800/+0x804. hasRoom 0x002EABF3, push 0x002EAC14 and contains
// 0x002EAC41 advance an index as `index == 511 ? 0 : index + 1`; the xfer
// 0x002EAC9F saves head, tail and the length 512.
// The ring's index step (out-of-line copy 0x002E7FC7, which the map names as a free
// int(int) function; the name stays the map's placeholder until a caller names it).
inline Int Rva002E7FC7(Int index) { return index == 512 - 1 ? 0 : index + 1; }

class PathfindRequestRing
{
public:
	enum { QUEUE_LEN = 512 };

	// Inline: retail emits out-of-line copies (0x002EABF3, 0x002EAC14,
	// 0x002EAC41) and its callers reload ecx after each call, as for a COMDAT.
	Bool hasRoom() const { return next(m_tail) != m_head; }
	void push(const ObjectID &id)
	{
		m_queue[m_tail] = id;
		m_tail = next(m_tail);
	}
	Bool contains(const ObjectID &id) const
	{
		for (UnsignedInt i = m_head; i != m_tail; i = next(i))
		{
			if (m_queue[i] == id)
				return true;
		}
		return false;
	}
	void rva002EAC9F(Xfer *xfer);					// 0x002EAC9F (map placeholder name: the ring's xfer)

	static UnsignedInt next(UnsignedInt index) { return Rva002E7FC7(index); }

	ObjectID m_queue[QUEUE_LEN];					// +0x000
	UnsignedInt m_head;						// +0x800 the oldest request (the xfer transfers both indices as unsigned ints)
	UnsignedInt m_tail;						// +0x804 where the next request goes
};
typedef char PathfindRequestRingSize[(sizeof(PathfindRequestRing) == 0x808) ? 1 : -1];

// Wall layers: 17..64 (0x002E6E8A; IsPointOnWall 0x002E940F treats any layer
// above 16 as a wall and GetWallHeight 0x002EF68C calls this).
Bool isWallLayer(Int layer);

// A cell's world position (0x002E79A8): the cell's centre (or near corner when
// centerInCell is false) at the terrain or layer height.
Coord3D cellToWorld(Bool centerInCell, Int cellX, Int cellY, PathfindLayerEnum layer);

// Line-walk callback objects. Zero Hour's iterateCellsAlongLine calls a
// CellAlongLineProc with a void* user block; BFME 2 makes the walk a member
// template over the block's type and calls the block's own
// cellCallback(from, to, x, y) (a thiscall on the block: each of the walk's
// copies differs only in the cellCallback its call names). Each struct is named
// for the Pathfinder member that walks with it (the Zero Hour struct name where
// Zero Hour has the same walk). Their fields are recovered with those callers.
struct ZoneChangeStruct				// cellCallback 0x002E7261; the walk 0x002F4AC1 (QuickDoesPathExistToStructure's): stops on the first cell whose obstacle is not m_obstacleID and records it
{
	Int cellCallback(PathfindCell *from, PathfindCell *to, Int toX, Int toY);
	Int m_unk0x00;					// +0x00
	ObjectID m_obstacleID;				// +0x04 the obstacle the walk may cross
	Int m_zone;					// +0x08 the stopping cell's +0x08 word
	Int m_cellX;					// +0x0C
	Int m_cellY;					// +0x10
};
struct ObstacleRunStruct			// cellCallback 0x002E6F92; 0x002ED535 (records each water cell as a world position, stops on any other type, noting a clear one)
{
	Int cellCallback(PathfindCell *from, PathfindCell *to, Int toX, Int toY);
	Coord3D m_pos;					// +0x00 the last water cell's corner, z 0
	Bool m_foundClear;				// +0x0C the walk stopped on a CELL_CLEAR cell
};
struct GroundCellCenterStruct			// cellCallback 0x002E7ED6; 0x002EE96B passes a 12-byte local as the block (stops on the first cell off the wall layers and stores its centre)
{
	Int cellCallback(PathfindCell *from, PathfindCell *to, Int toX, Int toY);
	Coord3D m_pos;					// +0x00
};
// IsGroundLineOnly 0x002ED219 walks with a block it never fills (retail places it
// in a whole dead parameter slot, [ebp+0xC], so it is one 4-byte word); the test is
// inline, so its walk 0x002E8251 has no call.
struct GroundLineOnlyStruct
{
	void *m_unused;
	Int cellCallback(PathfindCell *, PathfindCell *to, Int, Int) { return to->getLayer() != LAYER_GROUND; }
};
// The movement test block validMovementPosition 0x002E6DC4 reads (16 bytes;
// AdjustFlingDestination 0x002EE4DE builds one and copies it into FlingStruct,
// and LineBlockedStruct's init 0x002E931F builds one at +0x40).
struct PathfindMoveCheck
{
	UnsignedInt m_acceptableSurfaces;	// +0x00 tested against the cell type's surface bit (table 0x00DBD33C)
	Bool m_unk0x04;				// +0x04 when set, cells with bit 17 are refused
	Bool m_unk0x05;				// +0x05 when set, cells with bit 18 are refused
	Int m_unk0x08;				// +0x08 when >= 0, cells whose bits 19..20 exceed it are refused
	Bool m_unk0x0C;				// +0x0C when set, obstacle cells without bit 22 are refused
};
typedef char PathfindMoveCheckSize[(sizeof(PathfindMoveCheck) == 0x10) ? 1 : -1];

// AdjustFlingDestination's walk block (the wrapper 0x002EB4A0, the walk
// 0x002E85AD with the test inline; retail keeps the out-of-line test 0x002E7D6D
// and constructor 0x002E7D3F as well, so both are ordinary out-of-line members): stops on an impassable cell, a cell the
// movement test refuses, or a wall cell reached from the ground, and otherwise
// moves the destination to the cell. Offsets from the walk's loads.
struct FlingStruct
{
	Pathfinder *m_pathfinder;		// +0x00
	Coord3D *m_dest;			// +0x04
	Int m_layer;				// +0x08
	PathfindMoveCheck m_check;		// +0x0C
	Bool m_checkMovement;			// +0x1C
	FlingStruct(Pathfinder *pathfinder, Coord3D *dest, Int layer, const PathfindMoveCheck &check, Bool checkMovement);	// 0x002E7D3F
	Int cellCallback(PathfindCell *from, PathfindCell *to, Int toX, Int toY);	// 0x002E7D6D (out of line, and inlined into the walk 0x002E85AD)
};

struct LinePassableStruct			// cellCallback 0x002ECE6A; IsLinePassable 0x002F1AF4, IsLinePassableForOptimize 0x002F1B66
{
	Int cellCallback(PathfindCell *from, PathfindCell *to, Int toX, Int toY);
};
struct LineBlockedStruct			// cellCallback 0x002ED01E; IsLineBlocked 0x002F1B32
{
	Int cellCallback(PathfindCell *from, PathfindCell *to, Int toX, Int toY);
};
struct HordeMeleeLinePassableStruct		// cellCallback 0x002ED15A; IsHordeMeleeLinePassable 0x002F1BA2
{
	Int cellCallback(PathfindCell *from, PathfindCell *to, Int toX, Int toY);
};
struct ViewAttackBlockedStruct			// cellCallback 0x002F18D4; isAttackViewBlockedByObstacle 0x002F314A
{
	Int cellCallback(PathfindCell *from, PathfindCell *to, Int toX, Int toY);
};
struct segmentIntersectsStruct			// cellCallback 0x002F1BD5; the tall-building segment test 0x002F3392
{
	Int cellCallback(PathfindCell *from, PathfindCell *to, Int toX, Int toY);
};
struct ApproachToTargetStruct			// cellCallback 0x002F4D8B; CanApproachToTarget 0x002FA2DC
{
	Int cellCallback(PathfindCell *from, PathfindCell *to, Int toX, Int toY);
};
struct TightenPathStruct			// cellCallback 0x002F600C; TightenPath 0x002FC95E
{
	Int cellCallback(PathfindCell *from, PathfindCell *to, Int toX, Int toY);
};
struct GroundPathPassableStruct			// cellCallback 0x002E93A7; IsGroundPathPassable 0x002EF663
{
	Int cellCallback(PathfindCell *from, PathfindCell *to, Int toX, Int toY);
};
struct GroundPathPositionStruct			// cellCallback 0x002E7B29; AdjustGroundPathPosition 0x002EDFBD
{
	Int cellCallback(PathfindCell *from, PathfindCell *to, Int toX, Int toY);
};
struct MADStruct				// cellCallback 0x002F3F7D; MoveAlliesAwayFromDestination 0x002F714B
{
	Int cellCallback(PathfindCell *from, PathfindCell *to, Int toX, Int toY);
};
struct GroundCellsStruct			// cellCallback 0x002F4491; FindGroundPath 0x002F95F6
{
	Int cellCallback(PathfindCell *from, PathfindCell *to, Int toX, Int toY);
};
struct ExamineCellsStruct			// cellCallback 0x002F5925; the neighbour examination 0x002F83F5
{
	Int cellCallback(PathfindCell *from, PathfindCell *to, Int toX, Int toY);
};

// Pathfinder. SIZE 0x1D200: AI's ctor 0x002FEA85 allocates 0x1D200 before
// calling Pathfinder's ctor 0x002F6E22.
//  - bases: PathfindServicesInterface (+0, table 0x00C05408) and Snapshot
//    (+4, table 0x00C053F8: deleting dtor thunk 0x002F21F1, loadPostProcess
//    folded 0x000B3FD0, GetSnapshotName 0x002F21EB "Pathfinder", xfer 0x002F21F9).
//  - members: the ctor zeroes +0x0C/+0x10/+0x34/+0x58/+0x5C; builds 16
//    PathfindLayers at +0x60 (vector ctor, 0x40 each), the zone manager at
//    +0x460 (ctor 0x00532361), 64 strings at +0x1BFBC, a set at +0x1C1C0
//    (ctor 0x000D3A71), a vector at +0x1C1CC, zeroes +0x1C1D8/+0x1C1DC/
//    +0x1C9E0/+0x1C9E4/+0x1D1E8/+0x1D1EC, builds the object at +0x1D1F0 (ctor
//    0x001F81BF), clears 64 dwords at +0x1C0BC, then calls 0x002F462C.
//  - getCell 0x002E6D62 bounds x/y by the extent at +0x14..+0x20 and reads the
//    ground grid's column pointers at +0x10 (16-byte cells); NewMap 0x002E8DAA
//    allocates the cell block at +0x0C and points each column into it.
//  - ForceMapRecalculation 0x002E7133 runs 0x0052F2EC only when +0x10 is set;
//    SetDebugPathPosition 0x002E74B5 copies a Coord3D to +0x4C; IsPointOnRamp
//    0x002E757D walks a Bridge list from +0x5C.
class Pathfinder : public PathfindServicesInterface, public Snapshot
{
public:
 void ProcessPathfindQueue(); // native 0x002F0F07, called by AI::update
 void reset(); // 0x002F462C (Zero Hour name; AI::reset calls it first)
 int GetGroundLayer(const Coord3D *);
 bool rva002EDFF9(Object *,Coord3D *);
	Int moveToFirstGroundCellOffWall(Coord3D *position, const Coord3D *towards);	// 0x002EE96B: walks the ground cells from *position towards *towards (0x002EB675, GroundCellCenterStruct) and on success stores the first cell centre off the wall layers into position x and y; map row ?rva002EE96B@Pathfinder@@QAEHPAVCoord3D@@PBU2@@Z (an address spelling)
 bool AdjustDestination(Object *,const LocomotorSet &,Coord3D *,const Coord3D * = 0);
 // Reachability test FlammableUpdate's flee scan asks (0x0023E1D0 family).
 bool QuickDoesPathExist(Object *,const Coord3D *,const Coord3D *,int);
 // Re-settles an object's pathfind layer (PhysicsBehavior::update 0x0039114B).
 void AdjustLayer(Object *obj);
 void AdjustFlingDestination(Object *obj, const Coord3D *from, Coord3D *dest);

	void * getObstacleID(Int, Int);	// 0x001e4461 N:ledger
	PathfindCell * getCell(PathfindLayerEnum, Int, Int);	// 0x002e6d62 N:ledger
	void ForceMapRecalculation();	// 0x002e7133 N:ledger
	void AddObjectToPathfindMap(Object *);	// 0x002e7178 N:ledger
	void rva00530212(Object *obj, Bool insert, Bool keepGateFlags, Bool goalOnly);	// 0x00530212 (map placeholder name; the four PathfindMap wrappers call it)
	void RemoveObjectFromPathfindMap(Object *);	// 0x002e718a N:ledger
	void RemoveObjectGoalFromPathfindMap(Object *);	// 0x002e719b N:ledger
	void RemoveObjectFromPathfindMapKeepingGateFlags(Object *);	// 0x002e71ad N:ledger
	Int AddBridge(Int);	// 0x002e71bf N:ledger
	void SetBridgeStateRepaired(Int, Bool);	// 0x002e7205 N:ledger
	Bool IsValidMovementTerrain(PathfindLayerEnum, const Locomotor *, const Coord3D *);	// 0x002e7296 N:ledger
	void SetDebugPathPosition(const Coord3D *);	// 0x002e74b5 N:ledger
	void SetDebugPath(Path *path);	// replaces m_debugPath, deleting the old path
	static void snapPosition(Object *obj, Coord3D *pos);	// snaps a position to its pathfind cell
	Bool IsPointOnRamp(const Coord3D *);	// 0x002e757d N:ledger
	void clip(Coord3D *, Coord3D *);	// 0x002e75a3 N:ledger
	void AdjustMeleeOffset(Object *, Object *, Coord3D *);	// 0x002e8c6e N:ledger
	void NewMap();	// 0x002e8daa N:ledger
	Bool IsValidMovementPositionForObject(const Coord3D *, Int, Int, const Object *);	// 0x002e909f N:ledger
	Bool IsPointOnWall(Int, Bool);	// 0x002e940f N:ledger
	Bool IsWaterCell(Int, Int);	// 0x002e9897 N:ledger
	Bool IsCliffCell(Int, Int);	// 0x002e98c6 N:ledger
	Int isBlockedGateObstacleCell(Int, Int);	// 0x002e98ea N:ledger
	Int IsNonPinchedCliffCell(Int, Int);	// 0x002e9919 N:ledger
	Bool IsImpassableCell(Int, Int);	// 0x002e9948 N:ledger
	Bool IsBuildRestrictedCell(const Coord3D *, Bool, Bool, Int);	// 0x002e996e N:ledger
	void GetCellType(Int, void *, void *, void *, Int);	// 0x002e99bd N:ledger
	Bool CircleClipsTallBuilding(const Coord3D *, const Coord3D *, Real, UnsignedInt, Coord3D *);	// 0x002e9be5 N:ledger
	Bool checkForMovement(Object *, TCheckMovementInfo &);	// 0x002ea34a N:ledger
	Bool queueForPath(ObjectID);	// 0x002ebd31 N:ledger
	Bool QueueForCollisionResponsePath(ObjectID);	// 0x002ebd65 N:ledger
	Int _GetOverlapUnits(Object *, const Coord3D *, Int *);	// 0x002ec8c3 N:ledger
	Int GetOverlapGoalUnits(Object *, const Coord3D *, Int *);	// 0x002ec9e1 N:ledger
	Int CountOverlapHordeGoalUnits(Object *, const Coord3D *);	// 0x002ecaff N:ledger
	Bool IsGroundLineOnly(const Coord3D *, const Coord3D *);	// 0x002ed219 N:ledger
	Bool adjustToMeleeDestination(Object *, const LocomotorSet &, Coord3D *);	// 0x002ed7f7 N:ledger
	Bool AdjustToNearestGroundCell(Coord3D *);	// 0x002edcf7 N:ledger
	Bool AdjustToNearestValidCell(Coord3D *);	// 0x002edda9 N:ledger
	Int AdjustGroundPathPosition(const Coord3D *, Coord3D *);	// 0x002edfbd N:ledger
	void PrependCells(Path *, const Coord3D *, PathfindCell *, Bool);	// 0x002ee1c7 N:ledger
	Bool IsValidObjectMovement(Object *, const Coord3D *, const Coord3D *);	// 0x002ee64a N:ledger
	Bool getClosestPointOnLand(const Coord3D *, Object *, Coord3D *);	// 0x002ee7ee N:ledger
	Bool IsGroundPathPassable(const Coord3D *, PathfindLayerEnum, const Coord3D *, Int);	// 0x002ef663 N:ledger
	Real GetWallHeight(PathfindLayerEnum, const Coord3D *, Coord3D *);	// 0x002ef68c N:ledger
	Path * BuildGroundPath(const Coord3D *, PathfindCell *, Bool, Int);	// 0x002ef908 N:ledger
	Bool CheckDestination(Object *, Int, Int, PathfindLayerEnum, Int, Bool, Int *, Bool);	// 0x002f00e9 N:ledger
	Path * BuildActualPath(Object *, Int, const Coord3D *, PathfindCell *, Bool, Bool, Bool);	// 0x002f0596 N:ledger
	Bool IsValidHordeMemberRegularMovement(Object *, const Coord3D *, const Coord3D *, const Coord3D *);	// 0x002f06a3 N:ledger
	void UpdateLayer(Object *, PathfindLayerEnum);	// 0x002f11a1 N:ledger
	Int _GetAdjacentUnits(Object *, const Coord3D *, Int *);	// 0x002f133c N:ledger
	Int IsLinePassable(void *, void *, PathfindLayerEnum, const Coord3D *, const Coord3D *, void *, Int, Int);	// 0x002f1af4 N:ledger
	Bool IsLineBlocked(void *, void *, PathfindLayerEnum, const Coord3D *, const Coord3D *);	// 0x002f1b32 N:ledger
	Int IsLinePassableForOptimize(void *, void *, PathfindLayerEnum, const Coord3D *, const Coord3D *, void *);	// 0x002f1b66 N:ledger
	Bool IsHordeMeleeLinePassable(void *, void *, PathfindLayerEnum, const Coord3D *, const Coord3D *);	// 0x002f1ba2 N:ledger
	unsigned char bfmeWrapE6E90(void *, void *, void *, void *, void *, void *);	// 0x002f1c25 N:ledger
	Bool adjustToPossibleDestination(Object *, const LocomotorSet &, Coord3D *);	// 0x002f287a N:ledger
	void SnapClosestGoalPosition(Object *, Coord3D *);	// 0x002f2b88 N:ledger
	Bool isAttackViewBlockedByObstacle(const Object *, const Coord3D &, const Object *, const Coord3D &);	// 0x002f314a N:ledger
	Path * GetAircraftPath(const Object *, const Coord3D *);	// 0x002f3971 N:ledger
	Bool isViewBlockedByObstacle(const Object *, const Object *);	// 0x002f3b6b N:ledger
	Bool isAttackViewBlockedByObstacle(const Object *, const Coord3D *);	// 0x002f3b9c N:ledger
	Bool AdjustTargetDestination(Object *, Int, Int, Int, const Coord3D *);	// 0x002f3bb3 N:ledger
	Bool MoveAllies(Object *, Path *, Bool);	// 0x002f3c09 N:ledger
	void AddToOpenList(PathfindCell *);	// 0x002f436f N:ledger
	Bool QuickDoesPathExistToStructure(Object *, const Coord3D *, Object *, Int);	// 0x002f4b33 N:ledger
	void CheckChangeLayers(PathfindCell *, Object *);	// 0x002f4e54 N:ledger
	Bool CheckForAdjust(Object *, const LocomotorSet &, Bool, Int, Int, PathfindLayerEnum, Int, Bool, Coord3D *, const Coord3D *, Real, Int *, Int);	// 0x002f52a7 N:ledger
	Int CheckPathCost(Object *, const LocomotorSet &, const Coord3D *, const Coord3D *);	// 0x002f5c7a N:ledger
	Path * GetHordeUnitPath(Object *, const Coord3D *);	// 0x002f6075 N:ledger
	Int MoveAlliesAwayFromDestination(Object *, const Coord3D *, const Coord3D *);	// 0x002f714b N:ledger
	Int ExamineNeighboringCellsForSidewaysAttack(PathfindCell *, const ICoord2D *, Int, const LocomotorSet &, Bool, Bool, Int, ICoord2D *, Object *, Int);	// 0x002f8b00 N:ledger
	Path * FindGroundPath(Object *, const Coord3D *, const Coord3D *, Int);	// 0x002f95f6 N:ledger
	Path * GetMoveAwayFromPath(Object *, Object *, Path *, Object *, Path *);	// 0x002f9dba N:ledger
	Bool AdjustHordeMeleeDestination(Object *, void *, Coord3D *);	// 0x002fa202 N:ledger
	Path * FindClosestPath(Object *, const LocomotorSet &, const Coord3D *, Coord3D *, Bool, Real, Bool);	// 0x002fa3f1 N:ledger
	Path * PatchPath(Object *, const LocomotorSet &, Path *, Bool);	// 0x002fb558 N:ledger
	Path * FindSafePath(Object *, const LocomotorSet &, const Coord3D *, const Coord3D *, const Coord3D *, Real);	// 0x002fb997 N:ledger
	Path * InternalFindPath(Object *, const LocomotorSet &, const Coord3D *, const Coord3D *, HierarchicalPath *, Bool *);	// 0x002fbbeb N:ledger
	void TightenPath(Object *, const LocomotorSet &, Coord3D *, const Coord3D *);	// 0x002fc95e N:ledger
	Path * FindPath(Object *, const LocomotorSet &, const Coord3D *, const Coord3D *, Bool *);	// 0x002fd344 N:ledger
	static void classifyMapCell(Int, Int, PathfindCell *);	// 0x0052e11a N:ledger
	void ClassifyWall(Object *, Bool);	// 0x0052f63f N:ledger

protected:
	Path * internal_findHierarchicalPath(Bool, Int, Object *, const Coord3D *, const Coord3D *, Bool, Bool, Coord3D *);	// 0x002f71a6 N:ledger
	Path * FindHierarchicalPath(Bool, const LocomotorSet &, Object *, const Coord3D *, const Coord3D *, Bool);	// 0x002f83a3 N:ledger
	Path * FindClosestHierarchicalPath(Bool, const LocomotorSet &, Object *, const Coord3D *, const Coord3D *, Bool, Coord3D *);	// 0x002f83ce N:ledger

public:
	Pathfinder();						// 0x002F6E22
	virtual ~Pathfinder();					// 0x002F213F
	virtual Path *FindAttackPath(Object *obj, const LocomotorSet &locomotorSet, const Coord3D *from, Object *victim, const Coord3D *victimPos, const Weapon *weapon, Bool, Bool *);	// slot 2, 0x002FAD13
	virtual Path *FindAttackPathSideways(Object *obj, const LocomotorSet &locomotorSet, const Coord3D *from, Object *victim, const Coord3D *victimPos, const Weapon *weapon);	// slot 3, 0x002FC9B1
	virtual Path *FindCollisionFreeQuickPath(Object *obj, const Coord3D *to, PathNode *);	// slot 6, 0x002F64DD
	const char *name() const;							// 0x002F21EB (the map files the slot-2 body under this name)
	PathfindCell *getCell(PathfindLayerEnum layer, const Coord3D *pos);	// 0x001E3647 (the map types it with a placeholder return)
	void Rva0052F2EC();					// 0x0052F2EC (map placeholder name; ForceMapRecalculation's work)

	// World position to grid cell (0x002E7875, cdecl: every line walk's
	// Coord3D form calls it with centerInCell true and passes the result on).
	static ICoord2D *worldToCell(ICoord2D *cell, Bool centerInCell, const Coord3D *pos);
	Bool validMovementPosition(const PathfindMoveCheck *check, PathfindCell *cell);	// 0x002E6DC4 (Zero Hour's role; the map files it under a placeholder class, but every caller passes the Pathfinder in ecx)

	// Line walks (Zero Hour names; see the callback structs above). The
	// Coord3D form converts both ends with worldToCell (0x002F0C78 and its
	// copies, 63 bytes each); the cell form is the Bresenham walk (0x002EEF16
	// and its copies, 256 bytes each), calling getCell for each cell and
	// ending on the first nonzero cellCallback result.
	template <class Callback>
	Int iterateCellsAlongLine(const Coord3D *startWorld, const Coord3D *endWorld, PathfindLayerEnum layer, Callback *callback);
	template <class Callback>
	Int iterateCellsAlongLine(const ICoord2D *start, const ICoord2D *end, PathfindLayerEnum layer, Callback *callback);

protected:
	virtual void loadPostProcess() {}			// Snapshot slot 1 (folded 0x000B3FD0)
	virtual const char *GetSnapshotName() const;		// Snapshot slot 2, 0x002F21EB
	virtual void xfer(Xfer *xfer);				// Snapshot slot 3, 0x002F21F9

public:
	Bool m_isMapReady;					// +0x08 (donor name; methods return early on false)
	PathfindCell *m_blockOfMapCells;			// +0x0C (donor name; NewMap's cell block)
	PathfindCell **m_map;					// +0x10 (donor name; column pointers into the block)
	IRegion2D m_extent;					// +0x14 cell bounds, inclusive (getCell)
	UnsignedByte m_unk0x24[0x10];				// +0x24
	void *m_unk0x34;					// +0x34 zeroed by the ctor
	UnsignedByte m_unk0x38[0x10];				// +0x38
	ObjectID m_ignoreObstacleID;				// +0x48 (donor name; IsValidObjectMovement 0x002EE64A saves and restores it)
	Coord3D m_debugPathPosition;				// +0x4C (donor name; SetDebugPathPosition)
	Path *m_debugPath;					// +0x58 (donor name; SetDebugPath 0x002EDEAB replaces it)
	Bridge *m_bridgeList;					// +0x5C (provisional; IsPointOnRamp walks it)
	PathfindLayer m_layers[LAYER_LAST + 1];			// +0x60
	PathfindZoneManager m_zoneManager;			// +0x460
	UnsignedByte m_unk0x1BEA4[0x1BFBC - (0x460 + sizeof(PathfindZoneManager))];	// zone manager tail and gap
	AsciiString m_unk0x1BFBC[64];				// +0x1BFBC
	Int m_unk0x1C0BC[64];					// +0x1C0BC cleared by the ctor
	UnsignedInt m_unk0x1C1BC;				// +0x1C1BC
	UnsignedByte m_unk0x1C1C0[0xC];				// +0x1C1C0 _STL::set<AsciiString> (ctor 0x000D3A71)
	UnsignedByte m_unk0x1C1CC[0xC];				// +0x1C1CC an STLport vector (ctor 0x00211E58)
	Int m_unk0x1C1D8;					// +0x1C1D8
	Int m_unk0x1C1DC;					// +0x1C1DC
	PathfindRequestRing m_pathfindRequests;			// +0x1C1E0 (queueForPath)
	PathfindRequestRing m_collisionResponseRequests;	// +0x1C9E8 (QueueForCollisionResponsePath)
	UnsignedByte m_unk0x1D1F0[0x10];			// +0x1D1F0 an object built by ctor 0x001F81BF; the dtor frees an allocation it holds
};
// SIZE 0x1D200
typedef char PathfinderSize[(sizeof(Pathfinder) == 0x1D200) ? 1 : -1];



typedef char PathfinderLayersOffset[(offsetof(Pathfinder, m_layers) == 0x60) ? 1 : -1];
typedef char PathfinderZoneManagerOffset[(offsetof(Pathfinder, m_zoneManager) == 0x460) ? 1 : -1];
typedef char PathfinderStringsOffset[(offsetof(Pathfinder, m_unk0x1BFBC) == 0x1BFBC) ? 1 : -1];
typedef char PathfinderRequestsOffset[(offsetof(Pathfinder, m_pathfindRequests) == 0x1C1E0) ? 1 : -1];
