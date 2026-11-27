// AIUpdate.h: SAGE AI update module, AIUpdateInterface. SIZE 0x3E4.
// Portions derived from Command & Conquer Generals and Command & Conquer Generals Zero Hour, (c) Electronic Arts Inc., GPL-3.0-or-later; see LICENSE and NOTICE.
// Target facts:
// - Bases: UpdateModule (0x20), AICommandInterface at +0x20, CollideModuleInterface
//   at +0x24. Constructor 0x0026E9BD calls UpdateModule's, then stores the tables
//   0x00BFA480 (+0), 0x00BFA3C0 (+0xC), 0x00C53A98 (+0x10), 0x00C47B88 (+0x20,
//   aiDoCommand 0x002673F6) and 0x00C52F88 (+0x24: onCollide 0x0026DF0D, slot 1
//   the shared one-argument `return false` 0x005CB9FF, slots 2-5 `return false`).
//   Its base table 0x00C40818 is six __purecall slots (CollideModuleInterface).
//   BloodthirstyUpdate 0x0044E4A9 adds 0x20 to Object::getAI's result before aiIdle.
// - Primary table 0x00BFA480 has 154 slots. Slots 13-153 below carry the map's
//   names and signatures where its row is evidenced; a slot whose target is a
//   folded body keeps a neutral vslotNNN name and the body's shape in its comment.
//   Earlier recovered slots: 91 isBusy (0x004DD068 calls +0x16C), 98 the aircraft
//   receiver (0x003A4F21), 110 isIdle (0x002645FF), 142 chooseLocomotorSet
//   (FlammableUpdate 0x0048C8F9/0x0048D0BE call +0x238; the constructor calls
//   0x00268B20 directly with 0).
// - Behavior getters (table 0x00BFA3C0): getCollide 0x003730FC (slot 1),
//   getAIUpdateInterface 0x004FA5F4 (slot 19).
// - Members from the constructor and xfer 0x00267EDD (operator== slots: 0x60
//   Coord3D, 0x6C AsciiString, 0x70 Real, 0x78 UnsignedInt, 0x7C Int, 0x90 Bool;
//   0x24 raw bytes). Zero Hour AIUpdate.h is the name donor where the type and
//   the initial value agree (0xFACADE waypoint IDs, 16 queued waypoints, the
//   CMD_FROM_AI last command source); other members keep offset names.
// Non-virtual: SetEmotionState 0x0026320D (int state and Object *target), the
// idle nudge 0x00263298, the deactivation nudge 0x002632E1, getCurrentStateID
// 0x00262FC3, isMoving 0x00264688. OnActivation writes the byte at +0x3C5
// straight after SetEmotionState.
#pragma once
#ifndef BFME2_AIUPDATE_H
#define BFME2_AIUPDATE_H

#include "Common/GameType.h"
#include "Common/BitFlags.h"
#include "Common/BitFlags11.h"
#include "Common/Vector.h"
#include "GameLogic/AICommandInterface.h"
#include "GameLogic/Module/UpdateModule.h"
#include "GameLogic/Module/CollideModule.h"
#include "GameLogic/Object/LocomotorSet.h"
#include "GameLogic/Module/SupplyTruckAIInterface.h"
#include "GameLogic/AIStoredCommand.h"
#include "Common/Snapshot.h"
#include "Common/StlList.h"
#include "GameLogic/ScriptTracker.h"
#include "GameLogic/Module/ModuleData.h"
#include "GameLogic/TurretAI.h"
#include "Common/AsciiString.h"
#include <stddef.h>
#include <map>
#include <vector>

class INI;
class MultiIniFieldParse;
extern Int BfmeLogicFramesPerSecond;	// VA 0x00DBA4E4 (GameLogic.h); the constructor below reads it
class Object;
class LocomotorSet;
class Locomotor;
class Waypoint;
class Team;
class PolygonTrigger;
class DamageInfo;
class CommandButton;
class ThingTemplate;
class Player;
class Path;
class TurretAI;
class AIStateMachine;
class Rva00263404Source;
class AttackPriorityInfo;
class PathfindServicesInterface;
class DozerAIInterface;
class WorkerAIInterface;
class GiantBirdAIUpdate;
class AnimalAIInterface;
class AssaultTransportAIInterface;
struct Coord2D;
enum WeaponSlotType;
enum AttitudeType;
enum MoodMatrixAction;
enum PathfindLayerEnum;
struct AICommandParms;

// LuaSpyData: AIUpdateInterface's +0x224 member, a 0x10-byte Snapshot. Target facts (retail game.dat):
// - constructor 0x0026E7C8 stores table 0x00BFA3A4 and constructs the +0x4 vector (_Vector_base 0x00211E58);
// - table 0x00BFA3A4: slot 0 deleting destructor 0x0026E81A, slot 1 the folded empty body 0x000B3FD0
//   (loadPostProcess), slot 2 0x0026E7E4 returns the literal "LuaSpyData" (GetSnapshotName: the class name),
//   slot 3 xfer 0x00332975 (Version 1, the element count as size/12, per element two name keys written as
//   AsciiStrings and the element's +0x8 ObjectID list through xferSTLObjectIDList);
// - destructor 0x0026E7EA (COMDAT) destroys the vector (0x0026E5B9: range destroy 0x0026E1DE, then free) and
//   restores Snapshot's table 0x00BBB554; ~AIUpdateInterface 0x0026E836 calls it out of line.
// Member names are roles read from xfer.
struct LuaSpyEntry
{
	NameKeyType m_firstKey;						// +0x0, xfer'd as its name
	NameKeyType m_secondKey;					// +0x4, xfer'd as its name
	_STL::list<ObjectID> m_objectIDs;			// +0x8, xferSTLObjectIDList
};

class LuaSpyData : public Snapshot
{
public:
	friend class AIUpdateInterface;				// AIUpdateInterface::xfer 0x00267EDD calls the protected xfer through the table (slot 3)
	LuaSpyData() {}								// 0x0026E7C8 (COMDAT)
	virtual ~LuaSpyData() {}					// 0x0026E7EA (COMDAT)
protected:
	virtual void loadPostProcess() {}			// slot 1, the folded empty body 0x000B3FD0
	virtual const char *GetSnapshotName() const;	// slot 2, 0x0026E7E4
	virtual void xfer(Xfer *xfer);				// slot 3, 0x00332975

	_STL::vector<LuaSpyEntry> m_entries;		// +0x4
};
// SIZE 0x10
// LocomotorSetType: Zero Hour's key of the AI module data's locomotor map (the map spells its operator[] 0x001EA05B
// ?$map@W4LocomotorSetType@@...); BFME 2 reads the names through INI::scanIndexList with the name list at VA 0x00DBB4F0.
enum LocomotorSetType
{
	LOCOMOTORSET_INVALID = -1,
	LOCOMOTORSET_NORMAL = 0
};
class LocomotorTemplate;
typedef _STL::vector<const LocomotorTemplate *> LocomotorTemplateVector;	// Zero Hour's typedef
typedef _STL::map<LocomotorSetType, LocomotorTemplateVector> LocomotorTemplateMap;	// Zero Hour's typedef

