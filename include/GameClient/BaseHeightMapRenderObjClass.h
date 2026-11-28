// BaseHeightMapRenderObjClass.h: the terrain render object that W3DTerrainLogic forwards its height, cliff and
// line-of-sight queries to (TheTerrainRenderObject, VA 0x00DE1EAC). Only the members W3DTerrainLogic calls are
// declared; the layout is not recovered (a generated vtable draft lives in Draft/BaseHeightMapRenderObjClass.h and
// is not included anywhere). Target facts: each W3DTerrainLogic wrapper loads the pointer and calls or tail-jumps to
// the member named in its comment.
#pragma once
#ifndef BFME2_BASEHEIGHTMAPRENDEROBJCLASS_H
#define BFME2_BASEHEIGHTMAPRENDEROBJCLASS_H

#include <stddef.h>
#include <vector>
#include "Common/GameType.h"
#include "Common/AsciiString.h"
#include "Common/Coord3D.h"
#include "Common/IRegion2D.h"
#include "Common/Snapshot.h"
#include "WW3D2/RenderObjClass.h"
#include "WW3D2/DX8CleanupHook.h"

class W3DTerrainLogic;
class DataChunkOutput;
class WorldHeightMap;
class W3DShroud;
class W3DTaint;
class W3DTreeBuffer;
class W3DPropBuffer;
class W3DBibBuffer;
class W3DRoadBuffer;
class W3DBridgeBuffer;
class RefRenderObjListIterator;

class BaseHeightMapRenderObjClass : public RenderObjClass, public DX8_CleanupHook, public Snapshot
{
public:
	BaseHeightMapRenderObjClass();
	virtual ~BaseHeightMapRenderObjClass();

	virtual void Notify_Added(SceneClass *scene);
	virtual bool Cast_Ray(RayCollisionTestClass &raytest);
	virtual void Get_Obj_Space_Bounding_Sphere(SphereClass &sphere) const;
	virtual void Get_Obj_Space_Bounding_Box(AABoxClass &box) const;

	virtual void ReleaseResources(void);
	virtual void ReAcquireResources(void);

	virtual void vslot129();
	virtual void vslot130();
	virtual Int initHeightData(Int x, Int y, WorldHeightMap *pMap, RefRenderObjListIterator *pLightsIterator);
	virtual void vslot132() = 0;
	virtual Int vslot133();
	virtual void vslot134(const void *bounds, Int mode);
	virtual void adjustTerrainLOD(Int adj);
	virtual void doPartialUpdate(const IRegion2D &partialRange, WorldHeightMap *htMap, RefRenderObjListIterator *pLightsIterator) = 0;
	virtual void vslot137(Int value); // 0x00068441 ignores its argument
	virtual void reset();
	virtual void vslot139(Bool flag);
	virtual void vslot140(void *quad) = 0;
	virtual void vslot141(const void *view, AABoxClass *box) = 0;
	virtual Bool vslot142(Int value);
	virtual void vslot143();
	virtual Int vslot144();
	virtual Real getHeightMapHeight(Real x, Real y, Coord3D *normal) const;
	virtual Real getMaxCellHeight(Real x, Real y) const;
	virtual Int vslot147() = 0;

	Bool isClearLineOfSight(const Coord3D &start, const Coord3D &end) const;
	void removeAllTerrainBibs();
	void writeEnvironmentData(DataChunkOutput *chunk);
	Bool showAsVisibleCliff(Int x, Int y) const;
	Bool showAsVisibleImpassable(Int x, Int y) const;
	void addProp(Int id, Coord3D location, Real angle, Real scale, const AsciiString &modelName, Bool distanceFog);
	Bool isCliffCell(Real x, Real y);
	UnsignedByte rva0006B187(Real x, Real y);
	UnsignedByte rva00067225(Real x, Real y);
	Bool rva00067298(Real x, Real y);
	UnsignedByte rva0006730B(Real x, Real y);
	void loadRoadsAndBridges(W3DTerrainLogic *terrain, Bool saveGame);

	WorldHeightMap *getMap() const { return m_map; }
	Real getTransparentWaterDepth() const { return m_transparentWaterDepth; }
	Real getMinWaterOpacity() const { return m_minWaterOpacity; }

protected:
	virtual void loadPostProcess() {}
	virtual const char *GetSnapshotName() const;
	virtual void xfer(Xfer *xfer);

public:
	UnsignedByte m_unk0xCC[0xC];
	Int m_curNumScorchVertices;
	Int m_curNumScorchIndices;
	UnsignedByte m_unk0xE0[0x36B0];
	Int m_numScorches;
	Int m_scorchesInBuffer;
	UnsignedByte m_unk0x3798[0x28];
	WorldHeightMap *m_map;
	UnsignedByte m_unk0x37C4[0x10];
	Bool m_needFullUpdate;
	UnsignedByte m_unk0x37D5[0xF];
	Real m_transparentWaterDepth;
	Real m_minWaterOpacity;
	_STL::vector<bool> m_showAsVisibleCliff;
	_STL::vector<bool> m_showAsVisibleImpassable;
	UnsignedByte m_unk0x3814[0xC];
	AsciiString m_environmentName0;
	UnsignedByte m_unk0x3824[8];
	AsciiString m_environmentName1;
	UnsignedByte m_unk0x3830[4];
	Bool m_environmentFlag;
	UnsignedByte m_unk0x3835[0x1B];
	W3DTreeBuffer *m_treeBuffer;
	UnsignedByte m_unk0x3854[4];
	W3DPropBuffer *m_propBuffer;
	W3DBibBuffer *m_bibBuffer;
	UnsignedByte m_unk0x3860[0xC];
	W3DRoadBuffer *m_roadBuffer;
	W3DBridgeBuffer *m_bridgeBuffer;
	UnsignedByte m_unk0x3874[4];
	W3DShroud *m_shroud;
	W3DTaint *m_taint;
	UnsignedByte m_unk0x3880;
};

typedef char BaseHeightMapEnvName0Offset[(offsetof(BaseHeightMapRenderObjClass, m_environmentName0) == 0x3820) ? 1 : -1];
typedef char BaseHeightMapEnvName1Offset[(offsetof(BaseHeightMapRenderObjClass, m_environmentName1) == 0x382C) ? 1 : -1];
typedef char BaseHeightMapSize[(sizeof(BaseHeightMapRenderObjClass) == 0x3884) ? 1 : -1];
typedef char BaseHeightMapMapOffset[(offsetof(BaseHeightMapRenderObjClass, m_map) == 0x37C0) ? 1 : -1];
typedef char BaseHeightMapWaterDepthOffset[(offsetof(BaseHeightMapRenderObjClass, m_transparentWaterDepth) == 0x37E4) ? 1 : -1];
typedef char BaseHeightMapWaterOpacityOffset[(offsetof(BaseHeightMapRenderObjClass, m_minWaterOpacity) == 0x37E8) ? 1 : -1];
typedef char BaseHeightMapShroudOffset[(offsetof(BaseHeightMapRenderObjClass, m_shroud) == 0x3878) ? 1 : -1];
typedef char BaseHeightMapNeedFullUpdateOffset[(offsetof(BaseHeightMapRenderObjClass, m_needFullUpdate) == 0x37D4) ? 1 : -1];

extern BaseHeightMapRenderObjClass *TheTerrainRenderObject;

#endif
