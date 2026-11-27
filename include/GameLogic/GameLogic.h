// GameLogic.h: the logic subsystem singleton (TheGameLogic, VA 0x00DFE78C). SIZE 0x2A8.
// Target facts:
//  - Bases SubsystemInterface +0x00 and Snapshot +0x0C: ctor 0x002470AE calls
//    SubsystemInterface() and stores Snapshot's table at +0x0C before its own
//    tables 0x00BEE130 (+0x00, 17 slots) and 0x00BEE120 (+0x0C, 4 slots).
//  - Size 0x2A8: Win32GameEngine::createGameLogic 0x00041F81 allocates 0x2A8
//    for W3DGameLogic, which only replaces the two vptrs.
//  - Members and their order: the ctor's stores and sub-object constructors.
//    Types are given where the ctor or a reader proves them; the rest stay
//    byte gaps of the measured size.
//  - Slots 14..16 of the primary table are __purecall here; W3DGameLogic
//    fills them with factories that allocate W3DTerrainLogic (0x00041FCF),
//    W3DGhostObjectManager (0x00042004) and a 0x10-byte object (0x00042036).
//    Slot 16's name is unknown, so it is not declared yet.
//  - Primary slots 1 (init 0x00243EE7), 9 (reset 0x002442A4), 10 (update, the
//    shared empty body 0x000B3FD0) and 13 (updatePhase 0x0024555A; the map spells
//    it update(int), but the slot is SubsystemInterface::updatePhase) override it.
// The object lookup is the object
// hash at +0xB4: an STLport hash_map<ObjectID, Object *> and its node.
#pragma once
#ifndef BFME2_GAMELOGIC_H
#define BFME2_GAMELOGIC_H

#include "Common/GameType.h"
#include "Common/Snapshot.h"
#include "Common/STLTypedefs.h"
#include "Common/StlHashMap.h"
#include "Common/StlList.h"
#include "Common/FreelistPool.h"
#include "Common/Vector.h"
#include "Common/AsciiString.h"
#include "Common/OpaqueRefElement4.h"
#include "Common/SubsystemInterface.h"

class Object;
class Drawable;
class CommandButton;
class Xfer;
class GameMessage;
class InvisibilityManager;
class LoadScreen;
class WindowLayout;
class UpdateModule;
class TerrainLogic;
class GhostObjectManager;
class ThingTemplate;
class Team;
class CommandButton;
class GameInfo;
class Rva0023E928;
struct CreateMask;

// The object table: retail's instantiation is
// hashtable<pair<const ObjectID, Object *>, ObjectID, rts::hash<ObjectID>, ...,
// _STL::equal_to<ObjectID>, ...> (rows ??1/_M_insert/??0 of the ObjectID tables).
typedef _STL::hash_map<ObjectID, Object *, rts::hash<ObjectID>, _STL::equal_to<ObjectID> > ObjectPtrHash;
// The buildable overrides keyed by template name: retail's operator[] COMDAT 0x002460C3 is
// hash_map<AsciiString, BuildableStatus, rts::hash<AsciiString>, equal_to<AsciiString> >.
typedef _STL::hash_map<AsciiString, BuildableStatus, rts::hash<AsciiString>, _STL::equal_to<AsciiString> > BuildableOverrideMap;
// Pooled, like ObjectIDList: its push_back 0x001EC03C -> insert 0x001EBD16 -> node creator 0x001EB9AC pops the
// freelist (FreelistPool::pop 0x002393E2); destroyObject 0x00242C09 and prepareLogicForObjectLoad 0x00242C86 call it.
typedef _STL::list<Object *, FreelistAllocator<Object *> > ObjectPointerList;

// Why and when a player slot left the game: GameLogic +0x1C4 holds one per slot.
// setPlayerQuitStatus 0x0023D201 stores the reason and the frame; setPlayerLeaveStatus 0x0023D1A5
// clears +0x10 and stores +0x14 and the name; the game-over report 0x0023F8DA reads them all.
struct PlayerLeaveStatus
{
	Int m_reason;			// +0x00 0 none, 1 graceful, 2 voted out
	UnsignedInt m_quitFrame;	// +0x04
	UnsignedInt m_defeatFrame;	// +0x08
	UnsignedInt m_victoryFrame;	// +0x0C
	Bool m_unk0x10;			// +0x10 the report says "Present at final frame" while it is clear
	Int m_unk0x14;			// +0x14
	AsciiString m_name;		// +0x18
};
// SIZE 0x1C