// AIUpdateModuleData: Zero Hour's class (AIUpdate.h), the INI data of every AI update. SIZE 0x64 (ModuleFactory's
// operator new for AIUpdateInterface, TransportAIUpdate and the hordes). Target facts: the constructor 0x0026E5D7 (a
// COMDAT, expected in AIUpdate.cpp) stores the table 0x007FA288 (slot 0 ??_G 0x0026E67D, slot 4 the shared `return
// true` 0x0050B5C6: isAiModuleData), builds the map at +0x8 and the strings and vector below, and is the base call of
// every AI module data's constructor (Animal 0x0024BE43, AssaultTransport 0x0024D221, SupplyTruck 0x0024EE77, Wander
// 0x0024F1CD, Worker 0x00254434, DeployStyle 0x00255154, GiantBird 0x0036812E, Dozer 0x00488A81, Siege 0x004904D9).
// The destructor 0x0026E1FC deletes +0x14 (null-tested) and destroys +0x58, +0x44, +0x2C, +0x8 in that order.
// buildFieldParse 0x002638FF adds the table 0x007F9378; members are its entries (offset, parse function's type), named
// as in Zero Hour where Zero Hour has the field and after the INI token for BFME 2's own. The map still spells the
// class TransportAIUpdateModuleData at 0x0026E5D7/0x0026E1FC/0x0026E67D/0x002638FF/0x0024BDF2.
class AIUpdateModuleData : public ModuleData
{
public:
	LocomotorTemplateMap m_locomotorTemplates;				// +0x08 Locomotor (parseLocomotorSet 0x0026E699)
	const TurretAIData *m_turretData;						// +0x14 Turret (parseTurret 0x002623C1: new TurretAIData 0x70)
	UnsignedInt m_moodAttackCheckRate;						// +0x18 MoodAttackCheckRate; duration, frames
	UnsignedInt m_autoAcquireEnemiesWhenIdle;				// +0x1C AutoAcquireEnemiesWhenIdle; bit string
	Real m_stopChaseDistance;								// +0x20 StopChaseDistance
	Bool m_standGround;										// +0x24 StandGround
	Bool m_canAttackWhileContained;							// +0x25 CanAttackWhileContained
	Real m_holdGroundCloseRangeDistance;					// +0x28 HoldGroundCloseRangeDistance
	AsciiString m_aiLuaEventsList;							// +0x2C AILuaEventsList
	UnsignedInt m_maxCowerTime;								// +0x30 MaxCowerTime; duration, frames
	UnsignedInt m_minCowerTime;								// +0x34 MinCowerTime; duration, frames
	UnsignedInt m_rampageTime;								// +0x38 RampageTime; duration, frames
	Bool m_rampageRequiresAflame;							// +0x3C RampageRequiresAflame
	UnsignedInt m_burningDeathTime;							// +0x40 BurningDeathTime; duration, frames
	AsciiString m_attackPriority;							// +0x44 AttackPriority ("DefaultAttackPriority")
	UnsignedInt m_timeToEjectPassengersOnRampage;			// +0x48 TimeToEjectPassengersOnRampage; duration, frames
	Real m_comboLocoAttackDistance;							// +0x4C ComboLocoAttackDistance
	Int m_comboLocomotorSet;								// +0x50 ComboLocomotorSet; index into the locomotor-set names
	Bool m_fadeOnPortals;									// +0x54 FadeOnPortals
	_STL::vector<AsciiString> m_specialContactPoints;		// +0x58 SpecialContactPoints (INI::parseAsciiStringVectorAppend)

	// The constructor's scalar stores precede the +0x44 string's constructor: an initializer list; +0x14 is set in the body.
	AIUpdateModuleData()
		: m_moodAttackCheckRate(BfmeLogicFramesPerSecond * 2),
		  m_autoAcquireEnemiesWhenIdle(0),
		  m_stopChaseDistance(500.0f),
		  m_standGround(false),
		  m_canAttackWhileContained(false),
		  m_holdGroundCloseRangeDistance(0.0f),
		  m_maxCowerTime(0),
		  m_minCowerTime(0),
		  m_rampageTime(0),
		  m_rampageRequiresAflame(false),
		  m_burningDeathTime(0),
		  m_attackPriority("DefaultAttackPriority"),
		  m_timeToEjectPassengersOnRampage(0),
		  m_comboLocoAttackDistance(80.0f),
		  m_comboLocomotorSet(0),
		  m_fadeOnPortals(false)
	{
		m_turretData = NULL;
	}
	virtual ~AIUpdateModuleData()
	{
		if (m_turretData)
			::delete m_turretData;
	}

	// Retail's tables for this family have the shared `return true` stub 0x0050B5C6 in slot 4; ModuleData's default is
	// false (0x0047A699). ThingTemplate's 0x0033C882 asks slot 4 (isAiModuleData).
	virtual Bool isAiModuleData() const { return true; }		// slot 4

	static void buildFieldParse(MultiIniFieldParse &parse);		// 0x002638FF
	static void parseLocomotorSet(INI *ini, void *instance, void *store, const void *userData);	// 0x0026E699
	static ModuleData *friend_newModuleData(INI *ini);			// 0x0024BDF2

private:
	static void parseTurret(INI *ini, void *instance, void *store, const void *userData);			// 0x002623C1
};
typedef char AIUpdateModuleData_size[(sizeof(AIUpdateModuleData) == 0x64) ? 1 : -1];
typedef char AIUpdateModuleData_off_m_turretData[(offsetof(AIUpdateModuleData, m_turretData) == 0x14) ? 1 : -1];
typedef char AIUpdateModuleData_off_m_aiLuaEventsList[(offsetof(AIUpdateModuleData, m_aiLuaEventsList) == 0x2C) ? 1 : -1];
typedef char AIUpdateModuleData_off_m_attackPriority[(offsetof(AIUpdateModuleData, m_attackPriority) == 0x44) ? 1 : -1];
typedef char AIUpdateModuleData_off_m_fadeOnPortals[(offsetof(AIUpdateModuleData, m_fadeOnPortals) == 0x54) ? 1 : -1];
typedef char AIUpdateModuleData_off_m_specialContactPoints[(offsetof(AIUpdateModuleData, m_specialContactPoints) == 0x58) ? 1 : -1];

// What an AI tells a container about leaving it (Zero Hour's AIUpdate.h): slot 106,
// getAiFreeToExit; the base returns FREE_TO_EXIT.
enum AIFreeToExitType
{
	FREE_TO_EXIT,
	NOT_FREE_TO_EXIT,
	WAIT_TO_EXIT
};

class AIUpdateInterface : public UpdateModule, public AICommandInterface, public CollideModuleInterface
{
	friend class Object;	// Object::rva0028AE6D 0x0028AE6D tail-calls the protected wakeUpNow 0x00262871 (Zero Hour's AIUpdateInterface names Object a friend)
	friend class ScriptTracker;	// ScriptTracker::update 0x0026249D reads the owner object (Team) of the tracker's AIUpdateInterface
	friend class AIGroup;	// AIGroup::groupIdle 0x0036FC23 sets +0x3CC on a player's idle order
public:
	// Zero Hour's inline accessors over the module data (+0x1C AutoAcquireEnemiesWhenIdle, bit 1 while stealthed):
	// AIGroup::groupIdle 0x0036FC23 reads [data+0x1C] twice in place.
	const AIUpdateModuleData *getAIUpdateModuleData() const { return (const AIUpdateModuleData *)getModuleData(); }
	Bool canAutoAcquire() const { return getAIUpdateModuleData()->m_autoAcquireEnemiesWhenIdle != 0; }
	Bool canAutoAcquireWhileStealthed() const { return ((getAIUpdateModuleData()->m_autoAcquireEnemiesWhenIdle >> 1) & 1) != 0; }
public:
	AIUpdateInterface(Thing *thing, const ModuleData *moduleData);	// 0x0026E9BD

	void SetEmotionState(int state, Object *target);	// 0x0026320D
	void rva00263298();						// 0x00263298
	void rva00262FFF();						// 0x00262FFF (38 B; StealthUpdate setWakeupIfInRange 0x00373DBE calls it on an object it just woke)
	Bool rva00262DD3(const Object *other) const;	// 0x00262DD3 (GateOpenAndCloseBehavior push-away 0x00498DF3)
	void rva002632E1();						// 0x002632E1
	int getCurrentStateID() const;			// 0x00262FC3
	void rva0026331C();						// 0x0026331C
	Int rva00265143();						// the goal path's point count in two particular machine states, else 0
	void setLocomotorUpgrade(Bool set);	// 0x00262804 (Zero Hour's name; placeholder row): stores m_upgradedLocomotors and re-chooses the normal set (slot 142) when the normal or upgraded set is current
	Bool isMoving() const;					// 0x00264688
	void restartWaypointPath();				// 0x0026293E
	void setDesiredSpeed(Real speed);	// 0x002628F9 (+0x44 m_desiredSpeed; role name on a placeholder-class row)
	ObjectID getIgnoredObstacleID() const;	// 0x0006E009 (+0x164 m_ignoreObstacleID; role name on a placeholder row)
	void destroyPathAndIdle();				// 0x00262D2D
	void startGuardingHere(Int unknownValue);	// 0x00262D40
	Int hasTemporaryState() const;			// 0x002632B3
	Int hasTimelessTemporaryState() const;	// 0x002632C7
	void setBusyUntilFrame(Int frame);		// 0x0026304D
	void EndDefectedStateMachine();			// 0x002633B6
	void friend_setGoalObject(Object *obj);	// 0x00262B0F
	void rva00262989(UnsignedInt id);	// 0x00262989 (the map's name): records a collider in m_recentCollisionIDs (onCollide 0x0026DF0D calls it)
	void rva0026D3FB(Object *victim);	// 0x0026D3FB (the map's name): attack the victim, or make it the goal while guarding idle or in states 0x21/0x3D (Object 0x00295A84 and HordeMeleeAmoeba::AttackUnit 0x00587253 call it)
	Int rva00262552(const Coord3D *destination);	// 0x00262552 (the map's name): the layer transition from the owner's layer to the destination's (0..5)
	ObjectID getCrateID() const { return m_crateCreated; }

