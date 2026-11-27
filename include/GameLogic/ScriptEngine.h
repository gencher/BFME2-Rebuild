// ScriptEngine.h: the map-script subsystem (TheScriptEngine, VA 0x00DFE16C). SIZE 0x1A500.
// Target facts:
//  - Bases SubsystemInterface +0x00 and Snapshot +0x0C: ctor 0x00208343 calls
//    SubsystemInterface(), stores Snapshot's table at +0x0C, then its own
//    tables 0x00BE3D70 (+0x00, 14 slots) and 0x00BE3D60 (+0x0C, 4 slots).
//  - Size 0x1A500: GameEngine::init allocates 0x1A500 before the ctor
//    (call at 0x0022F529).
//  - Primary slots 1 init 0x00205AFD, 9 reset 0x00209ABE, 10 update
//    0x0020D065; it adds no slots of its own. Snapshot slots: 0x002086BD
//    (deleting thunk), loadPostProcess 0x0020530E, GetSnapshotName 0x002086B7,
//    xfer 0x0020A859.
//  - Members are not recovered yet: the ctor builds a vector at +0x10 and a
//    0x257-element array of 0x80-byte records next; everything after the bases
//    is one gap of the measured size.
#pragma once
#ifndef BFME2_SCRIPTENGINE_H
#define BFME2_SCRIPTENGINE_H

#include <stddef.h>
#include <map>
#include "Common/GameType.h"
#include "Common/AsciiString.h"
#include "Common/Snapshot.h"
#include "Common/Coord3D.h"
#include "Common/Coord2D.h"
#include "Common/SubsystemInterface.h"
#include "Common/Vector.h"
#include "Common/StlList.h"
#include "Common/Science.h"
#include "GameLogic/ScriptTemplate.h"
#include "GameLogic/ScriptCounter.h"
#include "GameLogic/AttackPriorityInfo.h"

class Object;
class ObjectTypes;
class Template;
class ActionTemplate;
class ConditionTemplate;
class Parameter;
class ScriptAction;
class Condition;
class ScriptActions;
class ScriptConditions;
class Player;
class SequentialScript;
class ScriptList;
struct ScriptLink;
struct ScriptGroupLink;
class PolygonTrigger;
class Team;
class TeamPrototype;
class Script;
class ScriptGroup;
class ScriptList;
class Xfer;
class INI;

// One entry of the per-player progress lists: a name key (Rva003ECA13Get of the script's name)
// and a value (the source object's id for the "from unit" queries).
struct ScriptProgressRecord
{
	UnsignedInt m_key;	// 0x0
	Int m_value;		// 0x4
};

// One timed entry of the speech and audio completion lists: the name key and the logic frame the clip ends at.
struct ScriptTimedKey
{
	UnsignedInt m_key;		// 0x0 Rva003ECA13Get of the clip's name
	UnsignedInt m_readyFrame;	// 0x4
};

// One named map-reveal record (0x14 bytes): the name key leads; the rest is not established.
struct NamedMapRevealRecord
{
	UnsignedInt m_key;				// 0x00 Rva003ECA13Get of the reveal's name
	AsciiString m_name;				// 0x04 the waypoint (kind 0) or trigger area (kind 1) it reveals
	Real m_radius;					// 0x08 the reveal radius (kind 0)
	AsciiString m_playerName;		// 0x0C the player whose mask reveals it (rva00357475)
	Int m_kind;						// 0x10 0 a circle round a waypoint, 1 the bounds of a trigger area
	~NamedMapRevealRecord();				// 0x00204A30 (out of line in retail: two string releases)
};

// One scripted topple target (the list node is 0x18 bytes: links, then this).
struct ToppleRecord
{
	UnsignedInt m_key;		// 0x00 Rva003ECA13Get of the object's name
	Coord3DBase m_location;	// 0x04
};

// The named-object cache's element (Zero Hour's NamedRequest): the name and the object, null once the unit is gone
// (didUnitExist 0x0035743F). Retail destroys it through the shared pair destructor 0x0048BA39.
typedef _STL::pair<AsciiString, Object *> NamedRecord;

// The comparator of the scoped id maps (object ids, team ids, living-world players and armies): retail instantiates their tree code
// (0x00206C54, 0x002072AF, 0x0020796D, 0x00207EE0) apart from SidesList's index map, which uses the default less<>.
struct ScopedNameLess
{
	bool operator()(const _STL::pair<AsciiString, AsciiString> &left, const _STL::pair<AsciiString, AsciiString> &right) const
	{
		return left < right;
	}
};

