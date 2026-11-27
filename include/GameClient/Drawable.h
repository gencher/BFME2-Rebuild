// Drawable.h: the client drawable. SIZE 0x470.
// Target facts (code RVAs, tables VAs):
//  - Size 0x470: W3DGameClient::friend_createDrawable allocates 0x470 before
//    ??0Drawable(const ThingTemplate *, DrawableStatus, Int) 0x002797BD.
//  - Bases Thing +0x00 and Snapshot +0x60: the ctor calls ??0Thing, stores
//    Snapshot's table at +0x60, then Drawable's tables 0x00BFB0B4 (+0x00, 15
//    slots: Thing's 8 then 7 of its own) and 0x00BFB0A4 (+0x60, 4 slots).
//    Thing slots 3/4 are `mov eax,ecx; ret` (asDrawableMeth returns this),
//    slot 5 is reactToTransformChange 0x002742E5, slot 7 the deleting dtor
//    0x00279FFC; slots 0, 1, 2 and 6 keep Thing's bodies. Slots 8..14 carry
//    the map's names (several still address-named).
//  - Members: the ctor's stores and sub-object constructors. Names agree with
//    Zero Hour's member order where the ctor agrees with it; the rest stay
//    unnamed of the measured type or byte gaps. Three Matrix3Ds (3 x 0x10
//    rows each, eh-vector ctor) at +0x170, +0x1A0 (the instance matrix
//    getInstanceMatrix returns) and +0x1D0, a fourth at +0x208; three 0x4C bit
//    sets (memset ctor 0x00042526, the model-condition set size) at +0x258,
//    +0x2A4 and +0x2F0.
//  - Older facts kept: friend_bindToObject 0x00274240 (object +0xFC), setID
//    0x00271058 (ID +0x100), getDrawModules 0x002B224B (+0x14C), getID
//    0x0055A88B; getPosition 0x002763E6, getTransformMatrix and setPosition are
//    out of line on Drawable and hide Thing's inline ones.
#pragma once
#ifndef BFME2_DRAWABLE_H
#define BFME2_DRAWABLE_H

#include <stddef.h>
#include "Common/GameType.h"
#include "Common/AsciiString.h"
#include "Common/UnicodeString.h"
#include "Common/Thing.h"
#include "Common/Snapshot.h"
#include "Common/OpaqueRefElement4.h"
#include "Common/RGBColor.h"
#include "Common/BitFlags.h"
#include "GameClient/DrawableAudioEvent.h"
#include "GameClient/DrawableInterpolationHistory.h"
#include "WWMath/Matrix3D.h"
enum DrawableStatus;
class ClientUpdateModule;
class Image;
class ModelConditionFlags;
enum TerrainDecalType;
class DrawModule;
class DisplayString;
class BuffManager;
class ModelConditionFlags;
class Module;
class DrawableLocoInfo;
class GeometryInfo;
class FXList;
class BridgeInfo;
class PolygonTrigger;
class RenderObjClass;
class Object;
class SlavedUpdateInterface;
class Locomotor;
class Xfer;
class Rva002390CB;
class Coord3D;

// The drawable's status icons; values and spellings follow retail's name table
// TheDrawableIconNames (VA 0x00DBB660, 14 names and a NULL).
enum DrawableIconType
{
	ICON_INVALID = -1,
	ICON_FIRST = 0,
	ICON_DEFAULT_HEAL = ICON_FIRST,
	ICON_STRUCTURE_HEAL,
	ICON_VEHICLE_HEAL,
	ICON_DEMORALIZED_OBSOLETE,
	ICON_BOMB_TIMED,
	ICON_BOMB_REMOTE,
	ICON_DISABLED,
	ICON_BATTLEPLAN_BOMBARD,
	ICON_BATTLEPLAN_HOLDTHELINE,
	ICON_BATTLEPLAN_SEEKANDDESTROY,
	ICON_EMOTICON,
	ICON_ENTHUSIASTIC,
	ICON_SUBLIMINAL,
	ICON_CARBOMB,
	MAX_ICONS
};
extern const char *TheDrawableIconNames[];
// drawableIconIndexToName 0x0027015F takes its index in eax: a file-static of Drawable.cpp.
DrawableIconType drawableIconNameToIndex(const char *name);		// 0x00270167

#define DEF_ATTACK_FRAMES (1)
#define DEF_SUSTAIN_FRAMES (1)
#define DEF_DECAY_FRAMES (4)
#define SUSTAIN_INDEFINITELY (0xfffffffe)

