// TerrainLogic.h: the logic-side terrain subsystem (TheTerrainLogic, VA 0x00DFEC50). SIZE 0x191C.
// Target facts (code RVAs, tables VAs):
//  - Bases, from the ctor 0x00283FAA: Snapshot +0x00, SubsystemInterface +0x04
//    (SubsystemInterface() on this+4), AreaSetObserver<StandingWaterArea> +0x10
//    and AreaSetObserver<RiverArea> +0x14 (both get the observers' shared table
//    0x00BC6F34, then TerrainLogic's 0x00BC5848 / 0x00BC5838); primary table
//    0x00BFB2C8 (51 slots), SubsystemInterface table 0x00BFB28C.
//  - Which observer is which: the ctor appends this+0x10 to the set at +0x54 and
//    this+0x14 to the set at +0x58. In WorldBuilder the override for the first
//    calls AreaSet<StandingWaterArea>::getID and the one for the second
//    AreaSet<RiverArea>::getID; both insert into AreaSet<WaterArea> (+0x50 in
//    the game), and the river one offsets its ids by 0x3FFFFFFF in both builds.
//  - Size 0x191C: W3DTerrainLogic (allocated 0x1924 by
//    W3DGameLogic::createTerrainLogic 0x00041FCF) adds +0x191C and +0x1920.
//  - Primary slots 4..50 are declared, in order (21..50 from the table at
//    0x00BFB2C8 and the map's names for its targets). Names: TerrainLogic's map rows;
//    W3DTerrainLogic's overrides for the __purecall slots (table 0x00BC5890),
//    some of them still the map's address names; Zero Hour's TerrainLogic for
//    17..19, whose W3D signatures match getSourceFilename, alignOnTerrain and
//    isUnderwater. Slots 21..50 are not declared yet: 21..24 are unnamed cell
//    queries; WorldBuilder names slot 30 getWaterHeight, 32
//    changeWaterHeightOverTime and 37 getFirstWaypointOnPath (the map's
//    getWaypointByPath), and slot 33 returns Waypoint::getFirst().
#pragma once
#ifndef BFME2_TERRAINLOGIC_H
#define BFME2_TERRAINLOGIC_H

#include <stddef.h>
#include "Common/GameType.h"
#include "Common/AsciiString.h"
#include "Common/Coord3D.h"
#include "Common/Snapshot.h"
#include "Common/SubsystemInterface.h"
#include "GameLogic/Map/AreaSet.h"

class ChunkInputStream;
class DataChunkInput;
struct DataChunkInfo;
class Bridge;
class BridgeInfo;
class Dict;
class PolygonTrigger;
class Vector3;
class Waypoint;
class WaterHandle;
class Matrix3D;
class Object;
class Xfer;
class RiverArea;
class StandingWaterArea;
class StandingWaveArea;
class WaterArea;
class Coord3D;
struct Region3D;
enum PathfindLayerEnum;

// The two points a unit attacks a bridge from (Zero Hour's TBridgeAttackInfo).
struct TBridgeAttackInfo
{
	Coord3D attackPoint1;
	Coord3D attackPoint2;
};