	// The map's other evidenced non-virtual members, with their decorated signatures.
	static Module *friend_newModuleInstance(Thing *, const ModuleData *);	// 0x0024bdb7 N:ledger
	void setFinalPosition(const Coord3D *);	// 0x002622e9 N:ledger
	void setIgnoreCollisionTime(Int);	// 0x00262318 N:ledger
	void setTurretTargetObject(WhichTurretType, Object *, Bool);	// 0x002625e6 N:ledger
	Object *getTurretTargetObject(WhichTurretType);	// 0x00262605 N:ledger
	void setTurretTargetPosition(WhichTurretType, const Coord3D *);	// 0x00262636 N:ledger
	void setTurretEnabled(WhichTurretType, Bool);	// 0x00262651 N:ledger
	void recenterTurret(WhichTurretType);	// 0x0026266c N:ledger
	Bool isTurretEnabled(WhichTurretType) const;	// 0x00262683 N:ledger
	Bool isTurretInNaturalPosition(WhichTurretType) const;	// 0x0026269c N:ledger
	Bool isWeaponSlotOnTurretAndAimingAtTarget(WeaponSlotType, const Object *) const;	// 0x002626b7 N:ledger
	Bool getTurretRotAndPitch(WhichTurretType, Real *, Real *) const;	// 0x002626fa N:ledger
	Real getTurretTurnRate(WhichTurretType) const;	// 0x00262730 N:ledger
	WhichTurretType getWhichTurretForCurWeapon() const;	// 0x0026275e N:ledger
	WhichTurretType getWhichTurretForWeaponSlot(WeaponSlotType, Real *, Real *) const;	// 0x0026278a N:ledger
	void setQueueForPathTime(Int);	// 0x0026282a N:ledger
	void processSpies(Int, void *, void *);	// 0x00262907 N:ledger
	Bool queueWaypoint(const Coord3D *);	// 0x00262912 N:ledger
	void markAsDead();	// 0x0026296d N:ledger
	Int rva00260DED() const;	// 0x00260DED (RespawnUpdate::update 0x004AF92F compares it with 0x10)
	Bool canComputeQuickPath();	// 0x00262a64 N:ledger
	void destroyPath();	// 0x00262a8a N:ledger
	void friend_setPath(Path *);	// 0x00262af9 N:ledger
	Bool isPathAvailable(const Coord3D *) const;	// 0x00262b53 N:ledger
	Bool isAircraftThatAdjustsDestination() const;	// 0x00262cbe N:ledger
	void BeginTemporaryStateMachine();	// 0x002632ee N:ledger
	void BeginDefectedStateMachine();	// 0x0026336d N:ledger
	void setPathExtraDistance(Real);	// 0x002633f3 N:ledger
	void setGoalPositionClipped(const Coord3D *, Real, Int);	// 0x00263910 N:ledger
	void rva00265667(const Coord3D *pos, Int cmdSource);	// 0x00265667: setGoalPositionClipped with the unlimited range
	const char *name() const;				// 0x0026E982 (the map files the slot-2 body under this name)
	void requestAttackPath(ObjectID, const Coord3D *);	// 0x00263a39 N:ledger
	void requestApproachPath(Coord3D *);	// 0x00263b33 N:ledger
	Bool requestMeleeApproachPath(const Coord3D *, Bool);	// 0x00263c06 N:ledger
	void requestSafePath(ObjectID);	// 0x00263ea2 N:ledger
	void chooseGoodLocomotorFromCurrentSet();	// 0x00263fa7 N:ledger
	Bool isQuickPathAvailable(const Coord3D *) const;	// 0x00264274 N:ledger
	Real getLocomotorDistanceToGoal();	// 0x0026435e N:ledger
	AttitudeType getAttitude() const;	// 0x00264eb6 N:ledger
	UnsignedInt getMoodMatrixValue() const;	// 0x00264f5e N:ledger
	UnsignedInt getMoodMatrixActionAdjustment(MoodMatrixAction) const;	// 0x00264ff8 N:ledger
	void setPathFromWaypoint(const Waypoint *, const Coord2D *);	// 0x0026569f N:ledger
	void moveThroughPortalWithMicropath(const Coord3D *, const Coord3D *, Int, PathfindLayerEnum, const Coord3D *, const Coord3D *);	// 0x0026594f N:ledger
	Int handleEngagedFlag();	// 0x00265af9 N:ledger
	Real getCurLocomotorSpeed() const;	// 0x002627E8 (Zero Hour's name; map placeholder ?rva002627E8@Rva002627E8@@QBEMXZ)
	const Coord3D *friend_getGoalPathPosition(Int index) const;	// 0x000DFCCD: m_stateMachine->getGoalPathPosition(index)
	Bool computeQuickPath(const Coord3D *);	// 0x00265c5b N:ledger
	Bool computePath(PathfindServicesInterface *, Coord3D *);	// 0x00265e0b N:ledger
	Bool computeAttackPath(PathfindServicesInterface *, const Object *, const Coord3D *);	// 0x00266aa2 N:ledger
	Bool findNearestLabeledContactPointOnTarget(Object *, Coord3D *, const Coord3D *, Bool);	// 0x0026872b N:ledger
	void requestPath(Coord3D *, Bool);	// 0x0026893f N:ledger
	Bool chooseLocomotorSetExplicit(Int);	// 0x00268a37 N:ledger
	void setCompletedWaypoint(const Waypoint *);	// 0x00268aaa N:ledger
	Object *checkForCrateToPickup();	// 0x00268b79 N:ledger
	void transferAttack(ObjectID, ObjectID);	// 0x00268cdc N:ledger
	void setCurrentVictim(const Object *);	// 0x00268d1f N:ledger
	Object *getCurrentVictim() const;	// 0x00268d71 N:ledger
	const Coord3D *getCurrentVictimPos() const;	// 0x00264E93 (Zero Hour's name; the map's placeholder is ?rva00264E93@Rva00264E93@@QAEPAXXZ): the goal position while attacking with no victim object
	void rva0026DE3B(Int attitude);	// 0x0026DE3B stores m_attitude; at -3 drops the current victim (and idles if there was one)
	void ignoreObstacle(const Object *);	// 0x00268d88 N:ledger
	void ignoreObstacleID(ObjectID id);	// stores the id at +0x164 (out of line; defined with the other AIUpdateInterface rows)
	void setCanPathThroughUnits(Bool canPath) { m_canPathThroughUnits = canPath; }
	Bool isWaitingForPath() const { return m_waitingForPath; }
	Bool hasDesiredAngle() const;	// 0x0033F8D4: m_unk0x1A0 != FLT_MAX (AIFaceDirectionState::onEnter 0x00342572; map ?rva0033F8D4@AIUpdateInterface@@QBE_NXZ, ?:placeholder)
	Path *getPath() { return m_path; }		// inline (Zero Hour's): AIMoveAwayPanicState::update 0x00347B3D reads +0x140 in place
	AIStateMachine *getStateMachine() { return m_stateMachine; }	// inline (AIGuardRetaliateState::onEnter 0x003463DA reads +0x30)	// inline in AIInternalMoveToState::computePath
	void clearWaypointPath();				// 0x0026295E (role name, placeholder row): empties m_waypointCount and clears +0x3BF; ScriptActions' face and move rows call it
	void friend_endingMove();				// 0x00262AEA (Zero Hour name; placeholder row): sets +0x3B6, clears +0x3B7; AIInternalMoveToState::onExit 0x003473A4 calls it
	void rva00262ACE();						// 0x00262ACE: clears +0x3B6/+0x16C/+0x3B8, sets +0x3B7 (computePath calls it after a request)	// inline in AIDockState's update/onExit
	Object *getNextMoodTarget(Bool, Bool);	// 0x00268eb8 N:ledger
	void resetNextMoodCheckTime();	// 0x00263025
	WhichTurretType friend_getTurretSync() const { return (WhichTurretType)m_turretSyncFlag; }
	void friend_setTurretSync(WhichTurretType turret) { m_turretSyncFlag = turret; }
	UnsignedInt friend_getNextMoodCheckTime() const { return m_unk0x21C; }
	void NotifyPathHasInvalidPortals();	// 0x00269566 N:ledger
	UpdateSleepTime quickHordeUpdate();	// 0x0026c4eb N:ledger
	Locomotor *getCurLocomotor();	// 0x0026ff1e N:ledger