// TintEnvelope: Zero Hour's colour envelope (Drawable.h), with BFME 2's additions.
// SIZE 0x50: Drawable::colorFlash 0x0027541E and colorTint 0x00275490 allocate 0x50
// bytes and run the ctor COMDAT 0x00271826 (map: ??0Rva00271826) into Drawable +0x68.
// Table 0x007FAF68: deleting dtor (fold 0x004A10FD), loadPostProcess (empty fold
// 0x000B3FD0), GetSnapshotName 0x00271892, xfer 0x00271A0F (Version1, four Vector3,
// the counter +0x34, Bool +0x39, Byte +0x38: Zero Hour's xfer order).
// Members +0x04..+0x39 are Zero Hour's (setAttackFrames 0x0027192B, setDecayFrames
// 0x002719C0, play 0x002744F2); +0x3C/+0x40 (0x002719A7) and +0x44 are BFME 2's.
class TintEnvelope : public Snapshot
{
	friend class Drawable;	// clearTintStatus 0x00270619 sets the counter and the state directly
public:
	TintEnvelope()
	{
		m_attackRate.Set(0, 0, 0);
		m_decayRate.Set(0, 0, 0);
		m_peakColor.Set(0, 0, 0);
		m_currentColor.Set(0, 0, 0);
		m_unk0x44.Set(0, 0, 0);
		m_envState = ENVELOPE_STATE_REST;
		m_sustainCounter = 0;
		m_affect = FALSE;
		m_unk0x3C = 0.0f;
		m_unk0x40 = 0.0f;
	}
	void update();
	void play(const RGBColor *peak,
			  UnsignedInt attackFrames = DEF_ATTACK_FRAMES,
			  UnsignedInt decayFrames = DEF_DECAY_FRAMES,
			  UnsignedInt sustainAtPeak = DEF_SUSTAIN_FRAMES);
	void sustain() { m_envState = ENVELOPE_STATE_SUSTAIN; }
	void release() { m_envState = ENVELOPE_STATE_DECAY; }
	void rest() { m_envState = ENVELOPE_STATE_REST; }
	Bool isEffective() const { return m_affect; }
	const Vector3 *getColor() const { return &m_currentColor; }
	void rva002719A7(Real a, Real b);		// 0x002719A7: stores +0x3C and +0x40

protected:
	virtual void loadPostProcess() {}
	virtual const char *GetSnapshotName() const;
	virtual void xfer(Xfer *xfer);

private:
	void setAttackFrames(UnsignedInt frames);
	void setDecayFrames(UnsignedInt frames);
	void setPeakColor(const RGBColor *peak) { m_peakColor = Vector3(peak->red, peak->green, peak->blue); }

	enum EnvelopeStatesEnum
	{
		ENVELOPE_STATE_REST,
		ENVELOPE_STATE_ATTACK,
		ENVELOPE_STATE_DECAY,
		ENVELOPE_STATE_SUSTAIN
	};

	Vector3 m_attackRate;				// +0x04
	Vector3 m_decayRate;				// +0x10
	Vector3 m_peakColor;				// +0x1C
	Vector3 m_currentColor;				// +0x28
	UnsignedInt m_sustainCounter;		// +0x34
	Char m_envState;					// +0x38 (xfer through operator==(char &), Zero Hour's Byte)
	Bool m_affect;						// +0x39
	Real m_unk0x3C;						// +0x3C
	Real m_unk0x40;						// +0x40
	Vector3 m_unk0x44;					// +0x44
};
// SIZE 0x50

class Drawable : public Thing, public Snapshot
{
public:
	// The per-frame tilt the locomotor-specific physics produce, 0x10 bytes:
	// calcPhysicsXformTreads 0x00270817 stores +0x00, +0x04 and +0x0C,
	// rva00270B0F stores +0x04; names are Zero Hour's for the same roles.
	struct PhysicsXformInfo
	{
		Real m_totalPitch;		// +0x00
		Real m_totalRoll;		// +0x04
		Real m_totalYaw;		// +0x08
		Real m_totalZ;			// +0x0C
	};