class ScriptEngine : public SubsystemInterface, public Snapshot
{
public:
	ScriptEngine();				// 0x00208343
	void transferObjectName(const AsciiString &name, Object *unit);	// 0x00357960 (VeterancyCrateCollide::executeCrateBehavior 0x004BCFF0 needs it)
	virtual ~ScriptEngine();		// deleting 0x0020C0C7

	virtual void init();			// slot 1, 0x00205AFD
	virtual void reset();			// slot 9, 0x00209ABE
	virtual void update();			// slot 10, 0x0020D065

	enum FadeType { FADE_NONE = 0, FADE_SUBTRACT = 1, FADE_ADD = 2, FADE_SATURATE = 3, FADE_MULTIPLY = 4 };	// setFade 0x00203777

protected:
	friend class ScriptActions;
	friend class ScriptConditions;
	void updateFades();				// 0x002036CC
	void setCounterToThreat(ScriptAction *action, Bool second);	// 0x00209003 (protected in the map: IAE)
	void executeActions(ScriptAction *actions, void *script, void *name);	// 0x0020C5C7 (protected in the map: IAE; the cheat keys pass no script and the name of the key)
	void setFade(ScriptAction *action);		// 0x00203777
	ScriptCounter *bfmeCounter(AsciiString name);	// 0x0020874B (the named counter, created on first use)
	Bool *bfmeFlagForWrite(AsciiString name);	// 0x002088A0 (the named flag's value, created false on first use)
	Bool evaluateCounter(Condition *condition);	// 0x00208F61
	void setFlag(ScriptAction *action, Bool useScope);	// 0x00209405
	void setCounter(ScriptAction *action, Int randomKind, Bool fromCounter, Bool milliseconds);	// 0x00209144 (randomKind: 0 plain, 1 synced logic random, else client random)
	void setTimer(ScriptAction *action, Bool millisecondTimer, Bool random);	// 0x002094CB
	void adjustTimer(ScriptAction *action, Bool millisecondTimer, Bool add);	// 0x00209639
	void addCounter(ScriptAction *action);		// 0x002092F8
	void subCounter(ScriptAction *action);		// 0x0020933D
	void pauseTimer(ScriptAction *action);		// 0x002095CA
	void restartTimer(ScriptAction *action);	// 0x002095FF
	void setSway(ScriptAction *action);		// 0x0020385F
	void enableScript(ScriptAction *action);	// 0x00204FBB (turns the named group and script on, and the calling team's generic script)
	void disableScript(ScriptAction *action);	// 0x0020507F
	Bool evaluateCondition(Condition *condition);	// 0x002096E2 (protected in the map: IAE; runs only when the template's mode word allows the current game mode)
	void setPriorityKind(ScriptAction *action);	// 0x0020611D (protected, IAE: the same for every thing of a kind)
	void setPriorityThing(ScriptAction *action);	// 0x00205EEC (protected, like the map's IAE spelling: an attack-priority set's priority for one thing or every thing of a named list)
	void removeObjectTypes(ObjectTypes *types);	// 0x002046E4
	SequentialScript **cleanupSequentialScript(SequentialScript **it, Bool cleanDanglers, Bool flag);	// 0x00204733 (map: ?cleanupSequentialScript@ScriptEngine@@IAEPAPAVSequentialScript@@PAPAV2@_N1@Z)
	virtual void loadPostProcess();			// Snapshot slot 1, 0x0020530E
	virtual const char *GetSnapshotName() const;	// Snapshot slot 2, 0x002086B7
public:	// the map spells slot 3 ?DoXfer@ScriptEngine@@UAEXPAVXfer@@@Z (public)
	virtual void xfer(Xfer *xfer);			// Snapshot slot 3, 0x0020A859

public:
	Script *findScriptNamed(const AsciiString &name, AsciiString *scopedName);	// 0x00357130 (findScript with the name passed by reference)
	void runSubroutine(const AsciiString &scope, const AsciiString &name, Team *team);	// 0x0020C140 (callSubroutine for a team: the group or script named in the scope runs as the team's, its controlling player the current player)
	void initTemplateNameKeys();			// 0x00204D87
	void adjustToppleDirection(Object *object, Coord3D *direction);	// 0x00357340 (a named object with a scripted topple target topples towards it)
	void adjustToppleDirection(Object *object, Coord2D *direction);	// 0x00357C1F
	void addActionTemplateInfo(Template *actionTemplate);	// 0x00205A2B (an INI entry overrides the UI name, second UI name and help text of the action template with the same internal name)
	void addConditionTemplateInfo(Template *conditionTemplate);	// 0x00205A8D (the same for conditions)
	Bool evaluateConditions(Script *script, Team *team, Player *player);	// 0x00209748 (the OR of the script's AND groups, with the calling team and current player latched)
	Int getGameModeMask();	// 0x00203693 (1 in a single-player game, 2 in a multiplayer one; compared with the condition template's first word)
	Bool isMultiplayerGame();	// 0x00203688 (GameLogic's 0x001DCD1C: Zero Hour's isInMultiplayerGame; the name follows how getGameModeMask uses it)
	Bool evaluateTimer(Condition *condition);	// 0x00209486
	Bool evaluateFlag(Condition *condition);	// 0x00209382
	const ActionTemplate *getActionTemplate(Int index);		// 0x00203926 (the Template table entry for a script action type)
	const ConditionTemplate *getConditionTemplate(Int index);	// 0x00203941 (the Template table entry for a script condition type)
	// Map identity; returns the player mask a side parameter names (every
	// ScriptConditions player test feeds it to getEachPlayerFromMask).
	Int rva00357B82(Parameter *sideParm);	// 0x00357B82
	// Player-mask resolver for a named side or player (the map's second argument is a Bool pointer; actions pass 0).
	Int rva00357475(const AsciiString &name, Bool *flag);	// 0x00357475
	void rva00208A09(const AsciiString &name, AsciiString other);	// 0x00208A09 (map: ?rva00208A09@ScriptEngine@@QAEXABVAsciiString@@V2@@Z)
	void rva00208968(const AsciiString &name, Object *obj);	// 0x00208968
	Bool rva00357AF8(Object *obj);	// 0x00357AF8 (true for a named object, or an unnamed one whose id is in the scoped object-id table)
	void rva00208B2C(const AsciiString &name, Team *team);	// 0x00208B2C (the team counterpart: the team's id)
	void rva00208BCD(const AsciiString &name, AsciiString other);	// 0x00208BCD (the team counterpart of rva00208A09)
	Int *rva00208CF0(AsciiString name);	// 0x00208CF0 (the scoped Int that starts at -1, created on first use)
	AsciiString *rva00208DB8(AsciiString name);	// 0x00208DB8 (the scoped string, created empty)
	Int *rva00208E99(AsciiString name);	// 0x00208E99 (the scoped Int that starts at 0)
	void addObjectToCache(Object *obj, const AsciiString &name);	// map: ?addObjectToCache@ScriptEngine@@QAEXPAVObject@@ABVAsciiString@@@Z
	void setSequentialTimer(Object *obj, Int frames);	// 0x00203FCF
	void removeAllSequentialScripts(Object *obj);	// 0x0020517D
	void appendSequentialScript(const SequentialScript *scriptToSequence);	// 0x00207557 (queues a copy behind the script already running on the same object or team)
	void removeSequentialScriptsOf(Script *script);	// 0x00205140 (when the script's record is flagged, drops every sequential script that was started from it)
	void notifyOfTeamDestruction(Team *teamDestroyed);	// 0x002051B7 (Zero Hour's name: drops the team's sequential scripts and forgets it as the calling/condition team)
	const AttackPriorityInfo *getDefaultAttackInfo() const;	// slot 0 of m_attackInfo
	const AttackPriorityInfo *getAttackInfo(const AsciiString &setName);	// 0x00205DDD (AIUpdateInterface::m_attackInfo takes its result)
	AttackPriorityInfo *findAttackInfo(const AsciiString &setName, Bool create);	// 0x00205D5D
	void setPriorityDefault(ScriptAction *action);	// 0x002061D7
	Int getObjectCount(Int playerIndex, const AsciiString &name) const;	// 0x00206255
	void setObjectCount(Int playerIndex, const AsciiString &name, Int count);	// 0x00207DF4
	void setSequentialTimer(Team *team, Int frames);	// map: ?setSequentialTimer@ScriptEngine@@QAEXPAVTeam@@H@Z
	Bool rva00357B3D(Int playerIndex, ObjectID id, Bool flag);	// 0x00357B3D (the science-owned query ScriptConditions::evaluateScienceAcquired feeds)
	void rva002064CB(Team *team);	// 0x002064CB (stops a team's sequential script)
	void rva0020517D(Object *obj);	// 0x0020517D (stops a unit's sequential script)
	Player *getSkirmishEnemyPlayer();	// 0x00356F6E
	TeamPrototype *rva003570D1(AsciiString name);	// 0x003570D1 (the team prototype of a scope-qualified team name: resolveName(name), then the factory's two-argument lookup)
	Player *getCurrentPlayer(void);	// 0x00205C93
	void AppendDebugMessage(const AsciiString &message, Bool flag);	// 0x00205263
	void walkNamed(ScriptList *list, ScriptLink *link, Bool onlyCurrent);	// 0x0020A775 (runs the script every link of the chain names, unless it is a subroutine; onlyCurrent skips links whose entry has moved on)
	void walkChild(ScriptList *list, ScriptGroupLink *link);	// 0x0020C0E3 (the same for the scripts of every active, current group that is not a subroutine, and its child groups)
	void rva0020A586(void *script, void *name);	// 0x0020A586 (runs one script under its name: delay and one-shot handling, then 0x00209E15 or 0x0020A073; not written)
	Team *getTeamNamed(AsciiString name, Bool flag);	// 0x003584E9
	Object *getUnitNamed(Parameter *unitParm);	// 0x003588E7
	Object *getUnitNamed(AsciiString unitName);	// 0x00358752 (the by-name overload; ScriptActions::doTeamEnterNamed 0x003BEF8D; the map spells it ?lookupUnitByValue@Rva00358752Opaque@@QAEPAVObject@@VAsciiString@@@Z, evidence=?:placeholder)
	Bool didUnitExist(const AsciiString &unitName);	// 0x0035743F
	void rva00203FA8(Int id);	// 0x00203FA8
	Bool isTimeFrozenScript() const;	// 0x00203ACE
	Bool isTimeFrozenDebug();	// 0x00203AE5 (the debug window holds the logic frame while the stepping mode is on)
	Bool isClientTimeFrozenDebug();	// 0x00203B08 (the same for the client frame, stepping mode off)
	Bool rva0020424FF();	// 0x000424FF (the map's name: isTimeFrozenDebug() || isClientTimeFrozenDebug())
	void notifyCameraChange();	// 0x00203B2B (tells the debug window)
	void forceUnfreezeTime();	// 0x00203BB1 (the debug window's ForceAppContinue)
	void set(Int difficulty);	// 0x00203BCD (the map's name; Zero Hour's setGlobalDifficulty: stores m_globalDifficulty)
	Bool getEnableVTune() const;	// 0x00203BDA
	void enableVTune();	// 0x00203BE1
	void updateLogicDebugFrame();	// 0x002047CF
	void _bfme_updateClientDebugFrame();	// 0x00204806
	void toggleFrameStepping();	// 0x002064D0
	void cheatKeyVictory();	// 0x0020CDA2
	void cheatKeyDefeat();	// 0x0020CE13
	Bool evaluateConditionsInScope(const AsciiString &scope, Script *script, Team *team, Player *player);	// 0x0020A1D0
	void checkConditionsForTeamNames(Script *script, const AsciiString &scriptName);	// 0x002062A8 (picks the one team name the script's conditions refer to; a script naming two non-singleton teams is reported)
	void executeSequentialScript(Script *script, const AsciiString &scriptName);	// 0x0020A073 (queues the script as a sequential script on the team or unit its record names, once its conditions hold)
	void executeScript(Script *script, const AsciiString &name);	// 0x00209E15 (runs one script's actions for the team its conditions name, or once for no team)
	void callSubroutine(ScriptAction *action);	// 0x0020C3BB (runs the named script group's or script's subroutine)
	Bool canRunScript(Script *script);	// 0x00203F0E (clears the script's time and says whether it is active, enabled for the difficulty and due)
	void startQuickEndGameTimer();	// 0x002036A9 (m_endGameTimer = 1; ScriptActions' quick victory / defeat 0x003BD598 start it; Zero Hour's name)
	void startEndGameTimer();	// 0x002036B4 (m_endGameTimer = the win / lose message length)
	void startCloseWindowTimer();	// 0x002036C0
	Bool isTimeFast();	// 0x00203B47 (the map's placeholder name rva00203B47@Rva00203B47Host: the debug window's RunAppFast answer, or GlobalData's run-fast flag)
	void notifyOfObjectCreationOrDestruction();	// 0x002039B6
	void createNamedCache();	// 0x0020A7C9 (Zero Hour's name: every object that has a name, for lookups by name)
	void doFreezeTime();	// 0x00203AD5 (map: enable@Rva000203AD5ByteOneSetter): sets the script freeze flag
	void doUnfreezeTime();	// 0x00203ADD (map: disable@Rva000203ADDByteZeroSetter)
	void rva00356FAF(Int id);	// 0x00356FAF: forgets the id
	void undoNamedMapReveal(const AsciiString &name);	// 0x0035789D (ledger name; ScriptActions::doUndoRevealMapAtWaypointPermanent 0x003BCA43)
	NamedMapRevealRecord *createNamedMapReveal(const AsciiString &revealName, const AsciiString &waypointName, Real radius, const AsciiString &playerName, Int permanent);	// 0x00358853 (ledger name; the map spells the result BfmeStringRecord00204A30; ScriptActions::doRevealMapAtWaypoint 0x003BC9E0 and ...Permanent 0x003BCA12)
	void doNamedMapReveal(const AsciiString &name);	// 0x003577DA (appends the record to m_namedMapReveals)
	void rva00357D52(const AsciiString &name);	// 0x00357D52: drops the named map reveal
	void rva00357D86(const AsciiString &name);	// 0x00357D86
	void rva00357DAC(const AsciiString &name);	// 0x00357DAC
	void rva00357DD2(const AsciiString &name);	// 0x00357DD2
	void rva00357E7B(Int playerIndex, const AsciiString &name, Int value);	// 0x00357E7B
	void rva00357EAD(Int playerIndex, const AsciiString &name, Int value);	// 0x00357EAD
	void rva00357EDF(Int playerIndex, const AsciiString &name, Int value);	// 0x00357EDF
	void rva00357F11(Int playerIndex, const AsciiString &name, Int value);	// 0x00357F11
	void notifyOfAcquiredScience(Int playerIndex, ScienceType science);	// 0x00357F43
	void rva00357A03(Int playerIndex, ScienceType science);	// 0x00357A03: drops one entry for the science
	// The named object-type lists the scripts maintain (m_objectTypeLists): lookup by list name, and
	// add/remove one type, creating the list on first use and dropping it once it is empty.
	ObjectTypes *getObjectTypes(const AsciiString &listName);	// 0x00357651
	void doObjectTypeListMaintenance(const AsciiString &listName, const AsciiString &objectType, Bool add);	// 0x00358286
	PolygonTrigger *getQualifiedTriggerAreaByName(AsciiString name);	// 0x0035768D
	Bool rva00357A36(const AsciiString &name, Bool flag);	// 0x00357A36 (the video-completion query)
	Bool rva00357A97(const AsciiString &name, Bool flag);	// 0x00357A97
	Bool rva00357F5E(const AsciiString &name, Bool flag);	// 0x00357F5E
	Bool rva00358076(const AsciiString &name, Bool flag);	// 0x00358076
	// Per-player special-power and upgrade progress, keyed by the source
	// object's id; ScriptConditions' four "from unit" tests call them.
	Bool rva003571B8(Int playerIndex, const AsciiString &name, Bool flag, Int sourceID);	// 0x003571B8
	Bool rva0035721A(Int playerIndex, const AsciiString &name, Bool flag, Int sourceID);	// 0x0035721A
	Bool rva0035727C(Int playerIndex, const AsciiString &name, Bool flag, Int sourceID);	// 0x0035727C
	Bool rva003572DE(Int playerIndex, const AsciiString &name, Bool flag, Int sourceID);	// 0x003572DE
	Script *findScriptInScope(const AsciiString &scope, const AsciiString &name, AsciiString *scopedName);	// 0x003573C4 (findScriptNamed while the scope name is overridden; SequentialScript::xfer 0x002039D4 re-links the saved sequence with it; ScriptEngineSupport.cpp holds the row)
	void *rva002086C5(AsciiString name);	// 0x002086C5 looks a named script counter up in the counter map (null when absent)
	ScriptFlag *findFlag(AsciiString name);	// 0x0020881A the flag map's lookup (null when absent)
	// 0x00204EBB / 0x00204F3B: the group / script of that name in the side the (scope-qualified) name belongs to; the
	// scope-qualified name is handed back through the second argument when something is found.
	ScriptList *findScriptList(const AsciiString &scope);	// 0x00204E64 (a plain member that never reads this; the map spells it as a free stdcall function; the side whose player's name key is the scope's)
	ScriptGroup *findGroup(AsciiString name, AsciiString *scopedName);
	Script *findScript(AsciiString name, AsciiString *scopedName);
	AsciiString scopedPath(AsciiString name);	// 0x00205CE7 (the scope-qualified name, a slash, then the name: ScriptActions 0x003C03C6 / 0x003C04C8 call it)
	AsciiString resolveName(const AsciiString &name);	// 0x002046C0 the counter/flag name qualified by the current scope (+0x1A10C)

