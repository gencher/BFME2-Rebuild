// AI.h: the AI subsystem singleton (TheAI, VA 0x00DFF0F8) and its INI data.
// Target facts:
//  - AI: bases SubsystemInterface +0x00 and Snapshot +0x0C (ctor 0x002FEA85
//    stores Snapshot's table at +0x0C, then 0x00C073D0/0x00C073C0). SIZE 0x24
//    (GameEngine::init allocates 0x24 before calling the ctor at 0x0022F3BE).
//    The ctor builds, in order: +0x10 the Pathfinder (new 0x1D200), +0x14 a
//    _List_base, +0x18 the TAiData (new 0x114), +0x1C and +0x20 zeroed; init
//    0x002FD7D7 clears +0x1C. Zero Hour's AI has the same member order, which
//    supplies the names.
//  - TAiData: Snapshot-derived (table 0x00C071E4: deleting dtor, an empty
//    loadPostProcess, GetSnapshotName 0x002FE03B, xfer 0x002FDD94). SIZE 0x114.
//     - Members +0x04..+0x110: the AIData parse table (VA 0x00C06650, 74 entries),
//    each at its entry's offset with
//    the type its parse function writes, named from the INI token. The three
//    offset-0 entries (SideInfo, AttackPriority, SkirmishBuildList) are
//    sub-block parsers, not members. +0xF4..+0x100 are four words the ctor
//    zeroes; addSideInfo 0x002FD73B pushes onto the list headed at +0xF4.
#pragma once
#ifndef BFME2_AI_H
#define BFME2_AI_H

#include "Common/GameType.h"
#include "Common/Science.h"
#include "Common/AsciiString.h"
#include "Common/Snapshot.h"
#include "Common/StlList.h"
#include "GameLogic/AIAttackPrioritySet.h"
#include "Common/SubsystemInterface.h"

class Pathfinder;
class AIGroup;
class Object;
class Xfer;
class INI;
class AttackPriorityInfo;
class PartitionFilter;

enum { MAX_AI_UPGRADES = 20 };
// Native parseScience indexes twenty 4-byte sciences after the count;
// Zero Hour AI.h supplies this skill-set declaration and names.
struct TSkillSet
{
    Int m_numSkills;
    ScienceType m_skills[MAX_AI_UPGRADES];
};