	void calcPhysicsXformHugeFourLegs(const Locomotor *locomotor, PhysicsXformInfo &info);	// 0x0027B47F
	Bool calcPhysicsXform(PhysicsXformInfo &info);					// 0x0027BA95
	void applyPhysicsXform(Matrix3D *mtx);							// 0x0027BB65

protected:
	void calcPhysicsXformTreads(const Locomotor *locomotor, PhysicsXformInfo &info);	// 0x00270817
	void rva00270B0F(const Locomotor *locomotor, PhysicsXformInfo &info);		// 0x00270B0F
	void rva00272FB6(const Locomotor *locomotor, PhysicsXformInfo &info);		// 0x00272FB6
	void calcPhysicsXformWheels(const Locomotor *locomotor, PhysicsXformInfo &info);	// 0x00276CFB

public:
 const Matrix3D *getInstanceMatrix() const { return &m_instance; }
 void setInstanceMatrix(const Matrix3D *,bool);
 void colorFlash(const RGBColor *peak, UnsignedInt decayFrames, UnsignedInt attackFrames, UnsignedInt sustainAtPeak);	// 0x0027541E
 void colorTint(const RGBColor *peak);	// 0x00275490
 void flashAsSelected(const RGBColor *color);	// 0x00278C7C (CommandTranslator::createAttackMessage 0x004293EE; Zero Hour's name)
 UnicodeString getCaptionText();	// 0x0027653A (ControlBar::populateBeacon 0x0050DBE6; copies the caption at +0x344)
 void saturateRGB(RGBColor &color, Real factor);	// 0x002701A8
 void friend_bindToObject(Object *obj);	// 0x00274240
 void friend_clearSelected();	// 0x00276B95 (InGameUI::deselectDrawable 0x0029F34F)
 void friend_setSelected();	// 0x002796B8 (InGameUI::selectDrawable 0x002A3805)
 void fadeIn(UnsignedInt frames);	// 0x002707A8: fade mode 1 from transparent; 0 frames shows at once
 void fadeOut(UnsignedInt frames);	// 0x00270756: fade mode 2 from opaque; 0 frames hides at once (FoundationAIUpdate::rva0045527A passes 10)
 // (ActiveBody 0x004BE47D, 0x004BFE07): 0x0027675F returns the sound record at an index; the five wrappers pass a constant.
 Rva002390CB rva0027675F(Int index);	// 0x0027675F
 Rva002390CB getSoundMoveLoopDamaged();	// 0x0027682F, index 0x25 (donor-order role name)
 Rva002390CB getSoundAmbient();	// 0x00276848, index 0x26
 Rva002390CB getSoundAmbientDamaged();	// 0x00276861, index 0x27
 Rva002390CB getSoundAmbientReallyDamaged();	// 0x0027687A, index 0x28
 Rva002390CB getSoundAmbientRubble();	// 0x00276893, index 0x29
 Bool hasSoundAtIndex(Int index);	// 0x00276805: true when the sound record at the index holds a sound
 Rva002390CB getSoundEnter();	// 0x00462D95, index 0x2F
 Rva002390CB getSoundExit();	// 0x00462DAE, index 0x30
 Rva002390CB getSoundFalling();	// 0x00462DC7, index 0x34
 Rva002390CB getSoundOnDamaged();	// 0x004BE120, index 0x2D
 Rva002390CB getSoundOnReallyDamaged();	// 0x004BE139, index 0x2E
 Rva002390CB getSoundPromotedVeteran();	// 0x004BE152, index 0x31
 Rva002390CB getSoundPromotedElite();	// 0x004BE16B, index 0x32
 Rva002390CB getSoundPromotedHero();	// 0x004BE184, index 0x33
 void rva00270644(Real a, Real b);	// 0x00270644: +0xD8/+0xDC, stamped with the client frame at +0x380
 void rva00271A80(Real t);			// 0x00271A80: +0xF4 = t; +0xF0 the +0xE8 range at t when +0xE4 is set
 void rva002752F8();				// 0x002752F8: rva00270644(0, -0.03), rva00272BE7, then +0x445 -> +0x446
 void rva00272BE7();				// 0x00272BE7: draw-module slot 28, then the third list's slot 18
 void setOrientation(Real);
 void rva00275DCE(Int,Real,Real,Real);
 DrawableAudioEvent rva00274CD8(const AsciiString &);
 Bool isDrawableEffectivelyHidden() const; // 0x00270260
 ClientUpdateModule *findClientUpdateModule(NameKeyType key); // 0x0027449C (the client update module with that name key, or NULL)
 void rva002791E7(const ModelConditionFlags &flags, UnsignedInt a, UnsignedInt b);	// 0x002791E7 (Object::rva0028AE6D hands it the object's conditions, 0, 0)
 void updateDrawable(); // 0x0027566B (StealthUpdate::changeVisualDisguise 0x00374BC8)
	void replaceModelConditionFlags(const ModelConditionFlags &flags, UnsignedInt forceReplace, UnsignedInt unused);	// 0x002791E7 (Object::rva0028AE6D 0x0028AE6D; Zero Hour's name)
 void setDrawableHidden(Bool hidden); // 0x00271601
 void setSelectable(Bool selectable); // 0x00271700
 void changedTeam(); // 0x002742AC
 void setIndicatorColor(Int color);	// 0x002741DE
 void rva002724FD(const AsciiString &name, Bool a, Bool b, Real c, Real d);	// 0x002724FD, object-draw slot 34
 bool rva00278689(const AsciiString &, bool, bool);	// 0x00278689, the sub-object show test the Lua CurDrawable callbacks call
 // 0x00272414, ?rva00272414@Drawable@@QAEXPBV?$vector@VAsciiString@@V?$allocator@VAsciiString@@@_STL@@@_STL@@@Z: takes a pointer to a vector<AsciiString> (SubObjectsUpgrade passes ExcludeSubobjects); spelled void* here because this header cannot see the vector type
 void rva00272414(const void *names);
 // Refreshes the drawn state after a model-condition or texture swap
 // (retail call [edx+0x1C] from FlammableUpdate 0x0048C8F9 and 0x0048CB18).
 void rva00274176(Bool set);
 // DockUpdate::loadDockPositions 0x005897FB fills up to `maxBones` bone
 // positions from a named pristine bone and returns how many it wrote.
 Int getPristineBonePositions(const char *boneName, Int startIndex, Coord3D *positions,
  Matrix3D *transforms, Int maxBones, Int unused) const;