// The damage-over-time manager GameLogic +0x174 owns: a Snapshot (its table holds the deleting
// destructor, an empty loadPostProcess, GetSnapshotName and xfer). Only the members its users need are declared.
class DOTManager : public Snapshot
{
public:
	virtual ~DOTManager();
	virtual void loadPostProcess();
	UnsignedInt rva0043B72D(Int objectID);	// 0x0043B72D

protected:
	virtual const char *GetSnapshotName() const;
	virtual void xfer(Xfer *xfer);
};

// The terrain cell claim manager GameLogic +0x170 owns: a Snapshot. Only the members its users need are declared.
class TerrainResourceManager : public Snapshot
{
public:
	virtual ~TerrainResourceManager();
	virtual void loadPostProcess();

protected:
	virtual const char *GetSnapshotName() const;
	virtual void xfer(Xfer *xfer);
};

// One object table-of-contents record: a template name and the short ID a save file uses
// for it (addTOCEntry 0x00245F14 builds {name, (UnsignedShort)id} and pushes it; the map
// spells it a global struct: ?findTOCEntryByName@GameLogic@@QAEPAUObjectTOCEntry@@...).
struct ObjectTOCEntry
{
	AsciiString name;
	UnsignedShort id;
};
typedef _STL::list<ObjectTOCEntry> ObjectTOCList;

class GameLogic : public SubsystemInterface, public Snapshot
{
public:
	GameLogic();				// 0x002470AE
	virtual ~GameLogic();			// 0x00243BA0; deleting 0x00246213

	virtual void init();			// slot 1, 0x00243EE7
	virtual void reset();			// slot 9, 0x002442A4
	virtual void update() {}		// slot 10, the shared empty body 0x000B3FD0
	virtual void updatePhase(Int phase);	// slot 13, 0x0024555A

protected:
	virtual TerrainLogic *createTerrainLogic() = 0;			// slot 14
	virtual GhostObjectManager *createGhostObjectManager() = 0;	// slot 15