class TAiData : public Snapshot
{
public:
	TAiData();				// 0x002FE382
	virtual ~TAiData();			// 0x002FDF72

protected:
	virtual void loadPostProcess() {}	// slot 1, the shared empty body 0x000B3FD0
public:	// retail: ?GetSnapshotName@TAiData@@UBEPBDXZ (public), ?xfer@TAiData@@MAEXPAVXfer@@@Z (protected)
	virtual const char *GetSnapshotName() const;	// slot 2, 0x002FE03B
protected:
	virtual void xfer(Xfer *xfer);			// slot 3, 0x002FDD94

public:
	Real m_structureSeconds;                               // +0x4 StructureSeconds
	Real m_teamSeconds;                                    // +0x8 TeamSeconds
	Int m_wealthy;                                         // +0xC Wealthy
	Int m_poor;                                            // +0x10 Poor
	UnsignedInt m_forceIdleMSEC;                           // +0x14 ForceIdleMSEC; duration, frames
	Real m_structuresWealthyRate;                          // +0x18 StructuresWealthyRate
	Real m_teamsWealthyRate;                               // +0x1C TeamsWealthyRate
	Real m_structuresPoorRate;                             // +0x20 StructuresPoorRate
	Real m_teamsPoorRate;                                  // +0x24 TeamsPoorRate
	Real m_teamResourcesToStart;                           // +0x28 TeamResourcesToStart
	Real m_guardInnerModifierAI;                           // +0x2C GuardInnerModifierAI
	Real m_guardOuterModifierAI;                           // +0x30 GuardOuterModifierAI
	Real m_guardInnerModifierHuman;                        // +0x34 GuardInnerModifierHuman
	Real m_guardOuterModifierHuman;                        // +0x38 GuardOuterModifierHuman
	UnsignedInt m_guardChaseUnitsDuration;                 // +0x3C GuardChaseUnitsDuration; duration, frames
	UnsignedInt m_guardEnemyScanRate;                      // +0x40 GuardEnemyScanRate; duration, frames
	UnsignedInt m_guardEnemyReturnScanRate;                // +0x44 GuardEnemyReturnScanRate; duration, frames
	Real m_wallHeight;                                     // +0x48 WallHeight
	Real m_alertRangeModifier;                             // +0x4C AlertRangeModifier
	Real m_aggressiveRangeModifier;                        // +0x50 AggressiveRangeModifier
	Real m_attackPriorityDistanceModifier;                 // +0x54 AttackPriorityDistanceModifier
	Real m_skirmishGroupFudgeDistance;                     // +0x58 SkirmishGroupFudgeDistance
	Real m_maxRecruitRadius;                               // +0x5C MaxRecruitRadius
	Real m_repulsedDistance;                               // +0x60 RepulsedDistance
	Bool m_enableRepulsors;                                // +0x64 EnableRepulsors
	Bool m_forceSkirmishAI;                                // +0x65 ForceSkirmishAI
	Bool m_rotateSkirmishBases;                            // +0x66 RotateSkirmishBases
	Bool m_attackUsesLineOfSight;                          // +0x67 AttackUsesLineOfSight
	Bool m_attackIgnoreInsignificantBuildings;             // +0x68 AttackIgnoreInsignificantBuildings
	Real m_minDistanceForGroup;                            // +0x6C MinDistanceForGroup
	Real m_formationEnemyDistance;                         // +0x70 FormationEnemyDistance
	Real m_minClumpDensity;                                // +0x74 MinClumpDensity
	Int m_infantryPathfindDiameter;                        // +0x78 InfantryPathfindDiameter
	Int m_vehiclePathfindDiameter;                         // +0x7C VehiclePathfindDiameter
	Int m_rebuildDelayTimeSeconds;                         // +0x80 RebuildDelayTimeSeconds
	Real m_supplyCenterSafeRadius;                         // +0x84 SupplyCenterSafeRadius
	Real m_aiDozerBoredRadiusModifier;                     // +0x88 AIDozerBoredRadiusModifier
	Bool m_aiCrushesInfantry;                              // +0x8C AICrushesInfantry
	Real m_meleeApproachTolerance;                         // +0x90 MeleeApproachTolerance
	Real m_meleeApproachDist;                              // +0x94 MeleeApproachDist
	Real m_wadeWaterDepth;                                 // +0x98 WadeWaterDepth
	Real m_deepWaterDepth;                                 // +0x9C DeepWaterDepth
	Real m_formationColumnWidth;                           // +0xA0 FormationColumnWidth
	Real m_formationRowDepth;                              // +0xA4 FormationRowDepth
	Real m_formationSquadSpacing;                          // +0xA8 FormationSquadSpacing
	Real m_narrowPassageScale;                             // +0xAC NarrowPassageScale
	Real m_minFlightHeight;                                // +0xB0 MinFlightHeight
	Int m_formationColumns;                                // +0xB4 FormationColumns
	Bool m_waitForOthers;                                  // +0xB8 WaitForOthers
	Bool m_hordesWaitForHordes;                            // +0xB9 HordesWaitForHordes
	Bool m_attackMoveUsesFormations;                       // +0xBA AttackMoveUsesFormations
	Bool m_forceHordesToLowLOD;                            // +0xBB ForceHordesToLowLOD
	Bool m_allowForestFires;                               // +0xBC AllowForestFires
	Bool m_useFormations;                                  // +0xBD UseFormations
	Real m_altCameraZoomOverride;                          // +0xC0 AltCameraZoomOverride
	Real m_altCameraPitchOverride;                         // +0xC4 AltCameraPitchOverride
	Real m_maxRetaliateDistance;                           // +0xC8 MaxRetaliateDistance
	Real m_retaliateFriendsRadius;                         // +0xCC RetaliateFriendsRadius
	Real m_chaseFromBehindLimit;                           // +0xD0 ChaseFromBehindLimit
	Real m_castleSiegeStandBackDistance;                   // +0xD4 CastleSiegeStandBackDistance
	Bool m_useLowLODTrees;                                 // +0xD8 UseLowLODTrees
	AsciiString m_lowLodTreeName;                          // +0xDC LowLodTreeName
	AsciiString m_lowLodTreeNameNoGrab;                    // +0xE0 LowLodTreeNameNoGrab
	AsciiString m_lowLodTreeNameNoHarvest;                 // +0xE4 LowLodTreeNameNoHarvest
	Real m_lowLodTreeScale;                                // +0xE8 LowLodTreeScale
	Bool m_disableTrees;                                   // +0xEC DisableTrees
	Real m_artilleryMaxFriendlyFire;                       // +0xF0 ArtilleryMaxFriendlyFire; percent
	unsigned char m_unk0xF4[8];
	AIAttackPrioritySet *m_attackPrioritySets;			// +0xFC head of the attack-priority sets of the AttackPriority INI block (ScriptEngine::getAttackInfo 0x00205DDD walks it)
	TAiData *m_next;                                      // +0x100, the override chain AI::reset 0x002FEB3A unwinds
	Int m_buildPhase1PerSecondPriorityModifier;            // +0x104 BuildPhase1_PerSecondPriorityModifier
	Int m_buildPhase2PerSecondPriorityModifier;            // +0x108 BuildPhase2_PerSecondPriorityModifier
	Int m_buildPhase3PerSecondPriorityModifier;            // +0x10C BuildPhase3_PerSecondPriorityModifier
	Int m_buildPhaseNPerSecondPriorityModifier;            // +0x110 BuildPhaseN_PerSecondPriorityModifier
};
// SIZE 0x114