 DrawModule **getDrawModules() const;							// 0x002B224B, +0x14C
 DrawableID getID() const;										// 0x0055A88B, +0x100
 Object *getObject() const { return m_object; }					// +0xFC
	void onSelected();	// records the object id in the game client, then runs the contain and horde-contain selection reactions
	// 0x00270FEE: the bound object's current geometry (Object +0x104), else the template's (+0xA0).
	const GeometryInfo *getGeometryInfo() const;
	// 0x0027103F: the bound object's index-th additional geometry, else getGeometryInfo().
	const GeometryInfo *getGeometryInfo(Int index) const;
 void updateInterpolationHistory(Int snap);					// 0x002747F9
 void setFullyObscuredByShroud(Bool fullyObscured);			// 0x00278C29
 const Coord3D *getPosition() const;								// 0x002763E6
 // Out of line: WeaponFireSpecialAbilityUpdate::startUnpacking (0x0049283E) calls it.
 const Matrix3D *getTransformMatrix() const;
 void setPosition(const Coord3D *pos);
 Bool getCurrentWorldspaceClientBonePositions(const char *boneName, Matrix3D &transform) const;	// 0x00272835


	UnsignedInt getStatusBits() const { return m_status; }
	// Zero Hour's tint-status pair: set is inline (PoisonedBehavior::startPoisonedEffects
	// 0x00483011 ORs +0x118 directly), clear is out of line (0x00270619; clearing 0x10 also
	// resets the +0x68 envelope).
	void setTintStatus(UnsignedInt bits) { m_tintStatus |= bits; }
	void stopAmbientSound();				// 0x002743D7
	void startAmbientSound(Bool onlyIfPermanent);	// 0x002783F6 (map: placeholder, int argument)
	void enableAmbientSound(Bool enable);	// 0x002785D6
	void clearCustomSoundAmbient(Bool restartSound);	// 0x00279354 (Zero Hour)
	void setCustomSoundAmbientInfo(const OpaqueRefElement4 &customAmbientInfo);	// 0x00279797 (Zero Hour; the map's spelling)
	void clearTintStatus(UnsignedInt bits);	// +0x114, the ctor's DrawableStatus argument
	void clearCaptionText();				// 0x00271496 (Zero Hour's: frees the caption string)
	void turnOffBuff(Int type, Bool immediate);	// 0x00271BCC
	void extendEvaSecondDamageTimeout();	// 0x00271B03
	void startVoiceAttackChargeTimer();	// 0x00271C1C
	Drawable *getNextDrawable() const { return m_nextDrawable; }	// +0x104; GameClient::releaseShadows 0x00238F3F walks it inline
	// 0x00271779: starts a colour flash (colour by value, pre, post and sustain times, frequency,
	// amplitude); TintDrawableFXNugget::doFXObj 0x001E09D8 calls it after setTintStatus(0x20).
	void rva00271779(RGBColor color, UnsignedInt preTime, UnsignedInt postTime, UnsignedInt sustainTime, Real frequency, Real amplitude);
	// 0x00276223: attaches a model by name (name by value, random rotation, expire frames);
	// AttachedModelFXNugget::doFXObj 0x001E0878 calls it.
	void rva00276223(AsciiString modelName, Bool randomlyRotate, Int expireFrames);
	// 0x00271B5C: starts a buff effect of a type (BuffNuggetFXNugget::doFXObj 0x001E311E passes the
	// type, the buff object's template or NULL, the lifetime, the colour and the extrusion).
	void rva00271B5C(Int buffType, const ThingTemplate *buffTemplate, UnsignedInt lifeTime, const RGBColor *color, Real extrusion);
	// 0x00271BCC: ends the buff effect of a type (the same doFXObj, when the lifetime is not positive).
	void rva00271BCC(Int buffType, Int unused);
	void rva0027930F(int);	// 0x0027930F; GameClient::setTimeOfDay passes the time of day here for each drawable
	void setID(DrawableID id);							// 0x00271058 (the map spells the argument void *)
	void prependToList(Drawable **pListHead);			// 0x002714E6 (map: Rva002714E6), beside removeFromList
	void removeFromList(Drawable **pListHead);			// 0x0027150A
	void releaseShadows();								// 0x00272A38 (map: rva00272A38); calls each draw module's slot 13
	void allocateShadows();								// 0x00272A51 (map: rva00272A51); calls each draw module's slot 14
	void setShadowsEnabled(Bool enable);				// 0x00272A02 (map: rva00272A02); status bit 2, then each draw module's slot 12
	void onDestroy();									// 0x00270234 (map: placeholder): Module::onDelete on every module of the three lists
	void reactToGeometryChange();						// 0x002710EC (map: placeholder): each draw module's slot 39
	void inheritSlaverFlags(const Object *slaver, SlavedUpdateInterface *slave);	// 0x002710AE (map: placeholder ?rva002710AE@Rva002710AE@@QAEXPAVBuildListInfo@@PAVArg2@@@Z): the slave's drawable takes the slaver's flag bytes +0x445/+0x446
	void rva00271547();									// 0x00271547: re-evaluates the hidden flags (setDrawableHidden calls it)
	void rva0027167F(Int value);						// 0x0027167F, object-draw slot 16
	void notifyDrawableDependencyCleared();				// 0x002716AC (map: placeholder), object-draw slot 18
	void rva002716D3(Int value);						// 0x002716D3, object-draw slot 19
	void rva00271AEA();									// 0x00271AEA, draw-module slot 15
	Bool isAmbientSoundPlaying();						// 0x002714B4 (map: placeholder): TheAudio slot 52 on +0x144
	void rva002714CA();									// 0x002714CA: removes the +0x148 sound, then marks it 1
	void rva00271C68(Int type);							// 0x00271C68: tint bit 4 on for type 26
	void rva00271C79(Int type);							// 0x00271C79: tint bit 4 off for type 26
	Bool clientOnly_getFirstRenderObjInfo(Coord3D *pos, Real *boundingSphereRadius, Matrix3D *transform);	// 0x00272366
	void rva00272C42(Int value);						// 0x00272C42, first draw module's slot 53
	void rva00272C5B(const char *a, Int b);				// 0x00272C5B, first draw module's slot 54
	Module *rva002744C7(NameKeyType key);				// 0x002744C7: the +0x154 list's module with that name key
	Bool rva00274C62(AsciiString *out);					// 0x00274C62: copies +0x348 when set
	void rva00274C88(const AsciiString &name);			// 0x00274C88: +0x34C = name, cleared for an empty one
	Bool handleWeaponFireFX(Int wslot, Int specificBarrelToUse, const FXList *fxl, Real weaponSpeed,
		Real recoilAmount, Real recoilAngle, const Coord3D *victimPos);	// 0x00271108
	void rva00270FAC(Bool on);							// 0x00270FAC: +0x3AA, then object-draw slot 21 (with +0x454 when on)
	void updateDrawableClipStatus(UnsignedInt shotsRemaining, UnsignedInt maxShots, Int slot);	// 0x00271619 (map: placeholder), object-draw slot 7
	void updateDrawableSupplyStatus(Int maxSupply, Int currentSupply);	// 0x0027164E (map: placeholder), object-draw slot 8
	Bool rva0027267D(BridgeInfo *info, PolygonTrigger **trigger, RenderObjClass **robj, Bool b);	// 0x0027267D, object-draw slot 42
	Bool rva002726C3(BridgeInfo *info, PolygonTrigger **trigger, RenderObjClass **robj, Bool b);	// 0x002726C3, object-draw slot 43
	Int getCurrentClientBonePositions(const char *boneNamePrefix, Int startIndex, Coord3D *positions,
		Matrix3D *transforms, Int maxBones) const;		// 0x002727C8
	void rva002728C5(Real x, Real y);					// 0x002728C5: terrain decal size, +0xAC their mean
	Real rva00272C9E(RenderObjClass *robj);				// 0x00272C9E
	void rva00272D03(UnsignedInt frames);				// 0x00272D03: shown, fade mode 4
	void rva00272D3D(UnsignedInt frames);				// 0x00272D3D: hidden, fade mode 3
	void rva00272D77(UnsignedInt frames, UnsignedInt holdFrames);	// 0x00272D77: hidden, fade mode 5
	Bool rva00274CB2(AsciiString *out);					// 0x00274CB2: copies +0x34C when set