	// Snapshot, secondary table 0x00BEE120
	virtual void loadPostProcess();			// slot 1, 0x00242D1D
	virtual const char *GetSnapshotName() const;	// slot 2, 0x00041FC9
	virtual void xfer(Xfer *xfer);			// slot 3, 0x0024797E

public:
	void rva0023D17D();			// 0x0023D17D (the map has no better name; Network::init and startNewSession call it)
	Object *findObjectByID(ObjectID id) throw();	// 0x00049DC5; callers call it out of line
	void removeObjectFromLookupTable(Object *obj);	// 0x00240144 (Object::setID)
	void addObjectToLookupTable(Object *obj);		// 0x00242AC2 (Object::setID)
	Bool findBuildableStatusOverride(const ThingTemplate *tmpl, BuildableStatus *status);	// 0x00245EDF (map: placeholder rva00245EDF)
	void setBuildableStatusOverride(const ThingTemplate *tmpl, BuildableStatus status);	// 0x00246F71
	Object *getFirstObject();		// 0x0023CAD2, returns m_objList
	UnsignedInt getObjectCount();		// 0x0023CFCE, walks m_objList
	Object *friend_createObject(const ThingTemplate *thing, const CreateMask *mask, Team *team, ObjectID id); // 0x0023CAE7
	void destroyObject(Object *obj);	// 0x00242C09
	void destroyAllObjectsImmediate();	// 0x00243B52
	void rva00240EBC();	// 0x00240EBC (s05: calls Object::rva00295F05(true) on each +0x164 entry, then clears it)
	Bool rva002034E9() const;	// 0x002034E9 (map: placeholder host): false only when +0x114 == 3 and 0x0023C6A4 says no; Player::usesRankLevelLimitOverride 0x002A9F53 tail-calls it
	void sendObjectDestroyed(Object *obj);	// 0x0023CD67
	void bindObjectAndDrawable(Object *obj, Drawable *draw);	// 0x0023CD4A
	void selectObject(Object *obj, Bool createNewSelection, UnsignedInt playerMask, Bool affectClient);	// 0x0023C924
	void deselectObject(Object *obj, UnsignedInt playerMask, Bool affectClient);	// 0x0023C9F8
	void processCommandList();		// 0x0023CAA3
	UnsignedInt getCRC(Int mode);		// 0x0023CB2C
	UnsignedByte isGamePaused();		// 0x0023CD97, returns m_gamePaused
	Bool isInMultiplayerGame();		// 0x00042235
	void lastHeardFrom(Int playerId);	// 0x0023CF1C
	void processProgress(Int playerId, Int percentage);	// 0x0023D740
	void processProgressComplete(Int playerId);	// 0x0023D76E
	// Handles a type-30 save-game command message from the network; the ledger spells the message class as the placeholder Rva0023E928.
	void ProcessSaveGameCommand(Rva0023E928 *cmd, Bool isLocal);
	void clearGameData(Bool showScoreScreen, Bool unknown);	// 0x00376E92
	void CallPlayerGiveMoney(Int fromPlayerIndex, Int toPlayerIndex, Int amount);	// 0x0023D793
	void LivingWorldTacticalBattleComplete();	// 0x0023D914
	void setMissionName(const AsciiString &name);	// 0x0023DC8E: kept unless empty or GlobalData +0x9AD is set
	// 0x0023DCCE (placeholder row): inline in retail (a COMDAT in GameLogic's unit, expanded by its caller 0x002423E3;
	// defined in GameLogic.cpp until GameLogic.h can include GlobalData.h); LinearCampaign's mission start and LivingWorldLogic 0x002B36F3 call it on TheGameLogic.
	void setMissionOpening(const AsciiString &opening, const OpaqueRefElement4 &ref, const OpaqueRefElement4 &voice);
	void rva0023DA4E();	// 0x0023DA4E: clearGameData(false, true) when the living-world query 0x002B3669 holds, else LivingWorldTacticalBattleComplete
	void startNewGame(Bool fromSave);	// 0x00248558
	void loadMapINI(AsciiString mapName);	// 0x0023E3CE
	void logicMessageDispatcher(GameMessage *msg, void *userData);	// 0x00377613
	void processDestroyList();		// 0x002413DF
	void prepareLogicForObjectLoad();	// 0x00242C86
	void registerObject(Object *obj);	// 0x00242AE9
	ObjectTOCEntry *findTOCEntryByName(AsciiString name);	// 0x00240167
	void addTOCEntry(AsciiString name, UnsignedShort id);	// 0x00245F14
	void xferObjectTOC(Xfer *xfer);	// 0x00245F79
	void QueueForNotifyPathfindCellChanged(Object *obj);	// 0x00241C25
	void SwapObjectIDs(Object *a, Object *b);	// 0x00242BB4
	void formatPlayerStartWaypointName(AsciiString *out);	// 0x0024822B
	void bfmePopulateGameReport(GameInfo *info, Int *slotNum);	// 0x00247378
	void DoExitMap(Bool selfDestruct);	// 0x0023D0E3
	void TransitionFromLivingWorldTacticalBattle();	// 0x0023D0CD
	void CreateWOTRMPPlayers();		// 0x0023E99A
	void friend_awakenUpdateModule(Object *obj, UpdateModule *module, UnsignedInt whenToWakeUp);	// 0x0024297F

	// Map identities without a recovered name yet (spelled as the map has them).
	Bool rva0042219();			// 0x00042219, mode gate
	char rva0023C6FD();			// 0x0023C6FD (returns char in the map; SpecialPowerModule::rva00493251 gates the MP/skirmish filter on it)
	Bool rva00085124();			// 0x00085124, mode 4-or-7 predicate
	Bool rva001DCD1C();			// 0x001DCD1C
	void rva0023CD9E(Bool paused, Int mode, Bool affectInput);	// 0x0023CD9E
	void rva0023D0C2(Object *obj, Int value);	// 0x0023D0C2, +0x184 forwarder
	void rva0023D007(Int value);		// 0x0023D007, +0x184 forwarder
	const AsciiString &rva0023FB58(Int slot);	// 0x0023FB58: the slot's name, "Invalid Slot" out of range
	void rva00240E18(Bool keepMode);	// 0x00240E18: restarts the frame count and empties the object list and every update queue
	void rva0023FABE(Object *obj);	// 0x0023FABE: gives the object a new drawable and raises its Lua event
	PlayerLeaveStatus *getPlayerLeaveStatus(Int slot);	// 0x0023D1E3: NULL outside 0..7
	void rva0023F8DA();	// 0x0023F8DA: appends the game-over details of every slot to the last slot-name string, once
	void rva0023E0C7();	// 0x0023E0C7: adds the observer side and its team to the sides list, then validates the sides
	void rva0023D68E(Bool on);		// 0x0023D68E: passes the flag to every object's drawable (base sites only when set)