	// Accessed from outside the class; AICommandInterface is the base at +0x20.
	AICommandInterface *getAICommandInterface() { return this; }

	// Behavior getters (BehaviorModuleInterface at +0xC).
	virtual CollideModuleInterface *getCollide() { return this; }		// 0x003730FC
	virtual AIUpdateInterface *getAIUpdateInterface() { return this; }	// 0x004FA5F4

	// Module slots.
	virtual NameKeyType getModuleNameKey() const;		// slot 4, 0x0026E93D
	virtual void onObjectCreated();						// slot 5, 0x002625C8
	virtual void xfer(Xfer *xfer);						// slot 3, 0x00267EDD (public in retail)

	// AICommandInterface.
	virtual void aiDoCommand(const AICommandParms *parms);	// 0x002673F6

	// CollideModuleInterface (+0x24).
	virtual void onCollide(Object *other, const Coord3D *loc, const Coord3D *normal);	// 0x0026DF0D
	virtual Bool wouldLikeToCollideWith(const Object *other) const { return false; }
	virtual Bool isHijackedVehicleCrateCollide() const { return false; }
	virtual Bool isCarBombCrateCollide() const { return false; }
	virtual Bool isRailroad() const { return false; }
	virtual Bool isSalvageCrateCollide() const { return false; }

	friend class Object;	// Object::rva0028AE6D 0x0028AE6D tail-calls the protected wakeUpNow
protected:
	virtual ~AIUpdateInterface();						// 0x0026E836
	virtual void loadPostProcess();						// slot 1, 0x002686A4
	virtual const char *GetSnapshotName() const;		// slot 2, 0x0026E982

	virtual SleepyUpdatePhase getUpdatePhase() const { return (SleepyUpdatePhase)0; }	// slot 12 is the shared `xor eax,eax; ret` 0x000D43D0 in every AI module's table, not UpdateModule's 0x0022C4CB (from WanderAIUpdate's table 0x00C53D30)

public:
	virtual UpdateSleepTime update();	// update interface slot 0, 0x0026E267 N:ledger (DozerAIUpdate::update 0x00489800 and WanderAIUpdate::update 0x004A9424 call it)
	virtual DisabledMaskType getDisabledTypesToProcess() const { return DisabledMaskType(DisabledMaskType::kInit, 3); }	// UpdateModuleInterface slot 1: retail 0x004A2D67, the shared DISABLED_HELD-only body (push 4; push 0; memset; or [esi],8)
protected:
	void wakeUpNow();	// 0x00262871 N:ledger
	void playAttackVoiceResponse(Object *);	// 0x0026b25a N:ledger
	void playAttackVoiceResponse(const Coord3D *);	// 0x0026b2fb N:ledger
	void playVoiceEnterStateAttackMove(const Coord3D *);	// 0x0026b37f N:ledger
	void playMoveVoiceResponse(const Coord3D *);	// 0x0026b403 N:ledger
	void setLastCommandSource(CommandSourceType);	// 0x003b23af N:ledger