class AI : public SubsystemInterface, public Snapshot
{
public:
	AI();					// 0x002FEA85
	virtual ~AI();				// 0x002FEBB7

	virtual void reset();			// slot 9, 0x002FEB3A
	virtual void update();			// slot 10, 0x002FD7DC
	virtual void init();			// slot 1, 0x002FD7D7

protected:
	virtual void loadPostProcess() {}	// Snapshot slot 1, the shared empty body 0x000B3FD0
public:	// retail: ?GetSnapshotName@AI@@UBEPBDXZ and ?xfer@AI@@UAEXPAVXfer@@@Z are public
	virtual const char *GetSnapshotName() const;	// Snapshot slot 2, 0x002FEB2C
	virtual void xfer(Xfer *xfer);			// Snapshot slot 3, 0x002FE799

public:
	static void parseScience(INI *, void *, void *, const void *);
	static void parseSkillSet(INI *, void *, void *, const void *);
	static void parseAiDataDefinition(INI *ini);	// 0x002FFF72
	void newOverride();			// 0x002FFDC3
	AIGroup *createGroup();			// 0x002FEC4B
	void destroyGroup(AIGroup *group);	// 0x002FE712
	enum
	{
		CAN_SEE = 1 << 0,
		CAN_ATTACK = 1 << 1,
		IGNORE_INSIGNIFICANT_BUILDINGS = 1 << 2,
		ATTACK_BUILDINGS = 1 << 3,
		WITHIN_ATTACK_RANGE = 1 << 4,
		UNFOGGED = 1 << 5
	};
	Object *findClosestEnemy(const Object *me, Real range, UnsignedInt qualifiers, const AttackPriorityInfo *info, PartitionFilter *optionalFilter, Int arg);	// 0x002FFFA3
	Object *findClosestAlly(const Object *me, Real range, UnsignedInt flags);	// 0x002FDBC4
	Object *findClosestRepulsor(const Object *me, Real range);	// 0x002FDC9A
	static Real getAdjustedVisionRangeForObject(const Object *obj, Int flags);	// 0x002FDD0A
	static Bool isTargetInWeaponRange(Object *source, Object *target);	// 0x002FE193 (map ?rva002FE193@AI@@SA_NPAVObject@@0@Z, ?:placeholder; AIGuardOuterState::onEnter 0x005439F2)

	UnsignedInt getNextGroupID() { return ++m_nextGroupID; }	// inline (AIGroup::AIGroup 0x0036E4C7)
	Pathfinder *pathfinder() { return m_pathfinder; }
	const TAiData *getAiData() const { return m_aiData; }

	Pathfinder *m_pathfinder;		// +0x10
	_STL::list<AIGroup *> m_groupList;	// +0x14
	TAiData *m_aiData;			// +0x18
	UnsignedInt m_nextGroupID;		// +0x1C init clears it
	UnsignedInt m_nextFormationID;		// +0x20
};
// SIZE 0x24

extern AI *TheAI;

#endif