	UnsignedInt getFrame() const { return m_frame; }
	Bool getCommandButtonOverride(const AsciiString &commandSetName, Int slot, const CommandButton **button);	// looks up (slot, command set name) in the override tree at +0x24 and hands back its CommandButton
	InvisibilityManager *getManager178() const { return m_manager178; }
	Bool isScoringEnabled() const { return m_isScoringEnabled; }
	void enableScoring(Bool score) { m_isScoringEnabled = score; }
	Bool getDrawIconUI() const { return m_drawIconUI; }
	void setDrawIconUI(Bool setting) { m_drawIconUI = setting; }
	void setShowBehindBuildingMarkers(Bool setting) { m_showBehindBuildingMarkers = setting; }
	void setShowDynamicLOD(Bool setting) { m_showDynamicLOD = setting; }
	void setControlBarOverride(const AsciiString &templateName, Int slot, const CommandButton *button);	// 0x0024792F (ScriptActions 0x003BCB7A)
	Int getGameMode() const { return m_gameMode; }
	Bool isWarOfTheRingGame() const;	// 0x00210C66: +0x114 == 1 || +0x114 == 2 (the two War of the Ring campaign kinds; AptQuitMenu 0x0051AFF9, 0x0051B04A)
	void enterFPMode();	// 0x000421FD (FPModeScope constructor 0x0004224C)
	void setPlayerLeaveStatus(Int slot, const AsciiString &name, Int status);	// records a slot's player name and status for the game report
	void setPlayerQuitStatus(Int slot, Int status);	// 0x0023D201: records why a player slot left and the frame it happened on (Network::vslot98 0x0025E20F)
	void updateObjectsChangedTriggerAreas() { m_frameObjectsChangedTriggerAreas = m_frame; }

