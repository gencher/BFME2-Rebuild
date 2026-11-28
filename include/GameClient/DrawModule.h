// DrawModule.h: the client draw module, the object-draw interface and the
// small draw interfaces the module hands out.
// Target facts (code RVAs; tables are RVAs of .rdata, VA = RVA + 0x400000):
//  - DrawModule : DrawableModule, no data of its own (W3DPropDraw's ctor 0x000CEEC0
//    and W3DBoatWakeModelDraw's 0x000D0B34 start their fields at +0x0C). Ctor
//    0x000B19A1 calls DrawableModule's and stores table 0x007C9690 (62 slots);
//    the destructor 0x000CEB6F stores it again and ends in 0x0049B47C.
//  - Slots 0-10 are DrawableModule's. Slots 11-61 are named below from the
//    overrides in W3DModelDraw (via W3DTankDraw 0x007CCBE8), W3DDebrisDraw
//    (0x007C97A8), W3DLaserDraw (0x007CB9E0), W3DRopeDraw (0x007CBD60) and the
//    other draw modules' tables, and from the Drawable functions that forward to
//    them (Drawable.cpp: releaseShadows 0x00272A38 calls slot 13, allocateShadows
//    0x00272A51 slot 14, 0x00272870 stores +0xA8 and calls slot 20, the death-
//    decal virtuals 0x0027299A..0x00272971 call slots 29-33). Base bodies are the
//    folded targets the base table carries: __purecall (11-14, 35, 38, 39),
//    `ret` / `ret 4` / `ret 8` / `ret 0Ch` (stack widths give the argument
//    counts), `xor al,al` (17, 40, 55), `mov al,1` (37, 60), `xor eax,eax`
//    (41-49, 51, 52) and `fld 0.0f` (50, the map's ?vslot50@DrawModule@@UAEMXZ).
//  - The interface getters come in const/non-const pairs. The const one is the
//    lower slot (MSVC lays same-name overloads out in reverse order, so the
//    non-const one is declared first): LaserUpdate::getCurrentLaserRadius
//    0x00363193 (a const member) calls slot 47; Drawable's non-const
//    setAnimationLoopDuration 0x00272393 calls slot 42. Each pair's override
//    returns the same sub-object (0x000CA841: this+0x0C, or NULL).
//  - Unidentified slots keep neutral names; their argument counts are the
//    base bodies' stack widths.
#pragma once
#ifndef BFME2_DRAWMODULE_H
#define BFME2_DRAWMODULE_H

#include "Common/GameType.h"
#include "GameLogic/Module/Module.h"
#include "Common/Xfer.h"

class AsciiString;
class Image;
class Matrix3D;
class Object;
class RenderObjClass;
class Thing;
class Xfer;
class Coord3D;
class ModelConditionFlags;
class RadiusDecalTemplate;
class FXList;
class BridgeInfo;
class PolygonTrigger;
class OBBoxClass;
enum TerrainDecalType;		// values not recovered; Drawable's ctor default is 7

class LaserDrawInterface
{
public:
	virtual Real getLaserTemplateWidth() const = 0;		// slot 0
};

class DebrisDrawInterface;
class RopeDrawInterface;