	// Primary slots 13-153.
protected:
	virtual void privateMoveToPosition(const Coord3D *, Real, CommandSourceType);	// slot 13, N:ledger ?privateMoveToPosition@AIUpdateInterface@@MAEXPBUCoord3D@@MW4CommandSourceType@@@Z
	virtual void privateAttackMoveToPositionAndOrientation(const Coord3D *, Real, Int, CommandSourceType);	// slot 14, N:ledger ?privateAttackMoveToPositionAndOrientation@AIUpdateInterface@@MAEXPBUCoord3D@@MHW4CommandSourceType@@@Z
	virtual void bfmePrivateCommand01(Object *, CommandSourceType);	// slot 15, N:ledger ?bfmePrivateCommand01@AIUpdateInterface@@MAEXPAVObject@@W4CommandSourceType@@@Z
	virtual void privateMoveToPositionSA(const Coord3D *, CommandSourceType);	// slot 16, N:ledger ?privateMoveToPositionSA@AIUpdateInterface@@MAEXPBUCoord3D@@W4CommandSourceType@@@Z
	virtual void privateMoveToPositionAmphibious(const Coord3D *, Int, CommandSourceType);	// slot 17, N:ledger ?privateMoveToPositionAmphibious@AIUpdateInterface@@MAEXPBUCoord3D@@HW4CommandSourceType@@@Z
	virtual void bfmePrivateCommand48(Object *, CommandSourceType);	// slot 18, N:ledger ?bfmePrivateCommand48@AIUpdateInterface@@MAEXPAVObject@@W4CommandSourceType@@@Z
	virtual void privateMoveToAndEvacuate(const Coord3D *, CommandSourceType);	// slot 19, N:ledger ?privateMoveToAndEvacuate@AIUpdateInterface@@MAEXPBUCoord3D@@W4CommandSourceType@@@Z
	virtual void privateMoveToAndEvacuateAndExit(const Coord3D *, CommandSourceType);	// slot 20, N:ledger ?privateMoveToAndEvacuateAndExit@AIUpdateInterface@@MAEXPBUCoord3D@@W4CommandSourceType@@@Z
	virtual void bfmePrivateCommand38(const Coord3D *, CommandSourceType);	// slot 21, N:ledger ?bfmePrivateCommand38@AIUpdateInterface@@MAEXPBUCoord3D@@W4CommandSourceType@@@Z
	virtual void bfmePrivateCommand41(const Coord3D *, CommandSourceType);	// slot 22, N:ledger ?bfmePrivateCommand41@AIUpdateInterface@@MAEXPBUCoord3D@@W4CommandSourceType@@@Z
	virtual void bfmePrivateCommand42(Object *, CommandSourceType);	// slot 23, N:ledger ?bfmePrivateCommand42@AIUpdateInterface@@MAEXPAVObject@@W4CommandSourceType@@@Z
	virtual void privateIdle(CommandSourceType);	// slot 24, N:ledger ?privateIdle@AIUpdateInterface@@MAEXW4CommandSourceType@@@Z
	virtual void privateTightenToPosition(const Coord3D *, CommandSourceType);	// slot 25, N:ledger ?privateTightenToPosition@AIUpdateInterface@@MAEXPBUCoord3D@@W4CommandSourceType@@@Z
	virtual void privateFollowWaypointPath(const Waypoint *, CommandSourceType);	// slot 26, N:ledger ?privateFollowWaypointPath@AIUpdateInterface@@MAEXPBVWaypoint@@W4CommandSourceType@@@Z
	virtual void privateFollowWaypointPathAsTeam(const Waypoint *, CommandSourceType);	// slot 27, N:ledger ?privateFollowWaypointPathAsTeam@AIUpdateInterface@@MAEXPBVWaypoint@@W4CommandSourceType@@@Z
	virtual void privateFollowWaypointPathExact(const Waypoint *, CommandSourceType);	// slot 28, N:ledger ?privateFollowWaypointPathExact@AIUpdateInterface@@MAEXPBVWaypoint@@W4CommandSourceType@@@Z
	virtual void privateFollowWaypointPathAsTeamExact(const Waypoint *, CommandSourceType);	// slot 29, N:ledger ?privateFollowWaypointPathAsTeamExact@AIUpdateInterface@@MAEXPBVWaypoint@@W4CommandSourceType@@@Z
public:
protected:
	virtual void privateFollowPath(const _STL::vector<Coord3D> *path, Object *ignoreObject, CommandSourceType cmdSource, Bool exitProduction);	// slot 30, 0x0026BA84 (Zero Hour name and signature; the map spells the path type ?:placeholder Rva0035149F: it is AIStateMachine::m_goalPath's vector, 0x00351759 assigns it)
	virtual void bfmePrivateCommand24(const _STL::vector<Coord3D> *path, Object *ignoreObject, Real value, CommandSourceType cmdSource);	// slot 31, 0x0026BB5A: follows the path into state 0x36, the Real goes to +0x1A0
	virtual void bfmePrivateCommand25(const _STL::vector<Coord3D> *path, Object *ignoreObject, Real value, CommandSourceType cmdSource);	// slot 32, 0x0026BC16: the same into state 0x3D
protected:
	virtual void privateFollowPathAppend(const Coord3D *, CommandSourceType);	// slot 33, N:ledger ?privateFollowPathAppend@AIUpdateInterface@@MAEXPBUCoord3D@@W4CommandSourceType@@@Z
	virtual void privateAttackObject(Object *, Int, CommandSourceType);	// slot 34, N:ledger ?privateAttackObject@AIUpdateInterface@@MAEXPAVObject@@HW4CommandSourceType@@@Z
	virtual void bfmePrivateCommand39(Object *, CommandSourceType);	// slot 35, N:ledger ?bfmePrivateCommand39@AIUpdateInterface@@MAEXPAVObject@@W4CommandSourceType@@@Z
public:
	virtual void vslot036(void *, void *) {}	// slot 36, the shared `ret 8` 0x0050B238 (two dword arguments; their types are not proven)
	virtual void vslot037(void *, void *) {}	// slot 37, the shared `ret 8` 0x0050B238 (two dword arguments; their types are not proven)
protected:
	virtual void privateForceAttackObject(Object *, Int, CommandSourceType);	// slot 38, N:ledger ?privateForceAttackObject@AIUpdateInterface@@MAEXPAVObject@@HW4CommandSourceType@@@Z
	virtual void privateAttackTeam(const Team *, Int, CommandSourceType);	// slot 39, N:ledger ?privateAttackTeam@AIUpdateInterface@@MAEXPBVTeam@@HW4CommandSourceType@@@Z
	virtual void privateAttackPosition(const Coord3D *, Int, CommandSourceType);	// slot 40, N:ledger ?privateAttackPosition@AIUpdateInterface@@MAEXPBUCoord3D@@HW4CommandSourceType@@@Z
	virtual void bfmePrivateCommand49(Object *, CommandSourceType);	// slot 41, N:ledger ?bfmePrivateCommand49@AIUpdateInterface@@MAEXPAVObject@@W4CommandSourceType@@@Z
	virtual void privateAttackFollowWaypointPath(const Waypoint *, Int, Bool, CommandSourceType);	// slot 42, N:ledger ?privateAttackFollowWaypointPath@AIUpdateInterface@@MAEXPBVWaypoint@@H_NW4CommandSourceType@@@Z
	virtual void privateHunt(CommandSourceType);	// slot 43, N:ledger ?privateHunt@AIUpdateInterface@@MAEXW4CommandSourceType@@@Z
	// Slots 44 and 45: the base bodies are the shared `ret 8` 0x0050B238; DozerAIUpdate's
	// overrides are ?privateRepair@DozerAIUpdate@@MAEXPAVObject@@W4CommandSourceType@@@Z
	// 0x00488AC9 and ?privateResumeConstruction@DozerAIUpdate@@MAEXPAVObject@@W4CommandSourceType@@@Z
	// 0x00488B1A (WorkerAIUpdate's privateRepair 0x004A9BDE sits in slot 44 of its table too).
	virtual void privateRepair(Object *obj, CommandSourceType cmdSource);	// slot 44
	virtual void privateResumeConstruction(Object *obj, CommandSourceType cmdSource);	// slot 45
	virtual void privateGetHealed(Object *, CommandSourceType);	// slot 46, N:ledger ?privateGetHealed@AIUpdateInterface@@MAEXPAVObject@@W4CommandSourceType@@@Z
	virtual void privateGetRepaired(Object *, CommandSourceType);	// slot 47, N:ledger ?privateGetRepaired@AIUpdateInterface@@MAEXPAVObject@@W4CommandSourceType@@@Z
	virtual void privateEnter(Object *, CommandSourceType);	// slot 48, N:ledger ?privateEnter@AIUpdateInterface@@MAEXPAVObject@@W4CommandSourceType@@@Z
	virtual void bfmePrivateCommand3D(Object *, CommandSourceType);	// slot 49, N:ledger ?bfmePrivateCommand3D@AIUpdateInterface@@MAEXPAVObject@@W4CommandSourceType@@@Z
	virtual void privateMoveToObject(Object *, CommandSourceType);	// slot 50, N:ledger ?privateMoveToObject@AIUpdateInterface@@MAEXPAVObject@@W4CommandSourceType@@@Z
	virtual void privateDock(Object *, CommandSourceType);	// slot 51, N:ledger ?privateDock@AIUpdateInterface@@MAEXPAVObject@@W4CommandSourceType@@@Z
	virtual void privateHarvest(const Coord3D *, CommandSourceType);	// slot 52, N:ledger ?privateHarvest@AIUpdateInterface@@MAEXPBUCoord3D@@W4CommandSourceType@@@Z
	virtual void privateExitInstantly(Object *, CommandSourceType);	// slot 53, N:ledger ?privateExitInstantly@AIUpdateInterface@@MAEXPAVObject@@W4CommandSourceType@@@Z
	virtual void bfmePrivateCommand4B(Object *, const Coord3D *, CommandSourceType);	// slot 54, N:ledger ?bfmePrivateCommand4B@AIUpdateInterface@@MAEXPAVObject@@PBUCoord3D@@W4CommandSourceType@@@Z
public:
protected:
	virtual void bfmePrivateCommand4D(Object *, const _STL::vector<Coord3D> *, CommandSourceType);	// slot 55, 0x0026492A N:ledger (the map spells the path type ?:placeholder Rva0035149F); aiDoCommand 0x002673F6 case 0x4D
public:
protected:
	virtual void bfmePrivateCommand3E(Object *, CommandSourceType);	// slot 56, N:ledger ?bfmePrivateCommand3E@AIUpdateInterface@@MAEXPAVObject@@W4CommandSourceType@@@Z
	virtual void bfmePrivateCommand4A(Object *, const Coord3D *, CommandSourceType);	// slot 57, N:ledger ?bfmePrivateCommand4A@AIUpdateInterface@@MAEXPAVObject@@PBUCoord3D@@W4CommandSourceType@@@Z
public:
protected:
	virtual void bfmePrivateCommand4C(Object *, const _STL::vector<Coord3D> *, CommandSourceType);	// slot 58, 0x002649DC N:ledger (path type as slot 55)
public:
protected:
	virtual void bfmePrivateCommand54(Object *, const Coord3D *, CommandSourceType);	// slot 59, N:ledger ?bfmePrivateCommand54@AIUpdateInterface@@MAEXPAVObject@@PBUCoord3D@@W4CommandSourceType@@@Z
public:
	virtual void rva0026DDED(Int, CommandSourceType);	// slot 60, ?:placeholder ?rva0026DDED@AIUpdateInterface@@UAEXHW4CommandSourceType@@@Z
protected:
	virtual void privateGoProne(const DamageInfo *, CommandSourceType);	// slot 61, N:ledger ?privateGoProne@AIUpdateInterface@@MAEXPBVDamageInfo@@W4CommandSourceType@@@Z
	virtual void privateGuardTunnelNetwork(Int, CommandSourceType);	// slot 62, N:ledger ?privateGuardTunnelNetwork@AIUpdateInterface@@MAEXHW4CommandSourceType@@@Z
	virtual void privateGuardPosition(const Coord3D *, GuardMode, CommandSourceType);	// slot 63, N:ledger ?privateGuardPosition@AIUpdateInterface@@MAEXPBUCoord3D@@W4GuardMode@@W4CommandSourceType@@@Z
	virtual void privateGuardObject(Object *, GuardMode, CommandSourceType);	// slot 64, N:ledger ?privateGuardObject@AIUpdateInterface@@MAEXPAVObject@@W4GuardMode@@W4CommandSourceType@@@Z
public:
protected:
	virtual void bfmePrivateCommand20(const Team *, GuardMode, CommandSourceType);	// slot 65, 0x00264D81 N:ledger: aiDoCommand case 0x20 passes the parms' team; the body stores its id (+0x34) at +0x68 (the map spells the argument ?:placeholder Rva00264D81Arg)
public:
protected:
	virtual void privateGuardRetaliate(Object *, const Coord3D *, Int, CommandSourceType);	// slot 66, N:ledger ?privateGuardRetaliate@AIUpdateInterface@@MAEXPAVObject@@PBUCoord3D@@HW4CommandSourceType@@@Z
	virtual void privateGuardAreaFromPosition(const PolygonTrigger *, GuardMode, CommandSourceType, const Coord3D *);	// slot 67, N:ledger ?privateGuardAreaFromPosition@AIUpdateInterface@@MAEXPBVPolygonTrigger@@W4GuardMode@@W4CommandSourceType@@PBUCoord3D@@@Z
	virtual void privateAttackArea(const PolygonTrigger *, CommandSourceType);	// slot 68, N:ledger ?privateAttackArea@AIUpdateInterface@@MAEXPBVPolygonTrigger@@W4CommandSourceType@@@Z
	virtual void privateFaceObject(Object *, CommandSourceType);	// slot 69, N:ledger ?privateFaceObject@AIUpdateInterface@@MAEXPAVObject@@W4CommandSourceType@@@Z
	virtual void privateFacePosition(const Coord3D *, CommandSourceType);	// slot 70, N:ledger ?privateFacePosition@AIUpdateInterface@@MAEXPBUCoord3D@@W4CommandSourceType@@@Z
public:
	virtual void rva00262EFF(void *, void *, void *);	// slot 71, ?:placeholder ?rva00262EFF@AIUpdateInterface@@UAEXPAX00@Z
protected:
	virtual void privateCommandButton(const CommandButton *, CommandSourceType);	// slot 72, N:ledger ?privateCommandButton@AIUpdateInterface@@MAEXPBVCommandButton@@W4CommandSourceType@@@Z
	virtual void privateCommandButtonPosition(const CommandButton *, const Coord3D *, CommandSourceType);	// slot 73, N:ledger ?privateCommandButtonPosition@AIUpdateInterface@@MAEXPBVCommandButton@@PBUCoord3D@@W4CommandSourceType@@@Z
public:
	virtual void vslot074(void *, void *, void *) {}	// slot 74, the shared `ret 0xC` 0x000D1407 (three dword arguments; types not proven)
	virtual void bfmeGoCSG(void *, void *);	// slot 75, N:ledger ?bfmeGoCSG@AIUpdateInterface@@UAEXPAX0@Z
	virtual void bfmeGoCSA(void *);	// slot 76, N:ledger ?bfmeGoCSA@AIUpdateInterface@@UAEXPAX@Z
protected:
	virtual void bfmePrivateCommand53(CommandSourceType);	// slot 77, N:ledger ?bfmePrivateCommand53@AIUpdateInterface@@MAEXW4CommandSourceType@@@Z
public:
	virtual void bfmeGoCSH(void *, void *);	// slot 78, N:ledger ?bfmeGoCSH@AIUpdateInterface@@UAEXPAX0@Z
	virtual void bfmeGoCSD(void *, void *);	// slot 79, N:ledger ?bfmeGoCSD@AIUpdateInterface@@UAEXPAX0@Z
protected:
	virtual void bfmePrivateCommand31(Int, CommandSourceType);	// slot 80, N:ledger ?bfmePrivateCommand31@AIUpdateInterface@@MAEXHW4CommandSourceType@@@Z
	virtual void privateMoveAwayFromUnit(Object *, const Coord3D *, CommandSourceType);	// slot 81, N:ledger ?privateMoveAwayFromUnit@AIUpdateInterface@@MAEXPAVObject@@PBUCoord3D@@W4CommandSourceType@@@Z
	virtual void bfmePrivateCommand3C(Object *, CommandSourceType);	// slot 82, N:ledger ?bfmePrivateCommand3C@AIUpdateInterface@@MAEXPAVObject@@W4CommandSourceType@@@Z
public:
	virtual void bfmeGoCSE(void *, void *);	// slot 83, N:ledger ?bfmeGoCSE@AIUpdateInterface@@UAEXPAX0@Z
protected:
	virtual void rva002630FD(Int);	// slot 84, ?:placeholder ?rva002630FD@AIUpdateInterface@@MAEXH@Z
public:
	virtual void bfmeGoCSF(void *, void *);	// slot 85, N:ledger ?bfmeGoCSF@AIUpdateInterface@@UAEXPAX0@Z
protected:
	virtual void rva0026316B(void *, Int);	// slot 86, ?:placeholder ?rva0026316B@AIUpdateInterface@@MAEXPAXH@Z
	virtual void rva002631A6(Real, Int);	// slot 87, ?:placeholder ?rva002631A6@AIUpdateInterface@@MAEXMH@Z
public:
	virtual void vslot088() {}	// slot 88, the shared `ret` 0x000B3FD0
	virtual void vslot089() {}	// slot 89, the shared `ret` 0x000B3FD0
	virtual Bool vslot090() { return false; }	// slot 90, the shared `xor al,al; ret` 0x0047A699: SpecialAbilityUpdate 0x00450AE9 and 0x0044F28E test al after the call
	virtual Bool isBusy() const;	// slot 91, N:ledger ?isBusy@AIUpdateInterface@@UBE_NXZ
	// Slots 92-102: the per-class interface accessors (Zero Hour names). Each pair
	// is one accessor declared non-const then const, which MSVC lays out const
	// first. Overrides: DozerAIUpdate and WorkerAIUpdate 92/93 (+0x3E4),
	// SupplyTruckAIUpdate 94/95 (+0x3E4; WorkerAIUpdate +0x3E8), AnimalAIUpdate 96,
	// GiantBirdAIUpdate 97/98 (`return this`), WorkerAIUpdate 99/100 (+0x3EC),
	// AssaultTransportAIUpdate 101/102 (+0x3E4). The dock states call slot 95.
	virtual DozerAIInterface *getDozerAIInterface() { return 0; }	// slot 93
	virtual const DozerAIInterface *getDozerAIInterface() const { return 0; }	// slot 92
	virtual SupplyTruckAIInterface *getSupplyTruckAIInterface() { return 0; }	// slot 95
	virtual const SupplyTruckAIInterface *getSupplyTruckAIInterface() const { return 0; }	// slot 94
	virtual AnimalAIInterface *getAnimalAIInterface() { return 0; }	// slot 96 (role name, see AnimalAIUpdate.h)
	virtual GiantBirdAIUpdate *vslot097() { return 0; }	// slot 97, the shared `xor eax,eax; ret` 0x000D43D0; GiantBirdAIUpdate returns this
	virtual AIUpdateInterface *vslot098() { return 0; }	// slot 98, the aircraft receiver 0x003A4F21 (shared `return 0` body 0x000D43D0)
	virtual WorkerAIInterface *getWorkerAIInterface() { return 0; }	// slot 100
	virtual const WorkerAIInterface *getWorkerAIInterface() const { return 0; }	// slot 99
	virtual AssaultTransportAIInterface *getAssaultTransportAIInterface() { return 0; }	// slot 102
	virtual const AssaultTransportAIInterface *getAssaultTransportAIInterface() const { return 0; }	// slot 101
	virtual void joinTeam();	// slot 103, N:ledger ?joinTeam@AIUpdateInterface@@UAEXXZ
	virtual Int rva0026E988();	// slot 104, ?:placeholder ?rva0026E988@AIUpdateInterface@@UAEHXZ
	virtual Int rva0026E999();	// slot 105, ?:placeholder ?rva0026E999@AIUpdateInterface@@UAEHXZ
	virtual AIFreeToExitType getAiFreeToExit(const Object *exiter) const;	// slot 106, the shared `xor eax,eax; ret 4` 0x005DAA6C (FREE_TO_EXIT); TransportContain::isSpecificRiderFreeToExit 0x004671AC calls it with the rider and tests the result (Zero Hour's name)
	virtual Bool isAllowedToAdjustDestination() const;	// slot 107, the shared `return true` 0x0050B5C6; AIInternalMoveToState::getAdjustsDestination 0x00344138 asks it (Zero Hour's name)
	virtual Bool vslot108() { return true; }	// slot 108, the shared `mov al,1; ret` 0x0050B5C6
	virtual Int vslot109() { return 0; }	// slot 109, the shared `xor eax,eax; ret` 0x000D43D0
	virtual Bool isIdle() const;	// slot 110, 0x002645FF
	virtual Bool rva00262D8F() const;	// slot 111, ?:placeholder ?rva00262D8F@AIUpdateInterface@@UBE_NXZ
	virtual Bool isClearingMines() const;	// slot 112, N:ledger ?isClearingMines@AIUpdateInterface@@UBE_NXZ
	virtual Bool rva00262DA3() const;	// slot 113, ?:placeholder ?rva00262DA3@AIUpdateInterface@@UBE_NXZ
	virtual Bool rva00262DAB() const;	// slot 114, ?:placeholder ?rva00262DAB@AIUpdateInterface@@UBE_NXZ
	virtual Bool rva00262DBF() const;	// slot 115, ?:placeholder ?rva00262DBF@AIUpdateInterface@@UBE_NXZ
	virtual Bool rva00262D7B() const;	// slot 116, ?:placeholder ?rva00262D7B@AIUpdateInterface@@UBE_NXZ
	virtual void doQuickExit(const _STL::vector<Coord3D> *);	// slot 117, N:ledger ?doQuickExit@AIUpdateInterface@@UAEXPBV?$vector@UCoord3D@@V?$allocator@UCoord3D@@@_STL@@@_STL@@@Z
	virtual const Coord3D *vslot118() const { return &m_locationToGuard; }	// slot 118, folded `lea eax,[ecx+58h]; ret` 0x00042287
	virtual ObjectID vslot119() const { return m_objectToGuard; }	// slot 119, folded `mov eax,[ecx+64h]; ret` 0x002004A1
	virtual Int vslot120() const { return m_unk0x68; }	// slot 120, folded `mov eax,[ecx+68h]; ret` 0x00367508 (the guarded team's id, bfmePrivateCommand20 0x00264D81)
	virtual const PolygonTrigger *vslot121() const { return m_areaToGuard; }	// slot 121, folded `mov eax,[ecx+6Ch]; ret` 0x0036750C
	virtual void vslot122(const PolygonTrigger *area) { m_areaToGuard = area; }	// slot 122, folded store to +0x6C 0x0033F15D
	virtual Int vslot123() const { return m_guardTargetType[1]; }	// slot 123, folded `mov eax,[ecx+54h]; ret` 0x0009AAA4
	virtual void clearGuardTargetType();	// slot 124, N:ledger ?clearGuardTargetType@AIUpdateInterface@@UAEXXZ
	virtual Int vslot125() const { return m_unk0x4C; }	// slot 125, folded `mov eax,[ecx+4Ch]; ret` 0x002A98A7 (the guard mode: AIStates casts it)
	virtual Object *construct(const ThingTemplate *, const Coord3D *, Real, Player *, Bool, Int);	// slot 126, N:ledger ?construct@AIUpdateInterface@@UAEPAVObject@@PBVThingTemplate@@PBUCoord3D@@MPAVPlayer@@_NH@Z
	virtual Object *getEnterTarget();	// slot 127, N:ledger ?getEnterTarget@AIUpdateInterface@@UAEPAVObject@@XZ
	virtual Bool getSneakyTargetingOffset(Coord3D *offset) const;	// slot 128, the shared `xor al,al; ret 4` 0x005CB9FF; WeaponTemplate::shouldProjectileCollideWith 0x002CBA7C passes NULL and tests al (Zero Hour's name)
	virtual void vslot129(ObjectID attackerID, Int unk) {}	// slot 129, the shared `ret 8` 0x0050B238; setCurrentVictim 0x00268D1F tells the old victim's AI the attacker's ID and 0
	virtual Bool vslot130() { return false; }	// slot 130, the shared `xor al,al; ret` 0x0047A699 (folded with slot 90)
	virtual void setLocomotorGoalPositionOnPath();	// slot 131, N:ledger ?setLocomotorGoalPositionOnPath@AIUpdateInterface@@UAEXXZ
	virtual void setLocomotorGoalPositionExplicit(const Coord3D &);	// slot 132, N:ledger ?setLocomotorGoalPositionExplicit@AIUpdateInterface@@UAEXABUCoord3D@@@Z
	virtual void setLocomotorGoalPositionExplicitSmart(const Coord3D &);	// slot 133, N:ledger ?setLocomotorGoalPositionExplicitSmart@AIUpdateInterface@@UAEXABUCoord3D@@@Z
	virtual void appendPositionToLocomotorPath(const Coord3D &, Int);	// slot 134, N:ledger ?appendPositionToLocomotorPath@AIUpdateInterface@@UAEXABUCoord3D@@H@Z
	virtual void setLocomotorGoalOrientation(Real);	// slot 135, N:ledger ?setLocomotorGoalOrientation@AIUpdateInterface@@UAEXM@Z
	virtual void setLocomotorGoalNone();	// slot 136, N:ledger ?setLocomotorGoalNone@AIUpdateInterface@@UAEXXZ
	virtual Bool isDoingGroundMovement() const;	// slot 137, N:ledger ?isDoingGroundMovement@AIUpdateInterface@@UBE_NXZ
	virtual Bool rva00262BA9();	// slot 138, ?:placeholder ?rva00262BA9@AIUpdateInterface@@UAE_NXZ
	virtual void micropathToPosition(const Coord3D *);	// slot 139, N:ledger ?micropathToPosition@AIUpdateInterface@@UAEXPBUCoord3D@@@Z
	virtual void doPathfind(PathfindServicesInterface *);	// slot 140, N:ledger ?doPathfind@AIUpdateInterface@@UAEXPAVPathfindServicesInterface@@@Z
	virtual void rva00263404(Rva00263404Source *source);	// slot 141: calls its argument's virtual slot 6 and works on a path; the role is not established, so the ledger's own name stays
	virtual Bool chooseLocomotorSet(Int);	// slot 142, N:ledger ?chooseLocomotorSet@AIUpdateInterface@@UAE_NH@Z
	virtual CommandSourceType vslot143() const { return m_lastCommandSource; }	// slot 143, folded `mov eax,[ecx+48h]; ret` 0x005C4AFD
	virtual void vslot144() {}	// slot 144, the shared `ret` 0x000B3FD0
	virtual void rva00267E46();	// slot 145, ?:placeholder ?rva00267E46@AIUpdateInterface@@UAEXXZ
	virtual Bool vslot146() { return true; }	// slot 146, the shared `mov al,1; ret` 0x0050B5C6
	virtual void vslot147() {}	// slot 147, the shared `ret` 0x000B3FD0
	virtual Bool isAllowedToRespondToAiCommands(const AICommandParms *) const;	// slot 148, N:ledger ?isAllowedToRespondToAiCommands@AIUpdateInterface@@UBE_NPBUAICommandParms@@@Z
protected:
	virtual UpdateSleepTime doLocomotor();	// slot 149, N:ledger ?doLocomotor@AIUpdateInterface@@MAE?AW4UpdateSleepTime@@XZ
	virtual AIStateMachine *makeStateMachine();	// slot 150, N:ledger ?makeStateMachine@AIUpdateInterface@@MAEPAVAIStateMachine@@XZ
public:
	virtual Bool getTreatAsAircraftForLocoDistToGoal() const;	// slot 151, N:ledger ?getTreatAsAircraftForLocoDistToGoal@AIUpdateInterface@@UBE_NXZ
	virtual Bool rva002697FC();	// slot 152, ?:placeholder ?rva002697FC@AIUpdateInterface@@UAE_NXZ
	virtual void vslot153();	// slot 153, N:ledger ?vslot153@AIUpdateInterface@@UAEXXZ

