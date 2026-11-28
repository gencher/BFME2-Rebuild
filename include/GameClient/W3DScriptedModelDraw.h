// W3DScriptedModelDraw.h: BFME 2's model draw module, the base of the W3D model draw family.
// Target facts (code RVAs; tables are .rdata RVAs, VA = RVA + 0x400000):
//  - Retail has no separate W3DModelDraw class: the image registers only the module name
//    "W3DScriptedModelDraw", and its constructor 0x000C0DD8 calls DrawModule's 0x000B19A1 directly,
//    then stores three tables: 0x007CA090 (68 slots) at +0x00, 0x007CC588 (ObjectDrawInterface,
//    49 slots) at +0x0C and 0x007CA08C (1 slot) at +0x10. The map's W3DModelDraw:: names on this
//    class's bodies are donor spellings of Zero Hour's W3DModelDraw, which this class corresponds to.
//  - The derived draw modules (W3DHordeModelDraw 0x00078A82, W3DTruckDraw 0x000CB3FB,
//    W3DSailModelDraw 0x000D078F, ...) call 0x000C0DD8 and store the same two secondary tables, so
//    they derive from this class and override only primary-table slots.
//  - Primary slots 0-61 are DrawModule's; the overrides below are the slots whose target differs
//    from DrawModule's table 0x007C9690. Slots 62-67 are this class's own virtuals.
//  - The +0x10 base is a one-slot interface (slot 0 0x000BFB76 takes a const ModelConditionFlags &);
//    slots 9 and 10 (0x000CF155) hand it out. Its class name is not recovered, so it is not
//    declared yet, and neither are the data members.
#pragma once
#ifndef BFME2_W3DSCRIPTEDMODELDRAW_H
#define BFME2_W3DSCRIPTEDMODELDRAW_H

#include "GameClient/DrawModule.h"
#include "WW3D2/Shadow.h"

class INI;
class ModelConditionFlags;
class RenderObjClass;

// One model condition state of the draw module data. Only the fields the recovered code touches are named.
struct ModelConditionInfo
{
	static void parseRealRange(INI *ini, void *instance, void *store, const void *userData);

	Byte m_unk0x00[0x24];
	Real m_animMinSpeedFactor;		// 0x24
	Real m_animMaxSpeedFactor;		// 0x28
};

// The one-slot interface at +0x10 of the model draw module.
class ModelConditionInterface
{
public:
	virtual void onModelConditionChange(const ModelConditionFlags &flags, Bool a, Int b) = 0;	// 0
};

class W3DScriptedModelDraw : public DrawModule, public ObjectDrawInterface, public ModelConditionInterface
{
public:
	W3DScriptedModelDraw(Thing *thing, const ModuleData *moduleData);		// 0x000C0DD8