// The model-draw interface (W3DModelDraw's +0x0C base). 49 slots and no
// destructor; its own table 0x007C9B78 (stored by 0x000B2E0B) holds
// __purecall except where a default body is noted. Names are the map's
// override names in W3DModelDraw / W3DScriptedModelDraw (table 0x007CC588)
// where they are real; the slots Drawable calls carry Drawable's argument
// shapes. Slots 7, 8, 18, 21, 23, 24 and 27 take Zero Hour's ObjectDrawInterface
// names (same order and argument counts; Drawable's callers agree: setSelectable
// 0x00271700 calls 24, setIndicatorColor 0x002741DE stores +0x454 and 0x00270FAC
// passes it to 21).
class ObjectDrawInterface
{
public:
	virtual Bool clientOnly_getRenderObjInfo(Coord3D *pos, Real *boundingSphereRadius, Matrix3D *transform) const = 0;	// 0
	virtual Bool clientOnly_getRenderObjBoundBox(OBBoxClass *boundingBox) const = 0;						// 1
	virtual Bool clientOnly_getRenderObjBoneTransform(const AsciiString &boneName, Matrix3D *set) const = 0;	// 2
	virtual Int getPristineBonePositionsForConditionState(const ModelConditionFlags &condition, const char *boneNamePrefix,
		Int startIndex, Coord3D *positions, Matrix3D *transforms, Int maxBones, Int *unused) const = 0;	// 3
	virtual Int getCurrentBonePositions(const char *boneNamePrefix, Int startIndex, Coord3D *positions,
		Matrix3D *transforms, Int maxBones) const = 0;													// 4
	virtual Bool getCurrentWorldspaceClientBonePositions(const char *boneName, Matrix3D &transform) const = 0;	// 5 (Drawable 0x00272835)
	virtual Bool getProjectileLaunchOffset(const ModelConditionFlags &condition, Int wslot, Int specificBarrelToUse,
		Matrix3D *launchPos, Int tur, Coord3D *turretRotPos, Coord3D *turretPitchPos) const = 0;			// 6
	virtual void updateProjectileClipStatus(UnsignedInt shotsRemaining, UnsignedInt maxShots, Int slot) = 0;														// 7
	virtual void updateDrawModuleSupplyStatus(Int maxSupply, Int currentSupply) = 0;																// 8
	virtual Int getPlayingAnimationCount() = 0;															// 9
	virtual AsciiString getPlayingAnimationName(Int index) = 0;											// 10
	virtual void *getPlayingAnimationNameKey(Int index) = 0;											// 11
	virtual void buildPlayingAnimationStringInfo(Int index, AsciiString *name, AsciiString *info) = 0;	// 12
	virtual void *vslot13() = 0;																		// 13
	virtual Bool getAnimationFrameCount(const AsciiString &name, Int *count) { return false; }		// 14, default 0x0050B5F1
	virtual void getPlayingAnimationFrameRanges(Int index, Real *first, Real *last) = 0;				// 15
	virtual void vslot16(Int a) = 0;																	// 16
	virtual void vslot17(Int a, Int b) = 0;																// 17
	virtual void notifyDrawModuleDependencyCleared() = 0;																			// 18
	virtual void vslot19(Int a) = 0;																	// 19
	virtual void vslot20(Real a) = 0;																	// 20
	virtual void replaceIndicatorColor(UnsignedInt color) = 0;															// 21
	virtual Bool handleWeaponFireFX(Int wslot, Int specificBarrelToUse, const FXList *fxl, Real weaponSpeed,
		const Coord3D *victimPos) = 0;																	// 22
	virtual Int getBarrelCount(Int wslot) const = 0;																		// 23
	virtual void setSelectable(Bool selectable) = 0;																	// 24
	virtual void setAnimationLoopDuration(UnsignedInt numFrames) = 0;									// 25 (Drawable 0x00272393)
	virtual void setAnimationCompletionTime(UnsignedInt numFrames) = 0;								// 26 (Drawable 0x002723C0)
	virtual Bool updateBonesForClientParticleSystems(Bool a) = 0;																	// 27
	virtual void setPauseAnimation(Bool pauseAnim) = 0;													// 28
	virtual void updateSubObjects() = 0;																// 29 (Drawable 0x002723ED)
	virtual void vslot30(Int a) {}																		// 30, default `ret 4`
	virtual void vslot31(Int a, Int b) {}																// 31, default `ret 8` (Drawable 0x0027248A)
	virtual void drawInterfaceSlot32(Int a) {}															// 32, default `ret 4` (Drawable 0x002724C4); not vslot32, which DrawModule's slot 32 already is (W3DScriptedModelDraw overrides both with different bodies)
	virtual void vslot33(const void *textures) {}																		// 33, default `ret 4`; takes a pointer to a vector of texture-replacement pairs (the map's rva000C199D(const vector<Rva00B6CF1> &), one pointer wide: SubObjectsUpgrade::swapTextures 0x004B4F91 passes its UpgradeTexture list or an empty vector)
	virtual void vslot34(const AsciiString &a, Bool b, Bool c, Real d, Real e) = 0;					// 34
	virtual void vslot35(AsciiString a, Bool b, UnsignedInt c) = 0;									// 35
	virtual Int vslot36(Int a) { return 0; }															// 36, default `xor eax,eax; ret 4`
	virtual Int vslot37(Int a) { return 0; }															// 37
	virtual Int vslot38(Int a) { return 0; }															// 38
	virtual Int vslot39(Int a) { return 0; }															// 39
	virtual Int vslot40() { return 0; }																	// 40, default `xor eax,eax`
	virtual Int vslot41() { return 0; }																	// 41
	virtual Bool vslot42(BridgeInfo *info, PolygonTrigger **trigger, RenderObjClass **robj, Bool b) { return false; }	// 42, default `ret 10h`
	virtual Bool vslot43(BridgeInfo *info, PolygonTrigger **trigger, RenderObjClass **robj, Bool b) { return false; }	// 43
	virtual Bool vslot44(Int a, void *b) { return false; }												// 44, default `ret 8`
	virtual void vslot45() {}																			// 45, default `ret`
	virtual void vslot46() {}																			// 46
	virtual Bool vslot47(Int *a, void *b) = 0;															// 47
	virtual void vslot48(Int a) const {}																// 48, default `ret 4` (Drawable 0x00271C38, const)
};

class DrawModule : public DrawableModule
{
public:
	DrawModule(Thing *thing, const ModuleData *moduleData);					// 0x000B19A1