	UnsignedInt m_priorWaypointID;				// +0x028, 0xFACADE
	UnsignedInt m_currentWaypointID;			// +0x02C, 0xFACADE
	AIStateMachine *m_stateMachine;				// +0x030, xfer'd as a snapshot
	AIStateMachine *m_unk0x34;					// +0x034, recreated through makeStateMachine on load when set
	AIStateMachine *m_unk0x38;					// +0x038, the same
	UnsignedInt m_nextEnemyScanTime;			// +0x03C
	ObjectID m_currentVictimID;					// +0x040
	Real m_desiredSpeed;						// +0x044, constant 0x00BF9060
	CommandSourceType m_lastCommandSource;		// +0x048, 2 (CMD_FROM_AI)
	UnsignedInt m_unk0x4C;						// +0x04C, xfer'd as 4 raw bytes
	Int m_guardTargetType[2];					// +0x050, both 4; one helper (0x00263569) xfers the pair
	Coord3D m_locationToGuard;					// +0x058
	ObjectID m_objectToGuard;					// +0x064
	UnsignedInt m_unk0x68;						// +0x068
	const PolygonTrigger *m_areaToGuard;		// +0x06C, xfer'd by name
	const AttackPriorityInfo *m_attackInfo;		// +0x070, ScriptEngine::getAttackInfo
	Coord3D m_waypointQueue[16];				// +0x074, built with the vector constructor iterator
	Int m_waypointCount;						// +0x134
	Int m_waypointIndex;						// +0x138
	const Waypoint *m_completedWaypoint;		// +0x13C, xfer'd by ID
	Path *m_path;								// +0x140, new 0x28 bytes on load
	ObjectID m_requestedVictimID;				// +0x144
	Coord3D m_requestedDestination;				// +0x148
	Coord3D m_requestedDestination2;			// +0x154
	UnsignedInt m_unk0x160;						// +0x160, constructor zero, not xfer'd
	ObjectID m_ignoreObstacleID;				// +0x164
	Real m_pathExtraDistance;					// +0x168
	UnsignedInt m_unk0x16C;						// +0x16C
	Real m_unk0x170;							// +0x170
	Real m_unk0x174;							// +0x174, constant 0x00BF9060
	UnsignedInt m_unk0x178;						// +0x178
	UnsignedInt m_unk0x17C;						// +0x17C
	Coord3D m_unk0x180;							// +0x180
	ObjectID m_unk0x18C;						// +0x18C
	ObjectID m_unk0x190;						// +0x190
	Int m_unk0x194;								// +0x194, -1
	ObjectID m_unk0x198;						// +0x198
	ObjectID m_unk0x19C;						// +0x19C
	Real m_unk0x1A0;							// +0x1A0, constant 0x00BBB8E0
	UnsignedInt m_unk0x1A4;						// +0x1A4
	ObjectID m_recentCollisionIDs[4];			// +0x1A8, role name: rva00262989 0x00262989 records a collider here; onCollide 0x0026DF0D searches it
	UnsignedInt m_recentCollisionFrames[4];		// +0x1B8, the frame of each record (rva00262989 overwrites the oldest when full)
	UnsignedInt m_recentCollisionCount;			// +0x1C8, records in use (at most 4)
	LocomotorSet m_locomotorSet;				// +0x1CC (ctor 0x001E7087, xferSelfAndCurLocoPtr 0x001E70C2)
	Locomotor *m_curLocomotor;					// +0x1F0
	Int m_curLocomotorSet;						// +0x1F4, LocomotorSetType, -1
	Real m_unk0x1F8;							// +0x1F8
	UnsignedInt m_unk0x1FC;						// +0x1FC, xfer'd as 4 raw bytes
	Coord3D m_unk0x200;							// +0x200
	TurretAI *m_turretAI[1];					// +0x20C, new 0x40 bytes when the data names a turret; Zero Hour's m_turretAI[MAX_TURRETS] with one turret (the wrappers index it: [this+tur*4+0x20C], 0x002625E6)
	Int m_turretSyncFlag;						// +0x210, WhichTurretType, -1
	AsciiString m_attackInfoName;				// +0x214: the attack-priority set's name; loadPostProcess 0x002686A4 tests it (isEmpty) and looks m_attackInfo up by it
	Int m_attitude;								// +0x218, AttitudeType
	UnsignedInt m_unk0x21C;						// +0x21C
	void *m_unk0x220;							// +0x220, the LuaScriptEngine record found by name (rva00337DEF); xfer 0x00267EDD saves the record's first member, its AsciiString name
	LuaSpyData m_luaSpyData;					// +0x224 (ctor 0x0026E7C8; ~AIUpdateInterface 0x0026E836 destroys it through 0x0026E7EA)
	UnsignedInt m_unk0x234;						// +0x234
	ObjectID m_crateCreated;					// +0x238, AITNGuardIdleState::update 0x0054665B tests it (donor name)
	UnsignedInt m_unk0x23C;						// +0x23C
	UnsignedInt m_unk0x240;						// +0x240
	BitFlags<591> m_unk0x244;					// +0x244, model conditions (xfer 0x268476 calls BitFlags<591>::xfer 0x000BB710)
	BitFlags<591> m_unk0x290;					// +0x290, model conditions (xfer 0x268482 calls 0x000BB710)
	ObjectStatusMaskType m_unk0x2DC;			// +0x2DC, BitFlags<101> (reset 0x0024CA24; xfer 0x2685F7 calls BitFlags<101>::xfer 0x00292550)
	AIStoredCommand m_mostRecentCommand;		// +0x2EC (Zero Hour's member name; ctor 0x0026AFDA, xfer 0x003533BF; ~AIUpdateInterface 0x0026E836 destroys it first through 0x0026B03D)
	Bool m_unk0x3B0;							// +0x3B0
	Bool m_waitingForPath;						// +0x3B1, AIInternalMoveToState::computePath 0x003441F7 copies it after requestPath
	Bool m_unk0x3B2;							// +0x3B2
	Bool m_unk0x3B3;							// +0x3B3
	Bool m_unk0x3B4;							// +0x3B4
	Bool m_unk0x3B5;							// +0x3B5
	Bool m_unk0x3B6;							// +0x3B6
	Bool m_unk0x3B7;							// +0x3B7, a path request is pending
	Bool m_unk0x3B8;							// +0x3B8, the move/face command handlers clear it with +0x16C
	Bool m_upgradedLocomotors;					// +0x3B9 (Zero Hour's name): setLocomotorUpgrade 0x00262804 stores it, chooseLocomotorSet 0x00268B20 reads it
	Bool m_canPathThroughUnits;					// +0x3BA, AIDockState::update 0x00341770 sets it, onExit 0x00341723 clears it
	Bool m_unk0x3BB;							// +0x3BB
	Bool m_unk0x3BC;							// +0x3BC
	Bool m_isAiDead;							// +0x3BD, markAsDead 0x0026296D sets it; InstantDeathBehavior::onDie 0x0045CF9B tests it
	Bool m_isRecruitable;						// +0x3BE, starts true: the "AI Recruitable" object flag of ScriptActions' flag table sets it, and the team-stop-and-disband row 0x003C08CC sets it on every member (Zero Hour's AIUpdateInterface::setIsRecruitable)
	Bool m_unk0x3BF;							// +0x3BF
	Bool m_unk0x3C0;							// +0x3C0
	Bool m_unk0x3C1;							// +0x3C1, constructor zero (member initialiser, 0x0026E9BD)
	Bool m_unk0x3C2;							// +0x3C2
	Bool m_unk0x3C3;							// +0x3C3
	Bool m_unk0x3C4;							// +0x3C4
	Bool m_allowStateChange;					// +0x3C5
	Bool m_unk0x3C6;							// +0x3C6
	Bool m_unk0x3C7;							// +0x3C7
	Bool m_unk0x3C8;							// +0x3C8, starts true
	Bool m_flag3C9;								// +0x3C9 flee-in-progress flag
	Bool m_unk0x3CA;							// +0x3CA
	Bool m_unk0x3CB;							// +0x3CB
	Bool m_unk0x3CC;							// +0x3CC
	Bool m_unk0x3CD;							// +0x3CD, constructor zero (member initialiser)
	Bool m_unk0x3CE;							// +0x3CE, constructor zero; update 0x0026E267 sets it for kind 0x1BA
	Bool m_unk0x3CF;							// +0x3CF, constructor zero (member initialiser)
	ScriptTracker m_scriptTracker;				// +0x3D0, constructed with this (0x00262436)
	ObjectID m_unk0x3DC;						// +0x3DC
	Bool m_unk0x3E0;							// +0x3E0
};
// SIZE 0x3E4: ModuleFactory allocates 0x3E4 for AIUpdate; every derived AI update puts its first member or interface vptr at +0x3E4.
typedef char AIUpdateInterfaceSize[(sizeof(AIUpdateInterface) == 0x3E4) ? 1 : -1];
typedef char AIUpdateInterface_off_m_luaSpyData[(offsetof(AIUpdateInterface, m_luaSpyData) == 0x224) ? 1 : -1];
typedef char AIUpdateInterface_off_m_mostRecentCommand[(offsetof(AIUpdateInterface, m_mostRecentCommand) == 0x2EC) ? 1 : -1];
typedef char AIUpdateInterface_off_m_curLocomotor[(offsetof(AIUpdateInterface, m_curLocomotor) == 0x1F0) ? 1 : -1];
typedef char AIUpdateInterface_off_m_scriptTracker[(offsetof(AIUpdateInterface, m_scriptTracker) == 0x3D0) ? 1 : -1];

#endif