	// The draw-module forwarders (DrawModule.h names the slots they reach).
	Int isSelectable() const;							// 0x00271745 (map: rva00271745): 1 when the object is selectable
	Int isMassSelectable() const;						// 0x0027175F (map: Rva0027175F::rva0027175F): likewise for mass selection
	static void friend_lockDirtyStuffForIteration();	// 0x002717EB
	static void friend_unlockDirtyStuffForIteration();	// 0x00271816
	void setAnimationLoopDuration(UnsignedInt numFrames);	// 0x00272393, object-draw slot 25
	void setAnimationCompletionTime(UnsignedInt numFrames);	// 0x002723C0 (map: rva002723C0), object-draw slot 26
	void updateSubObjects();							// 0x002723ED, object-draw slot 29
	void rva0027248A(Int a, Int b);						// 0x0027248A, object-draw slot 31 when bound to an object
	void rva002724C4(Int a);							// 0x002724C4, object-draw slot 32 when bound to an object
	Int rva00272545(Int a);								// 0x00272545, object-draw slot 36: the first non-zero answer
	Int rva0027257B(Int a);								// 0x0027257B, slot 37
	Int rva002725B1(Int a);								// 0x002725B1, slot 38
	Int rva002725E7(Int a);								// 0x002725E7, slot 39
	Int rva0027261D();									// 0x0027261D, slot 40
	Int rva0027264D();									// 0x0027264D, slot 41
	const Image *getPortraitImage();					// 0x00272709 (map: rva00272709), draw-module slot 51
	const Image *getButtonImage();						// 0x0027272B (map: rva0027272B), draw-module slot 52
	void setTerrainDecal(TerrainDecalType type);		// 0x00272870 (map: rva00272870), +0xA8, draw-module slot 20
	void rva00272895(const AsciiString &name);			// 0x00272895: +0xA8 = 7 for no name, else 6; draw-module slot 19
	void rva0027290C(Int value);						// 0x0027290C: +0x44C, draw-module slot 23
	void rva0027292F(const Coord3D *a, const Coord3D *b);	// 0x0027292F, draw-module slot 25
	void rva00272945(Int value);						// 0x00272945, draw-module slot 24
	void rva0027295B(void *value);						// 0x0027295B, draw-module slot 26
	Bool rva00272C1E();									// 0x00272C1E: any draw module's slot 55
	void rva00272DBB(Object *obj);						// 0x00272DBB: draw-module slot 61 for a kind-of-69 object
	void rva00273648(void *value);						// 0x00273648, draw-module slot 59
	void rva0027366A();									// 0x0027366A, draw-module slot 57
	void rva00273686(Bool value);						// 0x00273686, draw-module slot 58

protected:
	virtual Drawable *asDrawableMeth() { return this; }				// Thing slot 4
	virtual const Drawable *asDrawableMeth() const { return this; }			// Thing slot 3
	virtual void reactToTransformChange(const Matrix3D *oldMtx, const Coord3D *oldPos, Real oldAngle);	// Thing slot 5, 0x002742E5

public:
	Drawable(const ThingTemplate *thing, DrawableStatus statusBits, Int value);	// 0x002797BD
	virtual ~Drawable();										// slot 7, 0x002793F0
	virtual void rva0027299A(Int value);							// slot 8, 0x0027299A
	virtual void setDeathDecalPosition(Int value);					// slot 9, 0x002729B5
	virtual void rva002729CB(Real value);							// slot 10, 0x002729CB
	virtual void rva002729E9(Int value);							// slot 11, 0x002729E9
	virtual void createDeathDecal(Int a, Real b, Int c);				// slot 12, 0x00272971
	virtual void preloadAssets();								// slot 13, 0x00271095: each draw module's preloadAssets (Module slot 7)
	virtual void rva00271C38(Int value);							// slot 14, 0x00271C38