class TerrainLogic : public Snapshot, public SubsystemInterface,
	public AreaSetObserver<StandingWaterArea>, public AreaSetObserver<RiverArea>
{
public:
	TerrainLogic();												// 0x00283FAA
	virtual ~TerrainLogic();									// 0x0028418A

	virtual Bool loadMap(const AsciiString &filename, ChunkInputStream *stream, Bool a, Bool b);	// slot 4, 0x00282BBB
	virtual void newMap(Bool saveGame);							// slot 5, 0x00284290
	virtual Real getGroundHeight(Real x, Real y, Coord3D *normal = 0) const;	// slot 6, 0x0027CE9A
	virtual Real getLayerHeight(Real x, Real y, PathfindLayerEnum layer, Coord3D *normal = 0, Bool clip = true) const;	// slot 7, 0x0027CEBC
	virtual void activeBoundaryExtent(Region3D *extent) const = 0;		// slot 8
	virtual void largestBoundaryExtent(Region3D *extent) const = 0;		// slot 9
	virtual void getExtentIncludingBorder(Region3D *extent) const = 0;	// slot 10
	virtual Real rva00062E4F() = 0;								// slot 11 (W3D 0x00062E4F)
	virtual void getMaximumPathfindExtent(Region3D *extent) const = 0;	// slot 12
	virtual Coord3D findClosestEdgePoint(const Coord3D *closestTo) const;	// slot 13, 0x0027D5E0 (Zero Hour name; OCLUpdate::update 0x0049B483 passes a result pointer and copies the Coord3D it returns)
	virtual Coord3D findFarthestEdgePoint(const Coord3D *farthestFrom) const;	// slot 14, 0x0027D6DF (Zero Hour name; same shape)
	virtual Bool isClearLineOfSight(const Coord3D &pos, const Coord3D &posOther) const = 0;	// slot 15
	virtual Real getHighestHeight(const Coord3D *start, const Coord3D *end) const = 0;	// slot 16, flight-path control points
	virtual AsciiString getSourceFilename();					// slot 17, 0x00062E72 (folded)
	virtual PathfindLayerEnum alignOnTerrain(Real angle, const Coord3D &pos, Bool stickToGround, Matrix3D &mtx);	// slot 18, 0x002811DD
	virtual Bool isUnderwater(Real x, Real y, Real *waterZ, Real *terrainZ, Bool *flag);	// slot 19, 0x0027D77D
	virtual Bool isCliffCell(Real x, Real y) const { return false; }	// slot 20, the shared `xor al,al; ret 8`
	// Slots 21..24: per-cell queries TerrainLogic answers with the same fold 0x0050B5F1
	// and W3DTerrainLogic overrides through the height map (0x00062DB3, 0x00062DD2,
	// 0x00062DF1, 0x00062E10); WorldBuilder does not name them, so they keep the
	// overrides' map spellings.
	virtual UnsignedByte rva00062DB3(Real x, Real y) const { return 0; }	// slot 21
	virtual UnsignedByte rva00062DD2(Real x, Real y) const { return 0; }	// slot 22
	virtual Bool rva00062DF1(Real x, Real y) const { return false; }	// slot 23
	virtual UnsignedByte rva00062E10(Real x, Real y) const { return 0; }	// slot 24
	virtual Real getWaterDepth(Real x, Real y);					// slot 25, 0x0027D815: isUnderwater's water height less the terrain height, else 0
	// Slots 26..32: Zero Hour's water-handle group. BFME 2 has two copies of the
	// first two (26/27 share 0x00062ACA, 28/29 share 0x00062AEC); the second names
	// are not known.
	virtual const WaterHandle *getWaterHandle(Real x, Real y, Real z);		// slot 26, 0x00062ACA (calls 0x0028170E)
	virtual const WaterHandle *getWaterHandleOther(Real x, Real y, Real z);	// slot 27, folded with 26
	virtual const WaterHandle *getWaterHandleByName(const AsciiString &name);	// slot 28, 0x00062AEC (jumps to 0x00282021)
	virtual const WaterHandle *getWaterHandleByNameOther(const AsciiString &name);	// slot 29, folded with 28
	virtual Real getWaterHeight(const WaterHandle *water);			// slot 30, 0x0027D85B (WorldBuilder name)
	virtual void setWaterHeight(const WaterHandle *water, Real height, Real damageAmount, Bool forcePathfindUpdate);	// slot 31, 0x00282E20
	virtual void changeWaterHeightOverTime(const WaterHandle *water, Real finalHeight, Real transitionTimeInSeconds, Real damageAmount);	// slot 32, 0x0027D88E (WorldBuilder name)
	virtual Waypoint *getFirstWaypoint();						// slot 33, 0x00062AC4 (returns the waypoint list head, the global after TheTerrainLogic)
	virtual Waypoint *getWaypointByName(const AsciiString &name);	// slot 34, 0x00281D4C
	virtual Waypoint *getWaypointByID(UnsignedInt id);			// slot 35, 0x0028333F
	virtual Waypoint *getClosestWaypointOnPath(const Coord3D *pos, const AsciiString &label);	// slot 36, 0x002812BF
	virtual Waypoint *getWaypointByPath(const AsciiString &label);	// slot 37, 0x002813EA (WorldBuilder: getFirstWaypointOnPath)
	virtual Bool isPurposeOfPath(Waypoint *way, const AsciiString &label);	// slot 38, 0x00281D92
	virtual PolygonTrigger *getTriggerAreaByName(const AsciiString &name);	// slot 39, 0x00281E47
	virtual Bridge *getFirstBridge() const { return m_bridgeListHead; }	// slot 40, the shared `mov eax,[ecx+0x40]; ret` 0x005C4AF5
	virtual Bridge *findBridgeAt(const Coord3D *loc) const;		// slot 41, 0x0027F30A
	virtual Bridge *findBridgeLayerAt(const Coord3D *loc, PathfindLayerEnum layer, Bool clip = false) const;	// slot 42, 0x0027F337
	virtual Bool objectInteractsWithBridgeLayer(Object *obj, Int layer, Bool considerBridgeHealth = true) const;	// slot 43, 0x002804B2
	virtual Bool objectInteractsWithBridgeEnd(Object *obj, Int layer) const;	// slot 44, 0x002805C1
	virtual Bool pickBridge(const Vector3 &from, const Vector3 &to, Vector3 *pos);	// slot 45, 0x00281ECA
	virtual void addBridgeToLogic(BridgeInfo *info, Dict *props, const AsciiString &bridgeTemplateName);	// slot 46, 0x00280294
	virtual void addLandmarkBridgeToLogic(Object *bridgeObj);		// slot 47, 0x0028125C
	virtual void deleteBridge(Bridge *bridge);					// slot 48, 0x00281F7F
	virtual void updateBridgeDamageStates();					// slot 49, 0x00281EA5
	virtual Bool rva00063082(const Coord3D *pos) const = 0;		// slot 50, __purecall (W3DTerrainLogic 0x00063082, which asks slot 11)

	// SubsystemInterface (table 0x00BFB28C)
	virtual void init() {}										// slot 1, the shared empty body
	virtual void reset();										// slot 9, 0x00283567
	virtual void update();										// slot 10, 0x0027CD63

	// AreaSetObserver (tables 0x00BC5848 / 0x00BC5838); WorldBuilder names both.
	virtual void onAreaAddedToSet(Int id, StandingWaterArea *area);		// 0x00283C0F
	virtual void onRemovingAreaFromSet(Int id, StandingWaterArea *area);	// 0x002833A3
	virtual void onAreaAddedToSet(Int id, RiverArea *area);				// 0x00283C40
	virtual void onRemovingAreaFromSet(Int id, RiverArea *area);			// 0x002833C5

protected:
	virtual void loadPostProcess();								// Snapshot slot 1, 0x002820A0
	virtual const char *GetSnapshotName() const;				// Snapshot slot 2, 0x0028417C
	virtual void xfer(Xfer *xfer);								// Snapshot slot 3, 0x00283845

	static Bool parseWaypointDataChunk(DataChunkInput &file, DataChunkInfo *info, void *userData);	// 0x002811CA (Zero Hour's name; tail of the registered chunk parser)
	void buildBeacons();	// 0x00283E9C (newMap calls it)
	Bool parseWaypointData(DataChunkInput &file, DataChunkInfo *info, void *userData);	// 0x0027FFF3 (Zero Hour's name; the links chunk: count, then pairs of waypoint IDs)
	void addWaypointLink(Int id1, Int id2);	// 0x0027F009

public:
	int rva0027F108(void *, float, int, int);
	Object *rva00283D70(const Coord3D *position);	// 0x00283D70 (FireSpreadUpdate::update 0x0048B7FD: the object a forest-fire position belongs to)
	void rva0027F28E(const Coord3D *pos, float radius, int arg);	// 0x0027F28E (ElvenWoodSpecialPower::rva004C3B17, TaintSpecialPower::rva004C49AE)
	void rva0027D1A4(const Coord3D *pos, float radius, int arg);	// 0x0027D1A4
	PathfindLayerEnum getHighestLayerForDestination(const Coord3D *, bool = false);
	PathfindLayerEnum getLayerForDestination(Object *obj, const Coord3D *pos);
	void deleteBridges();							// 0x0027D5AD (frees the bridge list; Zero Hour's name)
	Bool isBridgeRepaired(const Object *bridge);	// 0x0027D40B
	Int getPartitionBucket(Real *pos);	// 0x0027D0D6 (the 50 by 50 grid cell of an x, y pair, clamped to the map extent)
	void getBridgeAttackPoints(const Object *bridge, TBridgeAttackInfo *attackInfo);	// 0x0027D483
	Bool isBridgeBroken(const Object *bridge);	// 0x0027D447
	void setActiveBoundary(Int boundary);	// 0x0028069E (ScriptActions::doBorderSwitch 0x003BC3A8)
	// ScriptConditions::evaluateBridgeBroken/Repaired (0x003E3D95, 0x003E3DC6)
	// test the byte at +0x44 before asking about a bridge.
	Bool anyBridgesDamageStatesChanged() const { return m_bridgeDamageStatesChanged; }

	Int m_unk0x18[3];							// +0x18 (W3DTerrainLogic's ctor clears +0x18 again)
	Int m_unk0x24[3];							// +0x24
	unsigned char m_unk0x30[0xC];				// +0x30 a vector
	Int m_unk0x3C;								// +0x3C
	Bridge *m_bridgeListHead;					// +0x40 (Zero Hour name; getFirstBridge, slot 40)
	Bool m_bridgeDamageStatesChanged;			// +0x44
	Int m_unk0x48;								// +0x48 (1 in the ctor)
	Int m_unk0x4C;								// +0x4C
	AreaSet<WaterArea> *m_waterAreas;			// +0x50 the observer overrides insert into it
	AreaSet<StandingWaterArea> *m_standingWaterAreas;	// +0x54 this+0x10 observes it
	AreaSet<RiverArea> *m_riverAreas;			// +0x58 this+0x14 observes it
	AreaSet<StandingWaveArea> *m_unk0x5C;		// +0x5C the fourth set (same ctor; the wave set by elimination)
	Bool m_unk0x60;								// +0x60
	unsigned char m_unk0x64[0x4];				// +0x64 a _List_base
	unsigned char m_unk0x68[0x500];				// +0x68 (an array the ctor fills from +0x6C)
	Int m_unk0x568;								// +0x568
	unsigned char m_unk0x56C[0xC];				// +0x56C _STL::map<unsigned char, int>
	unsigned char m_unk0x578[0xC];				// +0x578 a vector
	void *m_unk0x584;							// +0x584 (new 0x00283081(Real, Int))
	unsigned char m_unk0x588[0x1388];			// +0x588 (an array the ctor fills from here)
	Int m_unk0x1910;							// +0x1910
	Bool m_unk0x1914;							// +0x1914
	Real m_unk0x1918;							// +0x1918
};
// SIZE 0x191C
typedef char TerrainLogicSize[(sizeof(TerrainLogic) == 0x191C) ? 1 : -1];
typedef char TerrainLogicWaterOffset[(offsetof(TerrainLogic, m_waterAreas) == 0x50) ? 1 : -1];

// The tree-iteration callback state of TerrainLogic::iterateTreesNearAPoint (the map's spelling of the
// class; its method is the callback body, and the free function wraps it for a plain C callback).
class BfmeX1035;
class BfmeT1035
{
public:
	void bfmeUse1035(BfmeX1035 *tree, Int arg);		// 0x002842CD
	Int m_bfmeV;
};
void __stdcall bfmeGo1035C(BfmeX1035 *tree, Int arg);	// 0x0028447F

extern TerrainLogic *TheTerrainLogic;

#endif