	BuildableOverrideMap m_thingTemplateBuildableOverrides;	// +0x10 template name -> BuildableStatus (setBuildableStatusOverride 0x00246F71)
	unsigned char m_unk0x24[0xC];		// +0x24 _STL::map<int, ?> (ctor 0x0024613C)
	Real m_unk0x30;				// +0x30
	Real m_unk0x34;				// +0x34
	UnsignedInt m_unk0x38;			// +0x38 the network command timestamp
	Bool m_unk0x3C;				// +0x3C
	UnsignedInt m_frame;			// +0x40
	Bool m_unk0x44;				// +0x44
	Int m_unk0x48;				// +0x48
	unsigned char m_unk0x4C[4];		// +0x4C not set by the ctor
	_STL::list<Int> m_unk0x50;		// +0x50 (_List_base<int> ctor, cleared at the end of the ctor)
	AsciiString m_unk0x54;			// +0x54 the game report's header text (bfmePopulateGameReport; the destructor releases it)
	_STL::vector<AsciiString> m_unk0x58;	// +0x58 one name per player slot: rva0023FB58 returns a const AsciiString & into it
	AsciiString m_unk0x64;			// +0x64 the game report's command-line summary (bfmePopulateGameReport)
	AsciiString m_unk0x68;			// +0x68 the game report's closing line (bfmePopulateGameReport)
	Bool m_unk0x6C;				// +0x6C
	Bool m_unk0x6D;				// +0x6D the end flag finishShowPurchaseScience reads
	Bool m_unk0x6E;				// +0x6E
	Bool m_unk0x6F;				// +0x6F
	Bool m_unk0x70;				// +0x70
	Bool m_unk0x71;				// +0x71
	Bool m_hasMissionName;			// +0x72 set with a name, cleared without, by setMissionName 0x0023DC8E and setMissionOpening 0x0023DCCE
	AsciiString m_missionName;		// +0x74 setMissionName 0x0023DC8E (LinearCampaign's mission start and LivingWorldLogic 0x002B36F3 pass it)
	Bool m_unk0x78;				// +0x78
	AsciiString m_unk0x7C;			// +0x7C (StringBase(const char *) in the ctor)
	AsciiString m_unk0x80;			// +0x80
	AsciiString m_missionOpening;		// +0x84 setMissionOpening 0x0023DCCE (the mission record's opening)
	OpaqueRefElement4 m_missionOpeningRef;	// +0x88 (LinearCampaign passes an empty one)
	OpaqueRefElement4 m_missionVoice;	// +0x8C (LinearCampaign passes the record's voice)
	Int m_unk0x90;				// +0x90 (7 in the ctor)
	Int m_unk0x94;				// +0x94
	Bool m_isScoringEnabled;		// +0x98 ScoreKeeper::addObjectLost 0x0039CF73 gates on it
	Bool m_showBehindBuildingMarkers;	// +0x99 (ScriptActions 0x003BCA68)
	Bool m_drawIconUI;			// +0x9A InGameUI::addFloatingText 0x002A0D19 gates on it
	Bool m_showDynamicLOD;		// +0x9B (ScriptActions 0x003BCA8E)
	Bool m_unk0x9C;				// +0x9C
	Bool m_unk0x9D;				// +0x9D
	Bool m_unk0x9E;				// +0x9E
	Bool m_unk0x9F;				// +0x9F
	Int m_unk0xA0;				// +0xA0
	Int m_unk0xA4;				// +0xA4 (1 in the ctor)
	Bool m_unk0xA8;				// +0xA8
	Object *m_objList;			// +0xAC the head getFirstObject returns
	Object *m_objListTail;		// +0xB0 the tail registerObject 0x00242AE9 passes to Object::appendToList with m_objList
	ObjectPtrHash m_objHash;		// +0xB4 findObjectByID's table
	_STL::vector<UpdateModule *> m_normalUpdates[4];	// +0xC8 one per update phase: registerObject 0x00242AE9 and
						// friend_awakenUpdateModule 0x0024297F index them by UpdateModule::getUpdatePhase()
	_STL::vector<UpdateModule *> m_sleepyUpdates;	// +0xF8 modules asleep forever (0x3FFFFFFF): registerObject stores
						// their index here; friend_awakenUpdateModule moves the last entry into a woken one's slot
	UpdateModule *m_curUpdateModule;	// +0x104
	ObjectPointerList m_objectsToDestroy;	// +0x108
	Int m_unk0x10C;				// +0x10C
	Int m_gameMode;				// +0x110 (9 in the ctor)
	Int m_unk0x114;				// +0x114 (3 in the ctor)
	Int m_unk0x118;				// +0x118 (1000 in the ctor)
	Bool m_unk0x11C;			// +0x11C
	Bool m_unk0x11D;			// +0x11D
	LoadScreen *m_loadScreen;		// +0x120 processProgress 0x0023D740 forwards to it
	Bool m_gamePaused;			// +0x124 isGamePaused 0x0023CD97 returns it
	Bool m_unk0x125;			// +0x125 GameClient::update 0x0023BEE8 skips its updates while set
	Bool m_unk0x126;			// +0x126 (1 in the ctor)
	Bool m_unk0x127;			// +0x127 (1 in the ctor)
	Bool m_progressComplete[8];		// +0x128 processProgressComplete 0x0023D76E
	UnsignedInt m_progressCompleteTimeout[8]; // +0x130 lastHeardFrom 0x0023CF1C stamps it
	Bool m_unk0x150;			// +0x150
	Int m_unk0x154;				// +0x154
	Int m_unk0x158;				// +0x158
	Int m_unk0x15C;				// +0x15C
	Int m_unk0x160;				// +0x160
	_STL::vector<Object *> m_unk0x164;	// +0x164 (_Vector_base ctor); rva00240EBC 0x00240EBC calls Object::rva00295F05(true) on each, then clears it
	TerrainResourceManager *m_terrainResourceManager;	// +0x170 (init news it; loadPostProcess forwards to it)
	DOTManager *m_dotManager;		// +0x174 (init news it; loadPostProcess forwards to it)
	InvisibilityManager *m_manager178;	// +0x178
	Int m_unk0x17C;				// +0x17C
	UnsignedInt m_frameObjectsChangedTriggerAreas; // +0x180 Object::initObject 0x002934E7 stores the frame
	unsigned char m_unk0x184[0x30];		// +0x184 a class with ctor 0x0040E039 (the +0x184 forwarders' target)
	Int m_unk0x1B4;				// +0x1B4
	WindowLayout *m_background;		// +0x1B8 window cleanup 0x00376D49 owns it
	Bool m_backgroundPending;		// +0x1BC
	ObjectTOCList m_objectTOC;		// +0x1C0 template name -> save-file ID (addTOCEntry 0x00245F14 pushes {name, id}; xferObjectTOC 0x00245F79)
	PlayerLeaveStatus m_playerLeaveStatus[8];	// +0x1C4 one per slot (getPlayerLeaveStatus 0x0023D1E3; eh vector ctor)
	Int m_unk0x2A4;				// +0x2A4 (2 in the ctor)
};
// SIZE 0x2A8

extern GameLogic *TheGameLogic;

// True once the game has run long enough for players to give each other money.
Bool canPlayersTransferMoneyYet();

// Logic frames per second, a variable in BFME2 (Zero Hour's LOGICFRAMES_PER_SECOND).
extern Int BfmeLogicFramesPerSecond;

#endif