	// Snapshot, secondary table 0x00BFB0A4 (the map spells slots 1 and 3
	// LoadPostProcess and DoXfer, all public)
	virtual void loadPostProcess();								// 0x002717BC
	virtual const char *GetSnapshotName() const;					// 0x00279669
	virtual void xfer(Xfer *xfer);								// 0x0027A219

private:
	static Int s_modelLockCount;		// VA 0x00DFEB7C (map: ?s_modelLockCount@Drawable@@0HA)
	void onComingOutOfShroud();			// 0x0027893E

public:
	void *m_unk0x64;					// +0x64 (Zero Hour: the colour-tint envelope)
	class TintEnvelope *m_colorTintEnvelope;	// +0x68 (colorFlash 0x0027541E and colorTint 0x00275490 allocate it and call TintEnvelope::play)
	RGBColor m_unk0x6C;					// +0x6C (setFromInt in the ctor)
	Int m_unk0x78;						// +0x78
	Int m_unk0x7C;						// +0x7C
	Int m_unk0x80;						// +0x80
	Real m_unk0x84;						// +0x84
	Real m_unk0x88;						// +0x88
	Int m_unk0x8C;						// +0x8C
	Coord3D m_unk0x90;					// +0x90
	Coord3D m_unk0x9C;					// +0x9C
	TerrainDecalType m_terrainDecalType;	// +0xA8 (7 in the ctor; setTerrainDecal 0x00272870)
	Real m_unk0xAC;						// +0xAC
	Real m_unk0xb0;						// +0xB0 the disguise's opacity, set to 1.0f on exit
	Real m_unk0xB4[5];					// +0xB4
	Real m_unk0xC8;						// +0xC8
	Real m_unk0xCC;						// +0xCC
	Real m_unk0xD0;						// +0xD0
	Int m_unk0xD4;						// +0xD4 (10 in the ctor)
	Real m_unk0xD8;						// +0xD8
	Real m_unk0xDC;						// +0xDC
	Real m_unk0xE0;						// +0xE0
	Bool m_unk0xE4;						// +0xE4
	Int m_unk0xE8[2];					// +0xE8
	Int m_unk0xF0;						// +0xF0
	Real m_unk0xF4;						// +0xF4
	Real m_unk0xF8;						// +0xF8
	Object *m_object;					// +0xFC
	DrawableID m_id;					// +0x100
	Drawable *m_nextDrawable;			// +0x104
	Drawable *m_prevDrawable;			// +0x108
	OpaqueRefElement4 m_customSoundAmbientInfo;	// +0x10C (Zero Hour: the script-set ambient sound; clearCustomSoundAmbient 0x00279354 clears it, setCustomSoundAmbientInfo 0x00279797 assigns it through operator= 0x00239099)
	OpaqueRefElement4 m_ambientSoundInfo;	// +0x110 a second ref holder: stopAmbientSound 0x002743D7 clears it through 0x000A8C9B
	UnsignedInt m_status;				// +0x114 the ctor's DrawableStatus argument
	UnsignedInt m_tintStatus;			// +0x118 (zeroed by the ctor); PoisonedBehavior sets bit 4 in place, 0x00270619 clears
	Int m_unk0x11C[3];					// +0x11C (zeroed by the ctor)
	Int m_fadeMode;						// +0x128 fadeOut 2, fadeIn 1; 3, 4 and 5 from 0x00272D3D, 0x00272D03, 0x00272D77
	UnsignedInt m_fadeElapsed;			// +0x12C zeroed when a fade starts
	UnsignedInt m_fadeFrames;			// +0x130 the fade's length in frames
	UnsignedInt m_fadeHoldFrames;		// +0x134 (0x00272D77's second argument)
	Int m_unk0x138;						// +0x138
	DrawableLocoInfo *m_locoInfo;		// +0x13C
	Real m_unk0x140;					// +0x140
	UnsignedInt m_ambientSoundHandle;	// +0x144 audio handle; 1 (no sound) once removed (stopAmbientSound 0x002743D7)
	UnsignedInt m_unk0x148;				// +0x148 a second audio handle (0x002714CA removes it)
	// +0x14C the three null-terminated module lists: draw, client update, and a
	// third kind (0x00270234 walks all three in one counted loop, so they are one
	// array; findClientUpdateModule 0x0027449C walks +0x150, 0x002744C7 +0x154).
	Module **m_modules[3];				// +0x14C
	Module **getModuleList(Int i) const { return m_modules[i]; }
	DrawModule **drawModuleList() const { return (DrawModule **)m_modules[0]; }
	unsigned char m_unk0x158[0xC];		// +0x158 a vector (_Vector_base ctor)
	Int m_unk0x164;						// +0x164
	Int m_unk0x168;						// +0x168
	Int m_unk0x16C;						// +0x16C
	Matrix3D m_unk0x170;				// +0x170
	Matrix3D m_instance;				// +0x1A0
	Matrix3D m_unk0x1D0;				// +0x1D0
	Real m_instanceScale;				// +0x200 (the template's +0x4E0)
	Int m_unk0x204;						// +0x204 (-1 in the ctor)
	Matrix3D m_unk0x208;				// +0x208
	Coord3D m_unk0x238;					// +0x238
	Int m_unk0x244;						// +0x244 (-1 in the ctor)
	unsigned char m_unk0x248[0x10];		// +0x248 a record whose +4 the ctor points at this drawable
	BitFlags<591> m_conditionFlags;		// +0x258 model conditions (591 bits)
	BitFlags<591> m_unk0x2A4;			// +0x2A4
	BitFlags<591> m_unk0x2F0;			// +0x2F0
	Real m_unk0x33C;					// +0x33C
	Int m_unk0x340;						// +0x340
	DisplayString *m_captionDisplayString;	// +0x344 freed through TheDisplayStringManager slot 15 (clearCaptionText 0x00271496)
	AsciiString m_unk0x348;				// +0x348 (isEmpty/set: 0x00274C62)
	AsciiString m_unk0x34C;				// +0x34C (isEmpty/set/clear: 0x00274C88, 0x00274CB2)
	Int m_unk0x350;						// +0x350
	void *m_unk0x354;					// +0x354
	Real m_secondMaterialPassOpacity;	// +0x358
	Int m_unk0x35C;						// +0x35C
	Int m_unk0x360;						// +0x360
	void *m_unk0x364;					// +0x364
	Int m_unk0x368;						// +0x368
	unsigned char m_unk0x36C[0xC];		// +0x36C a vector (_Vector_base ctor)
	Int m_unk0x378;						// +0x378 (-1 in the ctor)
	Int m_unk0x37C;						// +0x37C
	Int m_unk0x380;						// +0x380
	UnsignedInt m_voiceAttackChargeFrame;	// +0x384 frame + the template's VoiceAttackChargeTimeout (startVoiceAttackChargeTimer 0x00271C1C)
	UnsignedInt m_evaSecondDamageFrame;	// +0x388 pushed out to frame + EvaEventSecondDamageFarFromFirstTimeoutMS (0x00271B03)
	unsigned char m_unk0x38C[0x18];		// +0x38C three 8-byte objects (eh vector ctor with a dtor)
	Int m_unk0x3A4;						// +0x3A4
	Bool m_unk0x3A8[4];					// +0x3A8
	DrawableInterpolationHistory m_interpolation;	// +0x3AC (0x90 bytes; ctor 0x002701F4)
	UnsignedByte m_selected;			// +0x43C
	Bool m_unk0x43D[15];				// +0x43D flags the ctor sets (+0x43F, +0x442, +0x443, +0x447..+0x44B to 1)
	Int m_44C;							// +0x44C (rva0027290C stores its non-zero argument)
	BuffManager *m_buffManager;			// +0x450 (turnOffBuff 0x00271BCC forwards to BuffManager::TurnOffBuff)
	Int m_indicatorColor;				// +0x454 Zero Hour's (setIndicatorColor 0x002741DE stores it, rva00270FAC passes it on)
	Int m_unk0x458;						// +0x458
	Int m_unk0x45C;						// +0x45C
	unsigned char m_unk0x460[0x10];		// +0x460
};
// SIZE 0x470
typedef char DrawableSize[(sizeof(Drawable) == 0x470) ? 1 : -1];
typedef char DrawableObjectOffset[(offsetof(Drawable, m_object) == 0xFC) ? 1 : -1];
typedef char DrawableStatusOffset[(offsetof(Drawable, m_status) == 0x114) ? 1 : -1];
typedef char DrawableModulesOffset[(offsetof(Drawable, m_modules) == 0x14C) ? 1 : -1];
typedef char DrawableInstanceOffset[(offsetof(Drawable, m_instance) == 0x1A0) ? 1 : -1];
typedef char DrawableConditionOffset[(offsetof(Drawable, m_conditionFlags) == 0x258) ? 1 : -1];
typedef char DrawableOpacityOffset[(offsetof(Drawable, m_secondMaterialPassOpacity) == 0x358) ? 1 : -1];
typedef char DrawableSelectedOffset[(offsetof(Drawable, m_selected) == 0x43C) ? 1 : -1];

#endif