	// DrawModule (primary table 0x007CA090)
	virtual NameKeyType getModuleNameKey() const;							// 4
	virtual void onDrawableBoundToObject();									// 6
	virtual void preloadAssets();											// 7
	virtual void *vslot09();												// 9: this + 0x10, the one-slot interface (0x000CF155)
	virtual void *vslot10();												// 10: the same body
	virtual void doDrawModule(const Matrix3D *transformMtx);				// 11
	virtual void setShadowsEnabled(Bool enable);							// 12
	virtual void releaseShadows();											// 13
	virtual void allocateShadows();											// 14
	virtual void vslot15();													// 15
	virtual void setHidden(Bool hidden);									// 16
	virtual Bool vslot17();													// 17
	virtual void vslot18(Bool a);											// 18
	virtual void vslot19(const AsciiString &a);								// 19
	virtual void setTerrainDecal(TerrainDecalType type);					// 20
	virtual void setTerrainDecalSize(Real x, Real y);						// 21
	virtual void setTerrainDecalOpacity(Real opacity);						// 22
	virtual void vslot23(Int a);											// 23
	virtual void vslot24(Int a);											// 24
	virtual void vslot25(const Coord3D *a, const Coord3D *b);				// 25
	virtual void vslot26(void *a);											// 26
	virtual void vslot27(Int a, Int b);										// 27
	virtual void vslot28();													// 28
	virtual void vslot29(Int a);											// 29
	virtual void setDeathDecalPosition(Int a);								// 30
	virtual void vslot31(Real a);											// 31
	virtual void vslot32(Int a);											// 32
	virtual void createDeathDecal(Int a, Real b, Int c);					// 33
	virtual void vslot34();													// 34
	virtual void setFullyObscuredByShroud(Bool fullyObscured);				// 35
	virtual void vslot36();													// 36
	virtual Bool isVisible() const;											// 37
	virtual void reactToTransformChange(const Matrix3D *oldMtx, const Coord3D *oldPos, Real oldAngle);	// 38
	virtual void reactToGeometryChange() {}									// 39: the shared `ret` 0x000B3FD0 (Zero Hour's is empty too)
	virtual ObjectDrawInterface *getObjectDrawInterface() { return this; }				// 42: 0x000CA841, this + 0x0C
	virtual const ObjectDrawInterface *getObjectDrawInterface() const { return this; }	// 41: the same body
	virtual RenderObjClass *getRenderObject();								// 49: a member load that folds with another class's getter at 0x00225A98
	virtual Real vslot50();													// 50
	virtual const Image *getPortraitImage();								// 51
	virtual const Image *getButtonImage();									// 52
	virtual void vslot53(Int a);											// 53
	virtual void vslot54(const char *a, Int b);								// 54
	virtual Bool vslot55();													// 55
	virtual void vslot56(AsciiString *a, Int b);							// 56
	virtual void vslot57();													// 57
	virtual void vslot58(Bool a);											// 58
	virtual void vslot59(void *a);											// 59
	virtual void vslot61();													// 61

	// ObjectDrawInterface (table 0x007CC588): every slot is this class's
	virtual Bool clientOnly_getRenderObjInfo(Coord3D *pos, Real *boundingSphereRadius, Matrix3D *transform) const;	// 0
	virtual Bool clientOnly_getRenderObjBoundBox(OBBoxClass *boundingBox) const;						// 1
	virtual Bool clientOnly_getRenderObjBoneTransform(const AsciiString &boneName, Matrix3D *set) const;	// 2
	virtual Int getPristineBonePositionsForConditionState(const ModelConditionFlags &condition, const char *boneNamePrefix,
		Int startIndex, Coord3D *positions, Matrix3D *transforms, Int maxBones, Int *unused) const;	// 3
	virtual Int getCurrentBonePositions(const char *boneNamePrefix, Int startIndex, Coord3D *positions,
		Matrix3D *transforms, Int maxBones) const;													// 4
	virtual Bool getCurrentWorldspaceClientBonePositions(const char *boneName, Matrix3D &transform) const;	// 5
	virtual Bool getProjectileLaunchOffset(const ModelConditionFlags &condition, Int wslot, Int specificBarrelToUse,
		Matrix3D *launchPos, Int tur, Coord3D *turretRotPos, Coord3D *turretPitchPos) const;			// 6
	virtual void updateProjectileClipStatus(UnsignedInt shotsRemaining, UnsignedInt maxShots, Int slot);	// 7
	virtual void updateDrawModuleSupplyStatus(Int maxSupply, Int currentSupply) {}			// 8: the shared `ret 8` 0x0050B238
	virtual Int getPlayingAnimationCount();									// 9
	virtual AsciiString getPlayingAnimationName(Int index);					// 10
	virtual void *getPlayingAnimationNameKey(Int index);					// 11
	virtual void buildPlayingAnimationStringInfo(Int index, AsciiString *name, AsciiString *info);	// 12
	virtual void *vslot13();												// 13
	virtual Bool getAnimationFrameCount(const AsciiString &name, Int *count);	// 14
	virtual void getPlayingAnimationFrameRanges(Int index, Real *first, Real *last);	// 15
	virtual void vslot16(Int a);											// 16
	virtual void vslot17(Int a, Int b);										// 17
	virtual void notifyDrawModuleDependencyCleared();						// 18
	virtual void vslot19(Int a);											// 19
	virtual void vslot20(Real a);											// 20
	virtual void replaceIndicatorColor(UnsignedInt color);					// 21
	virtual Bool handleWeaponFireFX(Int wslot, Int specificBarrelToUse, const FXList *fxl, Real weaponSpeed,
		const Coord3D *victimPos);											// 22
	virtual Int getBarrelCount(Int wslot) const;							// 23
	virtual void setSelectable(Bool selectable);							// 24
	virtual void setAnimationLoopDuration(UnsignedInt numFrames);			// 25
	virtual void setAnimationCompletionTime(UnsignedInt numFrames);			// 26
	virtual Bool updateBonesForClientParticleSystems(Bool a);				// 27
	virtual void setPauseAnimation(Bool pauseAnim);							// 28
	virtual void updateSubObjects();										// 29
	virtual void vslot30(Int a);											// 30
	virtual void vslot31(Int a, Int b);										// 31
	virtual void drawInterfaceSlot32(Int a);								// 32
	virtual void vslot33(const void *textures);								// 33
	virtual void vslot34(const AsciiString &a, Bool b, Bool c, Real d, Real e);	// 34
	virtual void vslot35(AsciiString a, Bool b, UnsignedInt c);				// 35
	virtual Int vslot36(Int a);												// 36
	virtual Int vslot37(Int a);												// 37
	virtual Int vslot38(Int a);												// 38
	virtual Int vslot39(Int a);												// 39
	virtual Int vslot40();													// 40
	virtual Int vslot41();													// 41
	virtual Bool vslot42(BridgeInfo *info, PolygonTrigger **trigger, RenderObjClass **robj, Bool b);	// 42
	virtual Bool vslot43(BridgeInfo *info, PolygonTrigger **trigger, RenderObjClass **robj, Bool b);	// 43
	virtual Bool vslot44(Int a, void *b);									// 44
	virtual void vslot45();													// 45
	virtual void vslot46();													// 46
	virtual Bool vslot47(Int *a, void *b) { return true; }					// 47: the shared `mov al,1; ret 8` 0x0050B5F1
	virtual void vslot48(Int a) const;										// 48