	_STL::vector<SequentialScript *> m_sequentialScripts;	// +0x10: setSequentialTimer 0x00204002 (team) and 0x00203FCF (object) walk [+0x10, +0x14)
	Int m_unk0x1C;	// +0x1C not recovered (the constructor 0x00208343 zeroes it)
	// The two template tables the constructor 0x00208343 builds: 0x257 action templates, then 0xCA
	// condition templates, 0x80 bytes each (getActionTemplate 0x00203926 bounds the index by 0x257 and
	// returns this + 0x20 + index * 0x80; getConditionTemplate 0x00203941 uses 0xCA and + 0x12BA0).
	ActionTemplate m_actionTemplates[0x257];		// +0x20
	ConditionTemplate m_conditionTemplates[0xCA];	// +0x12BA0
	// The counter and flag maps (bfmeCounter 0x0020874B, rva002086C5, bfmeFlagForWrite 0x002088A0, findFlag 0x0020881A):
	// keyed by the pair (scope-qualified name, name); the lookups build the key with 0x0002C4FD and search with the
	// shared pair-keyed tree find 0x0032C07C.
	typedef _STL::map<_STL::pair<AsciiString, AsciiString>, ScriptCounter> CounterMap;
	typedef _STL::map<_STL::pair<AsciiString, AsciiString>, ScriptFlag> FlagMap;
	CounterMap m_counters;	// +0x190A0
	FlagMap m_flags;	// +0x190AC
	// Scoped name tables: (scope, name) -> the id of the object / team a script named (rva00208968 0x00208968 and
	// rva00208B2C 0x00208B2C store, the copy variants 0x00208A09 / 0x00208BCD read one name's entry into another's).
	typedef _STL::map<_STL::pair<AsciiString, AsciiString>, Int, ScopedNameLess> ObjectIdMap;
	typedef _STL::map<_STL::pair<AsciiString, AsciiString>, Int, ScopedNameLess> TeamIdMap;
	ObjectIdMap m_namedObjectIds;	// +0x190B8
	TeamIdMap m_namedTeamIds;	// +0x190C4
	// Keyed byte flags the save game stores after the scoped tables (xfer 0x0020A859, version 3 and later: a count, then name and flag).
	_STL::map<AsciiString, unsigned char> m_keyedByteFlags;	// +0x190D0
	// The living-world tables xfer saves from version 4: players (the id starts at -1, rva00208CF0 0x00208CF0), armies (the id
	// starts at 0, rva00208E99 0x00208E99) and strings (start empty, rva00208DB8 0x00208DB8), each keyed by (scope, name).
	typedef _STL::map<_STL::pair<AsciiString, AsciiString>, Int, ScopedNameLess> ScopedIntMap;
	typedef _STL::map<_STL::pair<AsciiString, AsciiString>, AsciiString> ScopedStringMap;
	ScopedIntMap m_livingWorldPlayers;	// +0x190DC
	ScopedIntMap m_livingWorldArmies;	// +0x190E8
	ScopedStringMap m_livingWorldStrings;	// +0x190F4
	// The attack-priority sets: slot 0 is the shared default, findAttackInfo 0x00205D5D searches slots 1..count-1 by
	// name and claims the next one (up to 256).
	AttackPriorityInfo m_attackInfo[256];	// +0x19100
	Int m_attackInfoCount;	// +0x1A100
	// +0x1A104: the end-game timer. ControlBar::showSpecialPowerShortcut 0x0031DAF8 and
	// showPurchaseScience 0x0031AD5A return when it is >= 0 (a game that is ending).
	Int m_endGameTimer;
	Bool isGameEnding() const { return m_endGameTimer >= 0; }
	Int m_closeWindowTimer;	// +0x1A108 startCloseWindowTimer 0x002036C0 sets it (Zero Hour's m_closeWindowTimer)
	AsciiString m_scopeName;	// +0x1A10C resolveName 0x002046C0 qualifies counter and flag names with it
	Team *m_callingTeam;	// +0x1A110 notifyOfTeamDestruction 0x002051B7 clears it when that team is destroyed (Zero Hour's m_callingTeam)
	Int m_unk0x1A114;	// +0x1A114 cleared by rva00356FAF when it equals the id being dropped
	Team *m_conditionTeam;	// +0x1A118 cleared the same way (Zero Hour's m_conditionTeam)
	Int m_unk0x1A11C;	// +0x1A11C cleared by rva00356FAF when it equals the id being dropped
	// Named-unit records, 8 bytes each: the name at +0, a flag at +4 (zero
	// while the unit lives). didUnitExist 0x0035743F walks them.
	_STL::vector<NamedRecord> m_namedObjects;	// +0x1A120 (addObjectToCache 0x0020A5FF pushes back, createNamedCache 0x0020A7C9 clears and refills it)
	Bool m_firstUpdate;	// +0x1A12C Zero Hour's m_firstUpdate: the constructor, reset 0x00209ABE and newMap 0x00207C02 set it, update 0x0020D065 clears it after createNamedCache
	unsigned char m_unk0x1A12D[3];	// +0x1A12D padding
	Player *m_currentPlayer;	// +0x1A130: getCurrentPlayer 0x00205C93 and the enemy lookup 0x00356F6E read it
	Player *m_skirmishHumanPlayer;	// +0x1A134 (Zero Hour's order after m_currentPlayer; constructor and reset zero it)
	// The camera fade, in Zero Hour's member order (updateFades 0x002036CC and setFade 0x00203777 use all of them).
	Int m_fade;				// +0x1A138 (FadeType)
	Bool m_unk0x1A13C;	// +0x1A13C a flag byte updateFades clears and setFade sets
	unsigned char m_unk0x1A13D[3];	// +0x1A13D padding
	Real m_minFade;			// +0x1A140
	Real m_maxFade;			// +0x1A144
	Real m_curFadeValue;		// +0x1A148
	Int m_curFadeFrame;		// +0x1A14C
	Int m_fadeFramesIncrease;	// +0x1A150
	Int m_fadeFramesHold;		// +0x1A154
	Int m_fadeFramesDecrease;	// +0x1A158
	UnsignedInt m_frameObjectCountChanged;	// +0x1A15C notifyOfObjectCreationOrDestruction 0x002039B6 stores the logic frame in it (Zero Hour's member)
	UnsignedInt m_unk0x1A160;	// +0x1A160 not recovered (0x002039C5 stores the frame there too; the constructor and reset zero it)
	// The per-player object counts the scripts keep (getObjectCount 0x00206255, setObjectCount 0x00207DF4), keyed by
	// the name key of the object type's name.
	_STL::map<UnsignedInt, Int> m_objectCounts[20];	// +0x1A164
	// The completion lists Zero Hour keeps (m_completedVideo, m_testingSpeech, m_testingAudio, m_uiInteractions; the constructor builds
	// them one by one in this order, reset 0x00209ABE clears them in the order video, interactions, speech, audio) and a fifth BFME 2 list.
	// rva00357A36 / rva00357A97 find and erase a key, rva00357D86 / rva00357DAC / rva00357DD2 add one.
	_STL::list<UnsignedInt> m_completedVideo;	// +0x1A254
	_STL::list<ScriptTimedKey> m_testingSpeech;	// +0x1A258 (the key and the logic frame its clip ends: rva00357F5E)
	_STL::list<ScriptTimedKey> m_testingAudio;	// +0x1A25C (the same for sounds: rva00358076)
	_STL::list<UnsignedInt> m_uiInteractions;	// +0x1A260
	_STL::list<UnsignedInt> m_unk0x1A264;	// +0x1A264 not recovered (rva00357DD2 pushes to its front)
	// Zero Hour's four per-player (0..19) lists, in its member order (reset clears them in this order): rva00357E7B/EAD/EDF/F11
	// append to them and rva003571B8/21A/27C/2DE query and erase.
	_STL::list<ScriptProgressRecord> m_triggeredSpecialPowers[20];	// +0x1A268
	_STL::list<ScriptProgressRecord> m_midwaySpecialPowers[20];	// +0x1A2B8
	_STL::list<ScriptProgressRecord> m_finishedSpecialPowers[20];	// +0x1A308
	_STL::list<ScriptProgressRecord> m_completedUpgrades[20];	// +0x1A358
	// notifyOfAcquiredScience 0x00357F43 appends to the acquiring player's vector.
	_STL::vector<ScienceType> m_acquiredSciences[20];	// +0x1A3A8
	// Zero Hour's m_toppleDirections: the scripted topple targets, keyed by the name key of the object's name
	// (adjustToppleDirection 0x00357340 / 0x00357C1F look the object up; BFME 2 stores the location the object topples towards).
	_STL::list<ToppleRecord> m_toppleDirections;	// +0x1A498
	_STL::vector<NamedMapRevealRecord> m_namedMapReveals;	// +0x1A49C doNamedMapReveal 0x003577DA appends, rva00357D52 drops one by name
	// The breeze (Zero Hour's BreezeInfo, in its member order; setSway 0x0020385F writes all of them).
	Real m_breezeDirection;		// +0x1A4A8
	Real m_breezeDirectionX;	// +0x1A4AC Sin of the direction
	Real m_breezeDirectionY;	// +0x1A4B0 Cos of the direction
	Real m_breezeIntensity;		// +0x1A4B4
	Real m_breezeLean;		// +0x1A4B8
	Real m_breezeRandomness;	// +0x1A4BC
	short m_breezePeriod;		// +0x1A4C0
	short m_breezeVersion;		// +0x1A4C2
	// +0x1A4C4: the difficulty OptionPreferences::getCampaignDifficulty 0x002E4573 returns when
	// the preference is absent (an int read straight off TheScriptEngine).
	Int m_globalDifficulty;
	_STL::vector<ObjectTypes *> m_objectTypeLists;	// +0x1A4C8 getObjectTypes 0x00357651 walks it, doObjectTypeListMaintenance 0x00358286 appends
	Bool m_freezeByScript;	// +0x1A4D4 isTimeFrozenScript 0x00203ACE reads it, doFreezeTime 0x00203AD5 / doUnfreezeTime 0x00203ADD write it
	Bool m_objectsShouldReceiveDifficultyBonus;	// +0x1A4D5 (Zero Hour's member after m_freezeByScript: the constructor sets it, reset 0x00209ABE sets it again)
	Bool m_chooseVictimAlwaysUsesNormal;	// +0x1A4D6 (Zero Hour's next member: the constructor and reset clear it)
	Bool m_enableVTune;	// +0x1A4D7 getEnableVTune 0x00203BDA reads it, 0x00203BE1 sets it
	Bool m_unk0x1A4D8;	// +0x1A4D8 not recovered (the constructor clears it)
	Bool m_steppingLogicFrames;	// +0x1A4D9 the debug window's frame-stepping mode: 0x002064D0 toggles it ("Stepping logic frames." / "Stepping client frames."), 0x002047CF and 0x00204806 each act on one value
	Bool m_unk0x1A4DA;	// +0x1A4DA not recovered (the constructor sets it)
	unsigned char m_unk0x1A4DB[5];	// +0x1A4DB padding to the doubles
	// Zero Hour's SPECIAL_SCRIPT_PROFILING members, kept by retail (the constructor zeroes all four; the fourth is not recovered).
	double m_numFrames;	// +0x1A4E0
	double m_totalUpdateTime;	// +0x1A4E8
	double m_maxUpdateTime;	// +0x1A4F0
	double m_unk0x1A4F8;	// +0x1A4F8
};
// SIZE 0x1A500
typedef char ScriptEngineSize[(sizeof(ScriptEngine) == 0x1A500) ? 1 : -1];

void parseScriptAction(INI *ini);		// 0x00206DFF (INI block ScriptAction: parses an ActionTemplate and hands it to addActionTemplateInfo)
void parseScriptCondition(INI *ini);	// 0x00206E63

extern ScriptEngine *TheScriptEngine;

#endif
