// Object.h: SAGE game object. Partial layout. SIZE 0x4D8.
// Target facts (retail game.dat; functions by RVA, tables by VA):
//  - Object::Object (0x00298EA9, ret 0x14: four arguments and the most-derived
//    flag) stores the vbtable 0x00BFC320 at +0x68 and ShroudClient's table at
//    +0x4D4 when most-derived, constructs Thing at +0, stores Snapshot's table
//    0x00BBB554 at +0x60, calls ShroudObjectClient's constructor 0x0028AADF on
//    +0x64 (flag 0), stores PartitionClient's 0x00BFBC9C at +0x6C and
//    CollisionClient's 0x00BFBCBC at +0x70, then its own tables: 0x00BFC300 +0,
//    0x00BFC2F0 +0x60, 0x00BFC2D8 +0x64, 0x00BFC2B8 +0x6C, 0x00BFC290 +0x70,
//    0x00BFC27C (vtordisp thunks) for the virtual base, and the vtordisp at
//    +0x4D0. Members run +0x74..+0x4CC.
//  - The three interface bases are the libraries' clients (WorldBuilder paths
//    Code/Libraries/Source/{shroudmanager,partitionmanager,collisionmanager}):
//    ~Object unregisters +0x64 from TheShroudManager (0x00DFE74C), and
//    0x0028BAC0 hands +0x6C to ThePartitionManager (0x00DFE748) and +0x70 to
//    TheCollisionManager (0x00DFE754), each when its data pointer (+0x4C4,
//    +0x4C8, +0x4CC) is set. GameLogic::init (0x00243F25..0x00243FBE) creates
//    the three and names them. Their class names are provisional; see each
//    header.
//  - sizeof 0x4D8: `push 0x4D8; call operator new` at 0x0023CAF2.
// Field names follow the constructor/destructor (Zero Hour names where the
// statement matched; offset names otherwise).
#pragma once
#ifndef BFME2_OBJECT_H
#define BFME2_OBJECT_H

#include <vector>
#include <list>
#include "Common/GameType.h"
#include "Common/Thing.h"
#include "Common/Snapshot.h"
#include "shroudmanager/ShroudObjectClient.h"
#include "partitionmanager/PartitionClient.h"
#include "collisionmanager/CollisionClient.h"
#include "collisionmanager/CollisionManager.h"
#include "GameLogic/PartitionManager.h"
#include "Common/AsciiString.h"
#include "Common/ModelConditionFlags.h"
#include "Common/Geometry.h"
#include "Common/BitFlags11.h"
#include "Common/BitFlags.h"
#include "Common/UpgradeMaskType.h"
#include "GameLogic/WeaponSet.h"
#include "GameLogic/Module/ContainModule.h"
#include "GameLogic/Damage.h"

class AIGroup;
struct CreateMask;
class Xfer;
class UpgradeTemplate;
class ObjectGuardingHelper;
class SpecialPowerModuleInterface;
enum SpecialPowerType;
enum ObjectScriptStatusBit;
class ObjectStatusMask;
enum RadarPriorityType;
class WallUpgradeUpdate;
class Rva00373EC6;
class StealthUpdate;
// The map's placeholder name for the 16-byte status mask the status setters take (rva0028CDEB's parameter);
// the class is BitFlags<101> (ObjectStatusMaskType), renamed here once the map spells it so. Only named in a
// signature, so only declared; GameLogic/Module/RebuildHoleStatusMask.h holds the layout RebuildHoleBehavior builds one with.
class Rva00346BC0;
class AIUpdateInterface;
class Locomotor;
class BehaviorModule;
class BodyModuleInterface;
class ContainModuleInterface;
class DamageModule;
class Drawable;
class ObjectDefectionHelper;
class EmotionTrackerUpdate;
class PathfinderPosGoalManager;
class ExperienceTracker;
class FiringTracker;
class Coord2D;
class ProductionUpdateInterface;
class Module;
// The shroud manager's per-object data (WorldBuilder's ShroudManager::Data;
// the map spells it PartitionData). Only the call Object makes is declared.
enum ObjectShroudStatus;
enum CellShroudStatus;
class PartitionData
{
public:
	void makeDirty();						// 0x0073A0F0
	void friend_updateCellsTouched();		// 0x0073B250 (Object::updateShroudNow 0x0028C11A)
	ObjectShroudStatus getShroudedStatus(Int playerIndex);	// 0x0073A6D0
};
class ObjectSMCHelper;
class FoundationAIUpdateInterface;
class BloodthirstyUpdateInterface;
class Player;
class CommandButton;
class PolygonTrigger;
class Team;
class UpdateModule;
class Weapon;
class AttributeModifierPoolUpdate;
class SpecialAbilityUpdate;
class SpecialPowerCompletionDie;
class SpecialPowerTemplate;
class DockUpdateInterface;
class ExitInterface;
class SpawnBehaviorInterface;
enum WeaponLockType;
enum PathfindLayerEnum;
enum CanAttackResult;
enum AbleToAttackType;
enum WeaponChoiceCriteria;
enum DamageType;
enum DeathType;
enum DisabledType;
enum ObjectScriptStatusBit;
enum KindOfType;
enum WeaponSetType;
enum ArmorSetType;
enum ObjectStatusTypes;
enum WeaponSlotType;