	// Slot 63 is public: the ledger spells W3DTankDraw's and W3DTruckDraw's overrides UAE.
	virtual void onRenderObjRecreated() {}									// 63: the shared `ret`; W3DTruckDraw and W3DTankDraw override it (map names, Zero Hour's)

protected:
	virtual ~W3DScriptedModelDraw();										// 0x000C79C9
	virtual void loadPostProcess();											// 1
	virtual const char *GetSnapshotName() const;							// 2
	virtual void xfer(Xfer *xfer);											// 3

	// This class's own virtuals, slots 62-67 of the primary table.
	virtual void slot62();													// 62: 0x000B3D39, no arguments
	virtual Bool setModelState(const ModelConditionInfo *newState, Bool arg2, void *arg3);	// 64: 0x000C4E23 (map's spelling; the last argument's type is not recovered)
	virtual void adjustTransformMtx(Matrix3D &mtx) const {}				// 65: the shared `ret 4`; W3DSailModelDraw's override 0x000D08C3 transforms the matrix (Zero Hour's name)
	virtual Int slot66();													// 66: 0x000C124C returns 100000
	virtual Int slot67(Int count);											// 67: 0x000B3E96, a client random value in [0, count - 1]

	Byte m_unk0x14[0x30];			// 0x14
	RenderObjClass *m_unk0x44;		// 0x44
	Byte m_unk0x48[2];				// 0x48
	Bool m_shadowEnabled;			// 0x4A
	Bool m_unk0x4B;					// 0x4B
	Bool m_unk0x4C;					// 0x4C
	Bool m_unk0x4D;					// 0x4D
	Byte m_unk0x4E[2];				// 0x4E
	RenderObjClass *m_renderObject;	// 0x50
	Byte m_unk0x54[4];				// 0x54
	Shadow *m_shadow;				// 0x58
	Shadow *m_terrainDecal;			// 0x5C
	Byte m_unk0x60[0x15];			// 0x60
	Bool m_pauseAnimation;			// 0x75
	Byte m_unk0x76[2];				// 0x76
	Int m_animationMode;			// 0x78
};

#endif