	virtual void doDrawModule(const Matrix3D *transformMtx) = 0;				// 11
	virtual void setShadowsEnabled(Bool enable) = 0;							// 12 (Drawable 0x00272A02)
	virtual void releaseShadows() = 0;											// 13 (Drawable 0x00272A38)
	virtual void allocateShadows() = 0;											// 14 (Drawable 0x00272A51)
	virtual void vslot15() {}													// 15
	virtual void setHidden(Bool hidden) {}										// 16
	virtual Bool vslot17() { return false; }									// 17
	virtual void vslot18(Bool a) {}											// 18
	virtual void vslot19(const AsciiString &a) {}								// 19 (Drawable 0x00272895)
	virtual void setTerrainDecal(TerrainDecalType type) {}						// 20 (Drawable 0x00272870)
	virtual void setTerrainDecalSize(Real x, Real y) {}						// 21
	virtual void setTerrainDecalOpacity(Real opacity) {}						// 22
	virtual void vslot23(Int a) {}												// 23 (Drawable 0x0027290C)
	virtual void vslot24(Int a) {}												// 24 (Drawable 0x00272945)
	virtual void vslot25(const Coord3D *a, const Coord3D *b) {}				// 25 (Drawable 0x0027292F)
	virtual void vslot26(void *a) {}											// 26 (Drawable 0x0027295B)
	virtual void vslot27(Int a, Int b) {}										// 27 (Drawable 0x00272BAB)
	virtual void vslot28() {}													// 28 (Drawable 0x00272BE7)
	virtual void vslot29(Int a) {}												// 29 (Drawable slot 8, 0x0027299A)
	virtual void setDeathDecalPosition(Int a) {}								// 30 (Drawable slot 9, 0x002729B5)
	virtual void vslot31(Real a) {}											// 31 (Drawable slot 10, 0x002729CB)
	virtual void vslot32(Int a) {}												// 32 (Drawable slot 11, 0x002729E9)
	virtual void createDeathDecal(Int a, Real b, Int c) {}						// 33 (Drawable slot 12, 0x00272971)
	virtual void vslot34() {}													// 34
	virtual void setFullyObscuredByShroud(Bool fullyObscured) = 0;				// 35
	virtual void vslot36() {}													// 36
	virtual Bool isVisible() const { return true; }							// 37
	virtual void reactToTransformChange(const Matrix3D *oldMtx, const Coord3D *oldPos, Real oldAngle) = 0;	// 38
	virtual void reactToGeometryChange() = 0;									// 39
	virtual Bool isLaser() const { return false; }								// 40
	virtual ObjectDrawInterface *getObjectDrawInterface() { return 0; }						// 42
	virtual const ObjectDrawInterface *getObjectDrawInterface() const { return 0; }			// 41
	virtual DebrisDrawInterface *getDebrisDrawInterface() { return 0; }						// 44
	virtual const DebrisDrawInterface *getDebrisDrawInterface() const { return 0; }			// 43
	virtual RopeDrawInterface *getRopeDrawInterface() { return 0; }							// 46
	virtual const RopeDrawInterface *getRopeDrawInterface() const { return 0; }				// 45
	virtual LaserDrawInterface *getLaserDrawInterface() { return 0; }						// 48
	virtual const LaserDrawInterface *getLaserDrawInterface() const { return 0; }			// 47
	virtual RenderObjClass *getRenderObject() { return 0; }					// 49 (W3DGhostObject 0x00063426 asks it)
	virtual Real vslot50() { return 0.0f; }									// 50
	virtual const Image *getPortraitImage() { return 0; }						// 51 (Drawable 0x00272709; W3DModelDraw's override is the map's GetPortraitImage)
	virtual const Image *getButtonImage() { return 0; }						// 52 (Drawable 0x0027272B; W3DScriptedModelDraw::getButtonImage)
	virtual void vslot53(Int a) {}												// 53
	virtual void SetRampMeshOverload(const char *name, Int index) {}								// 54
	virtual Bool vslot55() { return false; }									// 55 (Drawable 0x00272C1E)
	virtual void vslot56(AsciiString *a, Int b) {}								// 56
	virtual void vslot57() {}													// 57 (Drawable 0x0027366A)
	virtual void vslot58(Bool a) {}											// 58 (Drawable 0x00273686)
	virtual void vslot59(void *a) {}											// 59 (Drawable 0x00273648)
	virtual Bool vslot60() { return true; }									// 60
	virtual void vslot61() {}													// 61 (Drawable 0x00272DBB)

protected:
	virtual ~DrawModule() {}													// COMDAT 0x000CEB6F (map: placeholder name); inlined into the derived destructors (W3DDebrisDraw 0x000B1CE2 stores table 0x007C9690 then calls 0x0049B47C)
	virtual void loadPostProcess();											// slot 1; empty, out of line (derived bodies jmp to it)
	virtual const char *GetSnapshotName() const;								// slot 2, 0x000B19BD
	// fold 0x004CBF58: DrawableModule's (0x00560AE1), then Version 1 (W3DDebrisDraw::xfer and LaserUpdate call it)
	virtual void xfer(Xfer *xfer)
	{
		DrawableModule::xfer(xfer);
		xfer->Version1();
	}
};

#endif