class Object : public Thing, public Snapshot, public ShroudObjectClient,
	public PartitionClient, public CollisionClient
{
 friend class WallUpgradeUpdate;
 friend class SpecialPowerModule;	// SpecialPowerModule::ApplyPowerToObject 0x00493EA7 calls the private findAttributeModifierPoolUpdate 0x0028BDD7
 friend class GameLogic;	// Object::setID 0x0028B532 is protected (IAE); GameLogic::SwapObjectIDs 0x00242BB4 calls it on both objects
public:
	Object(const ThingTemplate *thingTemplate, const CreateMask *mask, Team *team, ObjectID id);	// 0x00298EA9

	// Snapshot (table 0x00BFC2F0 at +0x60).
	virtual void loadPostProcess();											// 0x00293EFF
	virtual const char *GetSnapshotName() const;							// 0x00299C6E

	// ShroudClient, the virtual base (table 0x00BFC27C, vtordisp thunks).
	virtual const GeometryInfo *ShroudGetGeometryInfo() const;				// 0x00299C99
	virtual const Coord3D *ShroudGetPosition() const;						// 0x00299CCE
	virtual Real ShroudGetOrientation() const;								// 0x00299CDD
	virtual void ShroudSetData(PartitionData *data);						// 0x00299CA8
	virtual PartitionData *ShroudGetData() const;							// 0x00299CBA

	// ShroudObjectClient (table 0x00BFC2D8 at +0x64).
	virtual AsciiString ShroudGetName();									// 0x00290EE0
	virtual void vslot01();													// 0x000B3FD0 (folded empty body)
	virtual ShroudGhostClient *ShroudCreateGhost();							// 0x00290EFE
	virtual void ShroudGetLookerRanges(Int *playerMask, Real *frontRange,
		Real *sideRange, Real *rearRange);									// 0x002941EA
	virtual Real ShroudGetValueRangeAndMask(Int valueIndex, UnsignedInt *range,
		UnsignedInt *mask) const;											// 0x0028E5E0
	virtual Bool ShroudHideIfFogged(Int playerIndex) const;					// 0x0028E775

	// PartitionClient (table 0x00BFC2B8 at +0x6C).
	virtual const GeometryInfo *PartitionGetGeometryInfo() const;			// 0x005FBE57
	virtual const Coord3D *PartitionGetPosition() const;					// 0x00299CBE
	virtual Real PartitionGetOrientation() const;							// 0x00299CC2
	virtual Object *PartitionGetObject();									// 0x00299C74
	virtual const Object *PartitionGetObject() const;						// 0x00299C74
	virtual void PartitionSetData(void *data);								// 0x00299C7C
	virtual void *PartitionGetData() const;									// 0x00042272
	virtual Int PartitionGetPlayerIndex() const;							// 0x0028E7F8

	// CollisionClient (table 0x00BFC290 at +0x70).
	virtual Coord3D CollisionGetBoundsMin();								// 0x0028C037
	virtual Coord3D CollisionGetBoundsMax();								// 0x0028C08D
	virtual Object *CollisionGetObject();									// 0x00299C78
	virtual const Object *CollisionGetObject() const;						// 0x00299C78
	virtual ObjectID CollisionGetID() const;								// 0x005801F2
	virtual UnsignedInt CollisionGetGroup() const;							// 0x0028E84A
	virtual void CollisionSetData(void *data);								// 0x00299C7C
	virtual void *CollisionGetData() const;									// 0x00042272
	virtual Bool CollisionShouldCollide(const Object *other, UnsignedInt a,
		UnsignedInt b) const;												// 0x0028C0E3
	virtual void CollisionOnCollide(Object *other, const Coord3D *loc,
		const Coord3D *normal);												// 0x0028E868

    int rva0028B511() const;
    Bool rva0028B35B(const Object *other) const;	// 0x0028B35B (CollisionShouldCollide 0x0028C0E3 forwards to it)
    void *rva0028BD5D(int) const;
    void setLayer(PathfindLayerEnum);
    void setDestinationLayer(PathfindLayerEnum layer);	// 0x0028B525 (Zero Hour's name: a plain store to +0x40C; AIUpdateInterface::loadPostProcess 0x002686A4 restores it after re-registering the goal)
    bool rva0028F518();
    Rva00373EC6 *rva0028F4BC();
    void rva0028EC68(int,void *,int);
    BehaviorModule *rva0028F2C4(int key) const;	// 0x0028F2C4 (Object.cpp); ActivateModuleSpecialPower::SetWakeState looks up a module by key
    void rva0028C2DD(Coord3D *) const;
    void rva0029660C(const Coord3D *,int);
    void rva0028DA28();
    void rva0028D680();	// 0x0028D680: refreshes the drawable against the controlling player's relationships
    void onReducedToRubble();	// 0x00298E6A: what the structure-rubble helper 0x004BDA67 tail-jumps to
    void rva0028DAB9();	// 0x0028DAB9 (ActiveBody structure-rubble helper 0x004BDA67, template CommandPointBonus > 0)
    const AsciiString *rva00290E67() const;		// 0x00290E67 (?rva00290E67@Object@@QBEPBVAsciiString@@XZ: the object's command-set name; CommandButtonHuntUpdate::xfer 0x004953EE)
    void rva0028CDEB(const Rva00346BC0 &,bool);		// 0x0028CDEB: the mangled name's mask type is Rva00346BC0 (was spelled ObjectStatusMask here, which mangles differently)
    void clearMask(const Rva00346BC0 &mask) { rva0028CDEB(mask, false); }	// inline in retail; the out-of-line copy is 0x0044E5AA
    void fireCurrentWeapon(Object *,int);
	SpecialPowerModuleInterface *getSpecialPowerModule(const SpecialPowerTemplate *spt) const;	// 0x0028BB9E
	SpecialAbilityUpdate *findSpecialAbilityUpdate(SpecialPowerType type) const;	// 0x0028BD92
	Bool hasSpecialPower(SpecialPowerType type) const;	// 0x0028D8EB (Zero Hour's name): the bit test 0x001E4426 on m_specialPowerBits (+0x3A4); ScriptConditions 0x003E73E7, 0x003E9062 call it
	SpecialPowerCompletionDie *findSpecialPowerCompletionDie() const;	// 0x0028BE2B
    SpecialPowerModuleInterface *findSpecialPowerModuleInterface(SpecialPowerType) const;
    Real GetRelativeAngle(const Coord3D *) const;
    void rva0028D412(const Matrix3D *);
    Real rva002C97E8(const Coord3D *, const Coord3D *) const;
	// Weapon.cpp rows over 0x002C97E8 / 0x002636F6: from this object's position to a point
	// (0x002C9846), and from a point to another object (0x002C9856).
	Real rva002C9846(const Coord3D *pos) const;
	Bool getWorldspaceBestContactPoint(Coord3D *result, const Coord3D *from, const char *boneName, Int a, Int b, Bool c) const;	// 0x002904DC (SpecialAbilityUpdate::isWithinStartAbilityRange 0x0044F28E)
	Real Get3DCenterDistanceSquared(const Coord3D *point) const;	// 0x001E435D (map spells the parameter with the provisional class BfmeVec3EJ)
	Coord3D rva00265514(const Object *other) const;	// 0x00265514: the 2D border vector from other to this object (Weapon::computeApproachTarget 0x002CAEA3)
	Coord3D Get2DBorderVectorTo(const Coord3D *pos) const;	// 0x002C986B (Weapon::computeApproachTarget 0x002CAEA3)
	Real rva002C9856(const Coord3D *pos, const Object *other) const;
	Real rva002636F6(const Coord3D *pos, const void *other, const Coord3D *otherPos) const;	// 0x002636F6
	// 0x002615E3 (map placeholder Rva000CBA20::distSq): squared distance from this object to a point;
	// makeAssistanceRequest 0x002C9D47 tests the asker's position against RequestAssistRange with it.
	Real rva002615E3(const Coord3D *pos) const;
	// 0x0028BCB4: walks the behavior module list (+0x244) and returns the first
	// non-null answer of each module's slot-22 query (+0x58); null if none.
	DockUpdateInterface *getDockUpdateInterface();
	// 0x00263763 (map placeholder name): passes this+0x38, other+0x38 and other
	// to the helper 0x002636F6 and returns its Real.
	Real rva00263763(const void *other) const;
	Real getDistanceSquaredToObject(const Object *other) const;	// 0x0044E6B8 (this+0x38, other and other+0x38 go to the geometry-aware distance 0x0028ED19; SpecialAbilityUpdate::isWithinStartAbilityRange 0x0044F28E)
	Object *getCurrentVictim() const;	// 0x0028ACA0: m_ai's current victim (AIUpdateInterface::getCurrentVictim 0x00268D71) when m_ai slot 111 (+0x1BC) answers true, else NULL; map row ?rva0028ACA0@Object@@QBEPAV1@XZ (?:placeholder)
	// 0x0028DCC4 (map placeholder name): registers the object with the
	// partition and collision managers (the counterpart of 0x0028BAC0).
	void rva0028DCC4();
	// 0x00290758 (map placeholder name): WorkerAIUpdate::loseOneBox 0x004A98AC and gainOneBox 0x004A9C3F
	// pass it the module's MaxBoxes and the new box count.
	void rva00290758(Int maxBoxes, Int boxes);
	// 0x0028BAC0 (map placeholder name): hands the object's partition and collision handles back
	// (the counterpart of 0x0028DCC4); DozerAIUpdate::makePhantomStructureInert 0x00489DE3 calls it
	// when +0x454 is set.
	void rva0028BAC0();
	ObjectID getID() const { return m_id; }
	ObjectID getProducerID() const { return m_producerID; }
	Team *getTeam() { return m_team; }
	// The team member list link test (0x0028A6C7): true when this object is the
	// head or has either link set; Team::loadPostProcess 0x003A2339 calls it.
	bool dlink_isInList_TeamMemberList(Object *const *listHead) const;
	void dlink_prependTo_TeamMemberList(Object **listHead);	// 0x0028A6E7
	void dlink_removeFrom_TeamMemberList(Object **listHead);	// 0x0028A704
	void onSelectedByPlayer(Player *player);	// 0x0029041B
	void onDeselectedByPlayer(Player *player);	// 0x00290496
	const Team *getTeam() const { return m_team; }
	Player *getControllingPlayer() const;
	// 0x0028B749: credits damage this object dealt to its controlling player's skill points
	// (Player::addSkillPointsForDamage 0x002AA5E1); ActiveBody::attemptDamage 0x004BFE07 calls it on the damager.
	void addSkillPointsForDamage(const Object *victim, Real damage);
	const WeaponSetFlags &getWeaponSetFlags() const;	// 0x0028B7AE `lea eax,[ecx+0x370]; ret` (map: placeholder ?get@Rva0028B7AELeaGetter)
	void setTeam(Team *team);				// 0x00298AE4 (TaintSpecialPower::addObject 0x004C4940 calls it)
	void friend_bindToDrawable(Drawable *draw);				// 0x0029080C (GameLogic::bindObjectAndDrawable 0x0023CD4A, sendObjectDestroyed 0x0023CD67)
	Relationship getRelationship(const Object *that) const;
	Bool hasUpgrade(const UpgradeTemplate *) const;
	void removeUpgrade(const UpgradeTemplate *upgrade);	// 0x00290D42 (UpgradeDie::onDie 0x00486C00 calls it)
	void friend_adjustPowerForPlayer(Bool incoming);	// 0x0028D99A (Player::becomingTeamMember 0x002AD4EC calls it; Zero Hour's name)
	void topple(const Coord3D *toppleDirection, Real toppleSpeed, UnsignedInt options);	// 0x0028CF37 (Zero Hour's)
	Int getLastVictimID() const;	// 0x0028AF86 (the map spells the result int: ?getLastVictimID@Object@@QBEHXZ)
	void onCapture(Player *oldOwner, Player *newOwner);	// 0x002966A0 (Zero Hour's)
	void updateUpgradeModules();	// 0x00292EEA (Player::onUpgradeCompleted 0x002AD93A calls it for every member; Zero Hour's name)
	void giveUpgrade(const UpgradeTemplate *upgrade);	// 0x00293077 (GrantUpgradeCreate::onBuildComplete 0x004B9093 calls it)
	Bool affectedByUpgrade(const UpgradeTemplate *upgrade) const;	// 0x002940B9 (Team::rva0039E8FF 0x0039E8FF calls it on every member without a horde interface; Zero Hour's name)
	Bool isProducedByHorde();	// 0x002931BA (role name; the map row ?rva002931BA@Object@@QAE_NXZ is a placeholder on Object: the object was spawned by a horde, Team::getTeamAsAIGroup 0x003A0F62 skips such members)
	void setPathfinderGoal(Int a, Int b);	// 0x0028ACEE (role name; the map row ?rva0028ACEE@Object@@QAEXHH@Z is a placeholder on Object: forwards to the PathfinderPosGoalManager at +0xA4, which registers a goal through SetGoal 0x004DE109; GiantBird state onExit bodies call it)
	void rva0028BBE1(void *guarded);	// 0x0028BBE1 (18-byte forwarder to ObjectGuardingHelper::setGuardedObject 0x004DF3B0)
	void rva0028BBF3(Object *guarded);	// 0x0028BBF3 (18-byte forwarder to ObjectGuardingHelper::stopGuarding 0x004DF2FC)
	void addGuarder(Object *guarder);	// 0x0028BC05 (18-byte forwarder to ObjectGuardingHelper 0x004DF4B5; ObjectGuardingHelper::setGuardedObject 0x004DF3B0 calls it)
	void removeGuarder(Object *guarder);	// 0x0028BC17 (forwarder to ObjectGuardingHelper 0x004DF3F8; stopGuarding 0x004DF2FC calls it)
	CellShroudStatus getShroudStatusForPlayer(Int playerIndex) const;	// 0x0028D2A2
	Bool isStructure() const;					// 0x0028D481
	Bool isFactionStructure() const;			// 0x0028D491
	Int isNonFactionStructure() const;			// 0x0028D4C4 (the map spells the return Int)
	Int rva0028F4EF();
	Int rva002933CD();
	Bool isEffectivelyDead() const { return (m_privateStatus & EFFECTIVELY_DEAD) != 0; }
	void setEffectivelyDead(Bool dead);						// 0x0028D2FB
	void onDie(DamageInfo *damageInfo);						// 0x00698517
	void onDestroy();										// 0x0028FC1D (s05: GameLogic::destroyObject 0x00242C09 calls it last)
	void rva00295F05(Bool flag);							// 0x00295F05 (s05: GameLogic::rva00240EBC 0x00240EBC calls it with true on each +0x164 entry)
    Bool isNeutralControlled() const; // 28B091; CrateCollide::isValidToExecute4BC6C2
	Bool didEnter(PolygonTrigger *trigger);		// 0x0028D718
	Bool didExit(PolygonTrigger *trigger);		// 0x0028D757
	Real rva002972B1(Real range);			// 0x002972B1 the threat a unit poses within range
	// 0x0006F039 (COMDAT in W3DScene.cpp, 17 link referrers): the masked one-bit test of the model
	// conditions (`and eax,[esi+edx*4+0x10C]; neg; sbb`). Out of line in retail; the map's name
	// is a rename of the ledger's isKindOf spelling.
	Bool testModelConditionState(ModelConditionFlagType t) const;
	// 0x0028E58E: Zero Hour's getRadarPriority: the template's RadarPriority (+0x5F0, a signed
	// byte), else STRUCTURE for a garrisonable contain or KindOf bit 49 (RadarObject 0x00050125).
	RadarPriorityType getRadarPriority() const;
	Bool isSelectable() const;												// 0x0028D7FD
	Bool isMassSelectable() const;											// 0x00292FAC
	// Walkers over the behavior modules (+0x244) through BehaviorModuleInterface slots.
	void rva0028AB4E() const;							// 0x0028AB4E: each create interface's slot 2
	void rva0028B72A() const;							// 0x0028B72A: each behavior's slot 44
	ProductionUpdateInterface *getProductionUpdateInterface();	// 0x0028BC94 (map: placeholder): the first slot-28 answer
	// 0x0028BC58 optionally checks the answer through ProductionUpdateInterface slot 30.
	ProductionUpdateInterface *getProductionUpdateInterface(Bool checkProductionState);
	FoundationAIUpdateInterface *rva0028BCF4() const;							// 0x0028BCF4: the first slot-38 answer
	void *rva0028BD17() const;							// 0x0028BD17: the first slot-32 answer
	void *rva0028BD3A() const;							// 0x0028BD3A: the first slot-33 answer
	Bool isSlaved() const;								// 0x0028C4ED (map: placeholder): any slaved-update interface
	void setWeaponSetFlag(WeaponSetType);
	// (WeaponSetSpecialAbilityUpdate 0x0049251D, HeroModeSpecialAbilityUpdate 0x00492231/0x0049226C, TeleportSpecialAbilityUpdate 0x00492DAB):
	void clearWeaponSetFlag(WeaponSetType);	// 0x00290A10
	Bool hasWeaponTemplateSetFor(WeaponSetType flag) const;	// 0x0029091E: builds a one-bit WeaponSetFlags and returns findWeaponTemplateSet(mask) != 0
	void setBusyUntilFrame(Int frame);	// 0x002900E0: setStatus(0x4A, true), stores the frame at +0x42C
	void setArmorSetFlag(ArmorSetType);			// 0x0028B78A: forwards to the body (slot 12)
	void makeDirty();							// 0x0028BB44: the shroud data at +0x4C4
	// Map placeholder names; bodies in Object.cpp say what they touch.
	void rva0028AE9B(Int value);				// 0x0028AE9B: the drawable's +0x360
	void setReceivingDifficultyBonus(Bool receive);	// 0x0028B238: +0x43C, then the controller
	Bool isInside(PolygonTrigger *trigger);	// 0x0028B411
	void updateTriggerAreaFlags();	// 0x0028B2BF
	void friend_notifyOfNewMapBoundary();	// 0x0028B5EE
	ExitInterface *getObjectExitInterface() const;	// 0x0028B445: behaviors (slot 20), then the contain (slot 29)
protected:
	Bool didEnterOrExit() const;				// 0x0028D6EB (IBE)
public:
	// The doubly linked object list through +0x8C (next) and +0x90 (prev).
	void appendToList(Object **listHead, Object **listTail);	// 0x0028B49B
	void removeFromList(Object **listHead, Object **listTail);	// 0x0028B595
protected:
	void setID(ObjectID id);					// 0x0028B532 (IAE): re-keys the GameLogic lookup table
public:
	void rva0028B95F();							// 0x0028B95F: sets +0x48C and stamps the frame at +0x490
	void rva0028BA85() const;					// 0x0028BA85: hands +0x45C to every behavior (slot 46)
	void rva0028BAAE(Int value);				// 0x0028BAAE: stores +0x45C, then rva0028BA85
	void friend_setUndetectedDefector(Bool set);	// 0x0028AC34: bit 2 of +0x438
	// Pathfinding-goal forwarders (each tests m_pathfinderPosGoalManager, +0xA4).
	void rva0028ACCA(Int a);						// 0x0028ACCA
	void rva0028ACDC(Int a);						// 0x0028ACDC
	Weapon *findWaypointFollowingCapableWeapon();			// 0x0028AEEA (thunk to WeaponSet::findWaypointFollowingCapableWeapon 0x002C741C; ScriptActions::doNamedFireWeaponFollowingWaypointPath 0x003C9206)
	void rva0028ACEE(Int a, Int b);					// 0x0028ACEE
	void rva0028AD00(Int a, Real angle, Int b);		// 0x0028AD00 (SetGoal)
	void rva0028AD22();								// 0x0028AD22
	void rva0028AD32();								// 0x0028AD32
	Bool GetGoalPosition(Coord3D *pos) const;		// 0x0028AD42
	Real GetGoalAngle() const;						// 0x0028AD56
	Int GetGoalLayer() const;						// 0x0028AD6C (1 without a goal record)
	void rva0028AD7C();								// 0x0028AD7C
	Bool GetPathfinderPos(Coord3D *pos) const;		// 0x0028AD9C
	Bool IsAtGoalPosition() const;					// 0x0028ADB0
	void restartAllSpecialPowerRecharges() const;						// 0x0028B265: every special power restarts its recharge (1.0f)
	Bool getIsUndetectedDefector() const { return (m_privateStatus & UNDETECTED_DEFECTOR) != 0; }	// read inline
	Bool isOffMap() const { return (m_privateStatus & OFF_MAP) != 0; }	// read inline (AIUpdateInterface::privateAttackPosition 0x0026DB1A)
	Bool isInList(Object **listHead) const;		// 0x0028B47C
	SpawnBehaviorInterface *getSpawnBehaviorInterface() const;	// 0x0028BCD4
	void leaveGroup();							// 0x0028C01F
	void enterGroup(AIGroup *group);			// 0x0028E5CA
	Bool hasUpgradeMask(Int maskIndex) const;	// 0x0028D9E5
	void setTemporaryTeam(Team *team);			// 0x00298480
private:
	void setOrRestoreTeam(Team *team, Bool restoring);	// 0x00297379 (AAE)
public:
	void setCaptured(Bool captured);			// 0x0028B21E: bit 4 of +0x438
	void maskObject(Bool mask);					// 0x0028B7E2 (?maskObject@Object@@QAEX_N@Z)
	void setSelectable(Bool selectable);		// 0x0028B76D: +0x434, then the drawable
	void setCustomIndicatorColor(Int color);	// 0x0028AFFC: +0x30C, then the drawable
	void removeCustomIndicatorColor();			// 0x0028B01E
	Int getIndicatorColor() const;				// 0x0028B026: custom, else the controller's colour
	Int getNightIndicatorColor() const;			// 0x0028B050: the night colour (Drawable::changedTeam 0x002742AC)
	void updateIndicatorColor();				// 0x0028D253 (placeholder row): the drawable takes the night or day indicator colour by TheGlobalData's time of day
	void attemptHealing(Real amount, const Object *source);	// 0x0028FE55
	Bool attemptHealingFromSoleBenefactor(Real amount, const Object *source, UnsignedInt delay);	// 0x0028FEA7
	void healCompletely();						// 0x0028FF9E
	void clearArmorSetFlag(ArmorSetType);		// 0x0028B79C: forwards to the body (slot 13)
	void setProducer(Object *producer);			// 0x0028AFD2: +0x78 = producer's ID or 0
	void setBuilder(const Object *builder);		// 0x0028AFE7: +0x7C = builder's ID or 0
	void reloadAllAmmo(Bool now);				// 0x0028ADC2: the weapon set at +0x330
	Bool hasAnyWeapon() const;					// 0x0028ADE0
	Bool testStatus(ObjectStatusTypes bit) const;
	Bool testBit(Int bit) const;	// 0x0028D891 (map ?testBit@Object@@QBE_NH@Z; ScriptConditions::evaluateUnitHasToggledWeapon 0x003E89A9 calls it with 0x18, 0x19, 0x1A)
	void setStatus(ObjectStatusTypes bit, Bool set);
	void setScriptStatus(ObjectScriptStatusBit bit, Bool set);	// 0x00292969 (ScriptActions 0x003C058A, 0x003C05DE, 0x003BBD39, 0x003BBD61)
	void clearScriptStatus(ObjectScriptStatusBit bit);	// 0x002934D9: Zero Hour's setScriptStatus(bit, false), out of line in retail
	Bool canUnloadOnLandAt(Coord3D *pos);	// 0x0028ECDB: an amphibious ship transport with a contain module that can unload at a land point (AIUpdateInterface::privateFollowWaypointPath 0x0026B8E5, privateFollowPath 0x0026BA84)
	void adjustModelConditionForWeaponStatus();	// 0x0028DB3C (ObjectWeaponStatusHelper::update 0x0028F976, AIUpdateInterface::privateIdle 0x0026D5FB; Zero Hour's name)
	void report(Object *victim, Int kind);	// 0x00294D61 (ActiveBody::attemptDamage 0x004BFE07 reports a kill on the damage source)
	void addSkillPointsForDamage(Object *victim, Real ratio);	// 0x0028B749 (forwards to the controlling player)
	void notifyDamageDealt(DamageInfo *damageInfo, Object *victim) const;	// 0x0028B1D4
	void clearStatus(ObjectStatusTypes bit);
	Coord3D *getPlanarDirectionTo(Coord3D *direction, const Object *other) const;	// ?getPlanarDirectionTo@Object@@QBEPAUCoord3D@@PAU2@PBV1@@Z
	// Inline: the out-of-line copy 0x0023D394 (`add ecx,0x1C8; jmp
	// BitFlags<11>::any 0x0023C58B`) has no callers, and SpecialPowerModule's
	// doSpecialPower entries expand it in place.
	Bool isDisabled() const { return m_disabledMask.any(); }
	// Zero Hour's isDisabledByType, through BitFlags<11>::testMask: a constant type compiles to one
	// `test byte [obj+0x1C8],imm` (ScriptConditions 0x003E6AA9, DetachableRiderBody 0x004C1D49).
	Bool isDisabledByType(DisabledType type) const { return m_disabledMask.testMask(1u << type); }
	// 0x0028E0F9 (Zero Hour's, without the angle): not when disabled; the store's check
	// unless forced; then rva0028C4B6 and the module's doSpecialPowerAtLocation (slot 12).
	void doSpecialPowerAtLocation(const SpecialPowerTemplate *spt, const Coord3D *loc, UnsignedInt commandOptions, Bool forceUsable);
	void doSpecialPower(const SpecialPowerTemplate *spt, UnsignedInt commandOptions, Bool forceUsable);
	void doSpecialPowerAtObject(const SpecialPowerTemplate *spt, Object *target, UnsignedInt commandOptions, Bool forceUsable);
	void rva0028C4B6();								// 0x0028C4B6
	// 0x0028B204: INVALID_ID once TheGameLogic's frame passes the expiration.
	ObjectID getSoleHealingBenefactor() const;
	// 0x0028CCB9; SpecialPowerModule::doSpecialPowerAtObject calls it as
	// target = target->adjustVictim(owner, true, 0) when the owner has status bit 26.
	Object *adjustVictim(Object *owner, Bool flag, Int arg);
	const BitFlags<11> &getDisabledFlags() const { return m_disabledMask; }
    void setDisabled(DisabledType);
    bool clearDisabled(DisabledType);
    bool setWeaponLock(WeaponSlotType, WeaponLockType);
    void releaseWeaponLock(WeaponLockType);
	// 0x0028AF65: the weapon set's current lock status is non-zero (Weapon::isAmmoReady
	// 0x002CC3FF calls it).
	Bool isCurWeaponLocked() const;
	Bool hasWeaponLockStatus() const;							// 0x0028B7B5 (role name: m_weaponSet +0x24 is nonzero; AIAttackState::loadPostProcess 0x00346063 calls it)
	Bool chooseBestWeaponForTarget(const Object *victim, WeaponChoiceCriteria criteria, CommandSourceType commandSource);	// 0x0028AF4A
	Real getLargestWeaponRange() const;	// 0x0028AE13
	void calcNaturalRallyPoint(Coord2D *pt);	// 0x0028B65F (Zero Hour's)
	void reactToTurretChange(WhichTurretType turret, Real oldRotation, Real oldPitch);	// 0x0028B141 (Zero Hour's; TurretAI 0x004D8B14)
	// +0x380: Weapon::computeBonus 0x002C9A6F ORs it into the bonus conditions.
	UnsignedInt getWeaponBonusCondition() const { return m_weaponBonusCondition; }
	Bool testWeaponBonusCondition(Int bit) const { return ((m_weaponBonusCondition >> bit) & 1) != 0; }	// retail tests the shifted form (PropagandaTowerBehavior 0x00481A9E)
	void setWeaponBonusCondition(Int bit) { m_weaponBonusCondition |= (1u << bit); }
	void clearWeaponBonusCondition(Int bit) { m_weaponBonusCondition &= ~(1u << bit); }
    bool rva00298893();
    // 0x001E42F2 and 0x001E431E pass the complete 19-word mask to 0x0028CFB2.
    void clearModelConditionFlags(const ModelConditionFlags &flags);
    void setModelConditionFlags(const ModelConditionFlags &flags);
    HordeContainInterface *getHordeContainInterface() { ContainModuleInterface *contain=m_contain; return contain ? contain->getHordeContainInterface() : 0; }
	Bool isUsingAirborneLocomotor() const;
	void initObject();									// 0x002934E7 (ThingFactory::newObject runs it last)
	Real getMajorRadius() const { return m_geometryInfo.getMajorRadius(); }	// 0xB8
	Real getConstructionPercent() const { return m_constructionPercent; }	// +0x280; ControlBar::updateConstructionTextDisplay 0x0053E3E6 reads it through the accessor (a float return is moved in xmm0)
	GeometryInfo *getGeometryInfo() { return &m_geometryInfo; }		// 0x0A8
	const GeometryInfo *getGeometryInfo() const { return &m_geometryInfo; }
	// 0x00290EAB: the index-th additional geometry, or the object's own when the index is out
	// of range (Drawable::getGeometryInfo(Int) 0x0027103F forwards to it).
	const GeometryInfo *getAdditionalGeometryInfo(Int index) const;
	Bool getSingleLogicalBonePosition(const char *boneName, Coord3D *pos, Matrix3D *transform) const;	// 0x0028BEE0
	Bool getSingleLogicalBonePositionOnTurret(WhichTurretType turret, const char *boneName, Coord3D *pos, Matrix3D *transform) const;	// 0x0028E1AC
	Int getMultiLogicalBonePosition(const char *boneNamePrefix, Int maxBones, Coord3D *positions, Matrix3D *transforms, Bool worldSpace, Int turretIndex) const;	// 0x0028BF81 (ActiveBody::createParticleSystems 0x004BE24A calls it at 0x004BE2B1)

	// Model conditions at +0x10C..+0x158. Zero Hour's Object runs the same clear/set
	// sequence and notifies through 0x0028AE6D only when the clear bit is set
	// or the set bit is not.
    const ModelConditionFlags &getModelConditionFlags() const { return m_modelConditions; }
    // evidence: expanded at its callers (SupplyCenterDockUpdate 0x004A1119, FlammableUpdate 0x0048C8F9); no out-of-line copy in retail
    // Ordinary inlining of the five model-condition accessors loses 29 rows across 20 translation units.
    __forceinline void clearModelConditionState(ModelConditionFlagType bit) {
     if(m_modelConditions.test(bit)) { m_modelConditions.clear(bit); rva0028AE6D(); }
    }
    // Retail expands this method at its callers and has no out-of-line copy.
    // Ordinary inlining of the five model-condition accessors loses 29 rows across 20 translation units.
    __forceinline void setModelConditionState(ModelConditionFlagType bit) {
     if(!m_modelConditions.test(bit)) { m_modelConditions.set(bit); rva0028AE6D(); }
    }
	__forceinline UnsignedInt testConditionBit(UnsignedInt bit) const
	{
		return m_modelConditions.test(bit);
	}
	// Retail expands this method at its callers and also emits clearAndSetModelConditionState at 0x001E49E5.
	// Ordinary inlining of the five model-condition accessors loses 29 rows across 20 translation units.
	__forceinline void clearAndSetCondition(UnsignedInt clearBit, UnsignedInt setBit)
	{
		if (m_modelConditions.test(clearBit) || !m_modelConditions.test(setBit))
		{
			m_modelConditions.clear(clearBit);
			m_modelConditions.set(setBit);
			rva0028AE6D();
		}
	}
	// Single-condition set/clear. Both test the byte that holds the bit and
	// clear/set the containing word, which is how PhysicsBehavior 0x00390629 and
	// 0x003906BF reach the flags.
	// PhysicsBehavior expands this method at 0x00390629 and 0x003906BF. Retail also emits a copy at 0x001E49B4.
	// Ordinary inlining of the five model-condition accessors loses 29 rows across 20 translation units.
	__forceinline void setModelConditionState(UnsignedInt bit)
	{
		if (!m_modelConditions.testByte(bit))
		{
			m_modelConditions.set(bit);
			rva0028AE6D();
		}
	}
	// PhysicsBehavior expands this method at 0x00390629 and 0x003906BF. Retail also emits a copy at 0x001E4981.
	// The ordinary inline mask form matches that copy and stops expanding at other call sites.
	// Ordinary inlining of the five model-condition accessors loses 29 rows across 20 translation units.
	__forceinline void clearModelConditionState(UnsignedInt bit)
	{
		if (m_modelConditions.testByte(bit))
		{
			m_modelConditions.clear(bit);
			rva0028AE6D();
		}
	}
	void kill(DamageType damageType, DeathType deathType);
	void forceEmotion(Int index, Real value, Int flag);	// 0x0028ECA8 (map: ?forceEmotion@Object@@QAEXHMH@Z; the script emotion actions call it)
	void setEmotion(Int index, void *p);			// 0x0028EC48: the outermost container's emotion tracker
	void setDisabledCounted(DisabledType type);			// 0x00292EB3: counted setDisabled (+0x1F8 per type)
	void startDefectionTimer(UnsignedInt timeToDetect);		// 0x0028BFF9: undetected defector while the timer runs
	void rva0028C0F2(UnsignedInt milliseconds);		// 0x0028C0F2: +0x448 = now + the time in logic frames
	void rva0028C24C();								// 0x0028C24C: +0x458 = now + ten seconds of logic frames
	Bool isImmuneToShockwave(Bool checkShip) const;			// 0x0028DB0F: ShockwaveResistance >= 100 (ships excepted when asked)
	Bool clearDisabledCounted(DisabledType type);			// 0x00292ED0: counted clearDisabled
	void attemptDamage(DamageInfo *damageInfo);
	HordeContainInterface *rva0029439D();
	Bool rva002943B2(const Player *player);					// 0x002943B2
	void teleportTo(const Coord3D *pos, Bool flag);			// 0x0029660C
	void restoreObjectToWorldInternal();						// 0x00291C20
	void restoreObjectToWorld(const Coord3D *pos);			// 0x0029766B
	HordeContainInterface *rva0028C197() const;				// 0x0028C197
	Bool rva0028AFBB() const;								// 0x0028AFBB
	Bool rva0028C149(int attrKind, float *value, int flag);	// 0x0028C149
	// 0x0028C15E: forwards its four arguments to the object's attribute-modifier
	// pool update (findAttributeModifierPoolUpdate, then 0x00403448); false
	// without one.
	Bool rva0028C15E(Int a, Real *value, Int b, Int c);
	// 0x0028ADEC: whether any of the object's weapons deals damage (Zero Hour's hasAnyDamageWeapon);
	// PropagandaTowerBehavior::effectLogic 0x00481A9E calls it on the target.
	Bool hasAnyDamageWeapon() const;
	Real rva0028B842() const;	// 0x0028B842 (Locomotor::getMaxSpeed 0x001E46E1: the crew-powered speed factor)
	Bool rva00293926(KindOfType kind);	// 0x00293926 (Locomotor 0x001E468F)
	void rva0028FCD9(Int state, Int side);	// 0x0028FCD9 (ActiveBody slots 36 0x004BDC03 and 23 0x004BDB37)
	Bool isVisibleToPlayer(void *player, Int flag);	// 0x00294471 (EmotionTrackerUpdate::TauntPointFilter::Allow 0x004B0FEB)
	Bool rva0028F44C(const Object *source) const;	// 0x0028F44C (Armor::adjustDamage 0x001D90A1: the source flanks this object)
	Real getLocomotorSpeed() const;	// 0x0028AC7D (StealthUpdate::rva00373F59 0x00373F59)
	void replaceModelConditionFlags(const Int *flags, Bool forceDrawableUpdate);	// 0x0028CFF5 (StealthUpdate::changeVisualDisguise 0x00374BC8)
	void forceRefreshSubObjectUpgradeStatus() const;	// 0x0028B3D7 (StealthUpdate::changeVisualDisguise 0x00374BC8)
	// (ActiveBody::setCorrectDamageState 0x004BE8BD, loadPostProcess 0x004BF40F):
	void setGeometryInfoZ(Real height);	// 0x0028ABFC
	void rva0028AB75(Bool flag);	// 0x0028AB75
	void rva0029895A(const GeometryInfo *geometry);	// 0x0029895A
	// 0x0028FB6F (CaveContain::removeFromContain 0x004666C9, OpenContain 0x00462FB3): the rider learns its container is gone.
	void onRemovedFrom(Object *container);
	// 0x0028ADD5: true when the object has no ammunition left.
	Bool isOutOfAmmo() const;
	void defect(Team *newTeam, UnsignedInt detectionTime);	// 0x00298C76 (CaveContain 0x00466848 changeTeamOnAllConnectedCaves)
	Int getTransportSlotCount();	// 0x0028FBBE (ControlBar::populateInvDataCallback 0x0053B98C)
	// 0x0028C1CC: true while the current weapon is not ready and, when it is, while
	// the firing tracker's last frame (+0x240 -> +0x44) is recent.
	Bool rva0028C1CC() const;
	void updateShroudNow();									// 0x0028C11A
	Bool isLocallyControlled() const;						// 0x0028B07A
	Drawable *getDrawable() const;							// 0x005508E2: reads +0x84 (an ICF fold; the map spells the shared body BuildListInfo::getDesiredGatherers); callers call it out of line
	Object *getNextObject() { return m_next; }
	const Object *getNextObject() const { return m_next; }
	Bool isSignificantlyAboveTerrain() const;
	Bool addAttributeModifierToPool(const AsciiString &name, int strength);
	void removeAttributeModifierFromPool(const AsciiString &name);	// 0x0028EB42
	void setCommandSetStringOverride(const AsciiString &name);	// 0x0029336F: the command-set override the upgrade applies
	Bool hasCommandButton(const CommandButton *button);	// 0x002922D9: whether the button is one of this object's own (its special power's module, then the command set named by rva00290E67); map row ?rva002922D9@Object@@QAE_NPBVCommandButton@@@Z (?:placeholder)
	void doCommandButton(const CommandButton *button, Int arg, Bool flag);	// 0x00296749
	void doCommandButtonAtPosition(const CommandButton *button, const Coord3D *pos, Int arg, Int flag);	// 0x00297149
	void rva00297000(const CommandButton *button, Object *obj, Int arg, Int flag);	// 0x00297000
	void clearAndSetModelConditionFlagsForHorde(const int *clear, const int *set);
	void setWeaponSetFlagForHorde(WeaponSetType flag);	// sets the flag on the horde leader and on every member of its horde
	void clearWeaponSetFlagForHorde(WeaponSetType flag);	// clears it the same way
	void rva0028CFB2(const int *clear, const int *set);
	void rva0028AE6D();
	// Ends every disabled type whose timer has run out, and clears the model condition the first type sets.
	void checkDisabledStatus();
	// Tests one DisabledType bit of m_disabledMask out of line; the placeholder name is the ledger's.
	Bool rva001E4A4E(Int type);
	void rva0028EBDA(Bool flag);							// 0x0028EBDA
	void setSpecialModelConditionState(ModelConditionFlagType flag, UnsignedInt frames);	// 0x0028AEB2
	void setDisabledUntil(DisabledType type, UnsignedInt frame);	// 0x0028B8B0
	ContainModuleInterface *getContain() const { return m_contain; }
	BodyModuleInterface *getBodyModule() const { return m_body; }
	AIUpdateInterface *getAI() const { return m_ai; }
	Locomotor *getCurrentLocomotor() const;
	Bool areModulesReady() const { return m_modulesReady; }	// Zero Hour's
	AIUpdateInterface *getAIUpdateInterface() { return m_ai; }
	const AIUpdateInterface *getAIUpdateInterface() const { return m_ai; }
	Object *getContainedBy() { return m_containedBy; }
	void onContainedBy(Object *containedBy);	// 0x0028FAD1 (TunnelTracker::onTunnelDestroyed 0x004F571B hands its surviving tunnel to the contained objects that entered the dead one)
	BehaviorModule **getBehaviorModules() const { return m_behaviors; }
	ExperienceTracker *getExperienceTracker() { return m_experienceTracker; }
	const ExperienceTracker *getExperienceTracker() const { return m_experienceTracker; }
	UpdateModule *findUpdateModule(NameKeyType key) const { return (UpdateModule *)findModule(key); }
	const Weapon *getCurrentWeapon(WeaponSlotType *slot = 0) const;	// 0x0028AEBD
	void setFiringConditionForCurrentWeapon();	// 0x0028FC8F
	void fireCurrentWeapon(const Coord3D *pos);	// 0x00291916 (fires the current weapon at pos; AIRampageState helper 0x0034BE57)
	Bool isMobile() const;						// 0x002907A1 (AIUpdateInterface::privateFacePosition 0x00267D65, privateTightenToPosition 0x00267DAD)
	Weapon *getWeaponInWeaponSlot(WeaponSlotType slot) const { return m_weaponSet.getWeaponInSlot(slot); }
	Real getVisionRange() const;
	void setVisionRange(Real range);
	Real getShroudClearingRange() const;
	void setShroudClearingRange(Real range);

	DamageModule *findDamageModule(NameKeyType key) const;
	void setNextPosition(const Coord3D &pos);
	void setNextPosition(const Coord3D *pos) { m_hasNextPosition=true; m_nextPosition=*pos; }
	// Rowed methods with unrecovered names.
	void rva00293105();										// 0x00293105
	void rva0023D3AF(void *arg);								// 0x0023D3AF
	void rva00346C53(ObjectStatusTypes bit, Bool set);			// 0x00346C53
	void rva00294C1A(Object *victim, Bool flag, Real amount);	// 0x00294C1A
	void setModelConditionStateForHorde(ModelConditionFlagType flag);
	void clearModelConditionStateForHorde(ModelConditionFlagType flag);
	void clearAndSetModelConditionStateForHorde(ModelConditionFlagType clearFlag, ModelConditionFlagType setFlag);
	void clearModelConditionFlagsForHorde(const int *flags);
	void setModelConditionFlagsForHorde(const int *flags);
	void replaceModelConditionFlagsForHorde(const int *flags, Bool forceDrawableUpdate);
	Bool rva0028C264(Int *out, Int value);					// 0x0028C264
	Object *rva002931F5(Bool checkProducer);					// 0x002931F5
	Real rva002615E3(const Coord3D *pos);				// 0x002615E3 (map: placeholder owner Rva000CBA20): squared distance in x and y from the position (+0x38) to pos
	CanAttackResult getAbleToUseWeaponAgainstTarget(AbleToAttackType t, const Object *victim, const Coord3D *pos, CommandSourceType cmdSource) const;	// 0x0028AF2B
	UnsignedByte rva0028F326(UnsignedInt arg, Real range);	// 0x0028F326 (map: placeholder owner Rva0028F326Owner; ecx is the object); StealthUpdate setWakeupIfInRange 0x00373DBE calls it
	Bool isAbleToAttack() const;								// 0x00290B73
	Bool rva0028B3A6() const;								// 0x0028B3A6 (49 B; bool, used by canSelectionSalvage)
	CanAttackResult getAbleToAttackSpecificObject(AbleToAttackType t, const Object *target, CommandSourceType cmdSource) const;	// 0x0028D051
	BloodthirstyUpdateInterface *rva0028C1A9() const;					// 0x0028C1A9

protected:
	// Thing (table 0x00BFC300 at +0); slot 7 is the deleting destructor 0x0029A10F.
	virtual ~Object();														// 0x00299CE4 (map ??1Object@@MAE@XZ)
	virtual Real calculateHeightAboveTerrain() const;						// 0x0028B569
	virtual Object *asObjectMeth() { return this; }							// 0x0047A6A9 (folded)
	virtual const Object *asObjectMeth() const { return this; }				// 0x0047A6A9 (folded)
	virtual void reactToTransformChange(const Matrix3D *oldMtx, const Coord3D *oldPos, Real oldAngle);	// 0x00292D49
	virtual void vslot6();													// 0x0028B193
	// Snapshot.
	virtual void xfer(Xfer *xfer);											// 0x00297684

	// Protected in retail (?findModule@Object@@IBE...); callers use findUpdateModule.
	Module *findModule(NameKeyType key) const;

private:
	AttributeModifierPoolUpdate *findAttributeModifierPoolUpdate() const;	// 0x0028BDD7

protected:

public:
	// 0x060 Snapshot, 0x064 ShroudObjectClient (vptr; its vbptr at 0x068),
	// 0x06C PartitionClient, 0x070 CollisionClient.
	ObjectID m_id;									// 0x074
	ObjectID m_producerID;							// 0x078
	ObjectID m_builderID;							// 0x07C
	ObjectID m_specialAbilityReservationOwnerID;	// 0x080 SpecialAbilityUpdate records the caster ID while it reserves this target.
	Drawable *m_drawable;							// 0x084
	AsciiString m_name;							// 0x088 ctor 0x00298F5A zeroes it, dtor 0x0029A077 releases it
	Object *m_next;								// 0x08C object list link (GameLogic::getFirstObject chain)
	Object *m_prev;								// 0x090 object list link (isInList 0x0028B47C)
	ObjectStatusMaskType m_status;					// 0x094 testStatus 0x0004E536 tests bit n here
	PathfinderPosGoalManager *m_pathfinderPosGoalManager;	// 0x0A4 (forwarders 0x0028ACCA..0x0028ADB0)
	GeometryInfo m_geometryInfo;					// 0x0A8
	GeometryInfo *m_geometryClone;					// 0x104
	const _STL::vector<GeometryInfo> *m_additionalGeometry;	// 0x108 indexed by getAdditionalGeometryInfo 0x00290EAB (0x5C elements)
	ModelConditionFlags m_modelConditions;			// 0x10C, 0x4C bytes
	Matrix3D m_158;									// 0x158
	int m_188;										// 0x188
	unsigned char m_18c[0x0C];						// 0x18C
	Coord3D m_nextPosition; // 0x198, PhysicsBehavior next path position
	Bool m_1a4;										// 0x1A4
	Bool m_1a5;										// 0x1A5
	Bool m_hasNextPosition;							// 0x1A6 set with m_nextPosition (setNextPosition)
	AIGroup *m_group;								// 0x1A8
	Real m_1ac;									// 0x1AC
	Real m_visionRange;							// 0x1B0
	Real m_shroudClearingRange;					// 0x1B4
	Real m_1b8;									// 0x1B8
	Real m_shroudRange;							// 0x1BC
	Real m_firingArcOrientation;	// 0x1C0 AIRotateFiringarc::update saves the owner's current orientation here.
	Real m_visionReferenceHeight;	// 0x1C4 The vision and shroud range getters measure height bonuses from this Z.
	BitFlags<11> m_disabledMask;					// 0x1C8 DisabledMaskType
	unsigned int m_disabledTillFrame[11];			// 0x1CC
	unsigned int m_disabledCounts[11];				// 0x1F8 per DisabledType: setDisabledCounted 0x00292EB3 disables on the first count
	void *m_224;									// 0x224
	void *m_228;									// 0x228
	void *m_22c;									// 0x22C
	ObjectSMCHelper *m_smcHelper;					// 0x230 setSpecialModelConditionState 0x0028AEB2 forwards to it
	void *m_234;									// 0x234
	ObjectDefectionHelper *m_defectionHelper;		// 0x238 (startDefectionTimer 0x0028BFF9 starts its timer)
	ObjectGuardingHelper *m_guardingHelper;			// 0x23C (the helper module: addGuarder/removeGuarder forward to it)
	FiringTracker *m_firingTracker;					// 0x240
	BehaviorModule **m_behaviors;					// 0x244
	Bool m_248;										// 0x248
	Bool m_hasChargeAttackAbility;	// 0x249. The constructor records whether findSpecialAbilityUpdate(SPECIAL_CHARGE_ATTACK) succeeds. Locomotor uses this flag.
	EmotionTrackerUpdate *m_emotionTracker;			// 0x24C (setEmotion 0x0028EC48, forceEmotion 0x0028ECA8 forward to it)
	ContainModuleInterface *m_contain;				// 0x250
	BodyModuleInterface *m_body;					// 0x254
	AIUpdateInterface *m_ai;						// 0x258
	BehaviorModule *m_physics;						// 0x25C
	void *m_radarData;								// 0x260
	ExperienceTracker *m_experienceTracker;			// 0x264
	unsigned char m_268[0xC];						// 0x268 _STL::map<int, void *>
	Object *m_containedBy;							// 0x274
	ObjectID m_xferContainedByID;					// 0x278
	unsigned int m_containedByFrame;				// 0x27C
	Real m_constructionPercent;					// 0x280
	UpgradeMaskType m_upgrades;					// 0x284
	Team *m_team;									// 0x304
	AsciiString m_originalTeamName;					// 0x308 (setTeam 0x00298AE4 sets it to the prototype's owner/name)
	int m_indicatorColor;							// 0x30C
	Coord3D m_healthBoxOffset;						// 0x310
	Real m_31c[2];									// 0x31C
	Real m_paidBuildCost;	// 0x324. DozerAIUpdate records this cost on the phantom and refunds it on destruction. RebuildHoleBehavior saves it for rebuilding.
	Object *m_prevTeamMember;						// 0x328 the TeamMemberList DLINK pair (0x0028A6E7 prependTo, 0x0028A704 removeFrom)
	Object *m_nextTeamMember;						// 0x32C
	WeaponSet m_weaponSet;							// 0x330
	WeaponSetFlags m_weaponSetFlags;				// 0x370 BitFlags<104> (WeaponSet chooses its template set from it)
	UnsignedInt m_weaponBonusCondition;				// 0x380 (Weapon::computeBonus)
	unsigned char m_384[6];							// 0x384
	Coord3D m_currentTargetPosition;	// 0x38C Lua's drawable target distance and bearing are measured to this position.
	ObjectID m_currentTargetID;								// 0x398, the target the drawable callbacks read (CurDrawableIsCurrentTargetKindof)
	void *m_39c;									// 0x39C
	void *m_3a0;									// 0x3A0
	UnsignedInt m_specialPowerBits[5];				// 0x3A4 one bit per SpecialPowerType (hasSpecialPower 0x0028D8EB passes it to the bit test 0x001E4426)
	ObjectID m_soleHealingBenefactorID;				// 0x3B8
	UnsignedInt m_soleHealingBenefactorExpirationFrame;	// 0x3BC
	// The polygon triggers the object is tracked against: isInside 0x0028B411,
	// didEnter 0x0028D718 and didExit 0x0028D757 test bytes +6, +4 and +5 of
	// each 8-byte entry for the first m_numTriggerAreasActive (+0x43A) entries.
	struct TriggerInfo
	{
		const PolygonTrigger *pTrigger;	// +0
		Byte entered;						// +4
		Byte exited;						// +5
		Byte isInside;						// +6
		Byte padding;						// +7
	};
	TriggerInfo m_triggerInfo[7];					// 0x3C0
	UnsignedInt m_enteredOrExitedFrame;				// 0x3F8 didEnterOrExit 0x0028D6EB compares it with the frame and the frame before
	int m_3fc[3];									// 0x3FC
	void *m_408;								// 0x408 _STL::list<CameraMarker>
	PathfindLayerEnum m_layer;						// 0x40C Zero Hour's m_layer (setLayer 0x0028B4CE)
	int m_formationID;	// 0x410. AIFollowPathAsTeamState compares formation IDs. AIGroup and AIUpdate clear the ID when starting a new waypoint path.
	Real m_414[2];									// 0x414
	AsciiString m_primaryCommandSetOverride;	// 0x41C. The command-set lookup at 0x00290E67 checks this override first.
	AsciiString m_upgradeCommandSetOverride;	// 0x420 CommandSetUpgrade clears this override on removal when it matches the upgrade's command set.
	AsciiString m_secondaryCommandSetOverride;	// 0x424. The command-set lookup checks this after the primary override and before the upgrade override.
	UnsignedInt m_occlusionDelayFrame;				// 0x428 Zero Hour's: GarrisonContain/TunnelContain store the template's occlusion delay plus the frame
	int m_42c;										// 0x42C
	int m_430;										// 0x430
	Bool m_isSelectable;							// 0x434 (setSelectable 0x0028B76D)
	Bool m_modulesReady;							// 0x435 Zero Hour's areModulesReady (Player::becomingTeamMember 0x002AD4EC tests it)
	Bool m_436;										// 0x436
	UnsignedByte m_scriptStatus;									// 0x437 (the mask Object::setScriptStatus 0x00292969 or/ands: bit 1 script disabled, bit 2 script unpowered)
	// Zero Hour's private status bits: setEffectivelyDead 0x0028D2FB toggles 0x01,
	// friend_setUndetectedDefector 0x0028AC34 0x02, setCaptured 0x0028B21E 0x04;
	// isOffMap's readers test 0x08.
	enum { EFFECTIVELY_DEAD = 0x01, UNDETECTED_DEFECTOR = 0x02, CAPTURED = 0x04, OFF_MAP = 0x08 };
	UnsignedByte m_privateStatus;					// 0x438
	UnsignedByte m_privateStatus2;					// 0x439 bit 1: masked (maskObject 0x0028B7E2 sets and clears it)
	Char m_numTriggerAreasActive;					// 0x43A (read with movsx)
	Bool m_43b;										// 0x43B
	Bool m_isReceivingDifficultyBonus;				// 0x43C Zero Hour's (setReceivingDifficultyBonus 0x0028B238)
	_STL::list<DamageInfo> m_delayedDamage;	// 0x440 holds copies of damage events delayed by the Object.
	Real m_cachedNearbyThreat;	// 0x444. The nearby-threat getter at 0x002972B1 returns this value to ScriptConditions.
	int m_nearbyThreatCacheUntilFrame;	// 0x448. The nearby-threat getter recomputes the cache after this logic frame.
	ObjectID m_leaderID;	// 0x44C The script team-leader condition compares this ID to the selected unit's ID.
	void *m_450;									// 0x450
	Bool m_isRegisteredInWorld;	// 0x454. The registration path at 0x0028DCC4 sets this flag and 0x0028BAC0 clears it.
	Bool m_455;										// 0x455
	Bool m_456;										// 0x456
	int m_458;										// 0x458
	int m_armySummaryKey;	// 0x45C. Production and respawn forward this key to the ArmySummarySystem lookup at 0x0040D280.
	int m_460;										// 0x460
	int m_464;										// 0x464
	unsigned char m_468[0x18];						// 0x468
	Bool m_480;										// 0x480
	ObjectID m_specialAbilityTargetID;	// 0x484 SpecialAbilityUpdate records the target or its containing horde here.
	ObjectID m_playerOrderedRangedTargetID;	// 0x488 AIUpdate records the designated victim of a player-ordered ranged attack here.
	Bool m_forceGroundLayer;						// 0x48C rva0028B95F sets it with the frame; while set, rva0028B511 answers the ground layer
	int m_forceGroundLayerFrame;					// 0x490
	unsigned char m_494[4];							// 0x494 AsciiString
	int m_498;										// 0x498
	int m_49c;										// 0x49C
	int m_4a0;										// 0x4A0
	void *m_4a4[3];								// 0x4A4 _STL::vector<int>
	Bool m_isPhantomStructureInert;	// 0x4B0 DozerAIUpdate sets this when making a phantom structure inert and clears it when reactivating it.
	_STL::vector<StealthUpdate *> m_stealthUpdates;	// 0x4B4 rva0028F4BC 0x0028F4BC walks [+0x4B4, +0x4B8) and calls StealthUpdate::hasRequiredUpgrades 0x00373EC6 on each non-null element
	unsigned int m_4c0;								// 0x4C0
	PartitionData *m_partitionData;					// 0x4C4 the shroud manager's data (WorldBuilder m_smData)
	PartitionManager::Data *m_partitionManagerData;	// 0x4C8 the partition manager's data
	CollisionManager::Data *m_collisionManagerData;	// 0x4CC the collision manager's data
	// 0x4D0 the vtordisp, 0x4D4 the virtual ShroudClient (vptr).
};
// SIZE 0x4D8

// The delayed-damage list must fit the Object's single-word storage.
typedef char ObjectDelayedDamageLayout[
	(sizeof(_STL::list<DamageInfo>) == sizeof(void *) && offsetof(Object, m_delayedDamage) == 0x440) ? 1 : -1];

#endif
