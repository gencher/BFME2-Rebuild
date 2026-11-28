// Player.h: one side in a game; PlayerList holds 20 (ctor 0x002A7F80). SIZE 0x758.
// Target facts (code RVAs, tables VAs):
//  - Size 0x758: PlayerList's ctor allocates 0x758 before each ??0Player(Int).
//  - Bases: the ctor 0x002B0F3C stores Snapshot's table at +0x00, the science-query
//    interface's folded pure table 0x00BE2B78 at +0x04, constructs a
//    SciencePurchasePointTracker at +0x08 (ctor 0x00380523), then Player's own tables
//    0x00BFDF3C (+0), 0x00BFDF30 (+4) and 0x00BFDF18 (+8). Both are in
//    Common/SciencePurchasePointTracker.h. The +4 table's slots are this-adjusted
//    overrides: 0x002B118A (`add ecx,-4; jmp hasScience`), the folded getter 0x0030D377
//    (this+0x20 = Player+0x24, the tracker's purchase points) and 0x002B1192 (stores
//    isScienceHidden into its Bool *). The +8 table holds Player's five tracker overrides.
//  - 0x00BFDF3C, eight slots: deleting dtor, Snapshot's loadPostProcess (the
//    shared empty body), GetSnapshotName 0x002B117C, xfer 0x002B1522 (the map
//    spells it DoXfer, non-virtual), then computeSuperweaponTarget,
//    checkBridges, getAiBaseCenter, repairStructure.
//  - Members: the ctor's stores and sub-object constructors and the dtor
//    0x002B11A7's releases (StringBase<unsigned short> at +0x38, StringBase<char>
//    at +0x4C, +0x58, +0x74C). Names: Zero Hour's member order where the ctor
//    agrees with it, and Player::init 0x002AF729's stores:
//    template +0x34, build list +0x278, colours +0x280/+0x284,
//    AI +0x2DC, +0x2E4/+0x2E8, default team +0x2EC (setDefaultTeam 0x002ABC7E),
//    build flags +0x338/+0x339 (allowedToBuild 0x002ABE24), observer +0x33A,
//    units-should-hunt +0x33D (setUnitsShouldHunt 0x002AE9B8).
//    Sub-objects whose class has no real header on main stay byte gaps of
//    their measured size, with the class or constructor that builds them.
// Methods: every Player row of the map with the signature its decorated name
// states, except rows that need placeholder (Rva/Bfme) or template types.
#pragma once
#ifndef BFME2_PLAYER_H
#define BFME2_PLAYER_H

#include <stddef.h>
#include "Common/GameType.h"
#include "Common/FreelistPool.h"
#include "Common/AsciiString.h"
#include "Common/UnicodeString.h"
#include "Common/Handicap.h"
#include "Common/ScoreKeeper.h"
#include "Common/Money.h"
#include "Common/ObjectIDListType.h"
#include "Common/Command.h"
#include "Common/UnitRevivalTracker.h"
#include "Common/Snapshot.h"
#include "Common/StlList.h"
#include "Common/STLTypedefs.h"
#include "Common/StlHashMap.h"
#include "Common/SciencePurchasePointTracker.h"
#include "Common/Vector.h"
#include "Common/Science.h"
#include "Common/BitFlags.h"
#include "Common/StlMap.h"
#include "Common/Energy.h"
#include "Common/UpgradeMaskType.h"

class Coord3D;
class Xfer;
class AIGroup;
class AIPlayer;
class AcademyStats;
class BattlePlanBonuses;
enum BattlePlanStatus;
class BuildListInfo;
class RankInfo;
class CreateAHeroData;
class Dict;
class GameMessage;
class Object;
class PlayerTemplate;
class ResourceGatheringManager;
class SpecialPowerTemplate;
class Team;
class Squad;
enum GameDifficulty;
class TeamPrototype;
class TeamRelationMap;
class ThingTemplate;
class TunnelTracker;
class Upgrade;
class UpgradeTemplate;
class Waypoint;
class BfmeTab1026;
enum PlayerType { PLAYER_HUMAN = 0, PLAYER_COMPUTER = 1 };
enum Relationship;
// The three states a science can be in for one player. setScienceAvailability 0x002AD8C9
// appends a science given 1 to m_sciencesDisabled (+0x2FC) and one given 2 to
// m_sciencesHidden (+0x308); 0 leaves it in neither.
enum ScienceAvailabilityType
{
	SCIENCE_AVAILABILITY_INVALID = -1,
	SCIENCE_AVAILABLE,
	SCIENCE_DISABLED,
	SCIENCE_HIDDEN,
	SCIENCE_AVAILABILITY_COUNT
};
// Retail's name table for these values (VA 0x00BFDC0C: Available, Disabled, Hidden, NULL).
extern const char *ScienceAvailabilityNames[];
enum ScienceType;
enum UpgradeStatusType;

// A player's standing towards other players, keyed by player index.
// Target facts: table VA 0x00BFDD90 (deleting dtor 0x002ADE55, Snapshot's empty
// loadPostProcess, GetSnapshotName 0x002AD0C0 returning "PlayerRelationMap", xfer 0x002ADEBE);
// ctor 0x002AF6F7; dtor 0x002AD078 clears the table (0x001DBCDC) and destroys it. The table is
// STLport's hash_map at +0x04: Player::getRelationship 0x002AC3E0 tests its element count
// (+0x14) and calls _M_find on it.
typedef _STL::hash_map<Int, Relationship, rts::hash<Int>, _STL::equal_to<Int> > PlayerRelationHash;
class PlayerRelationMap : public Snapshot
{
public:
	PlayerRelationMap();						// 0x002AF6F7
protected:
	virtual ~PlayerRelationMap();				// 0x002AD078
	virtual void loadPostProcess() {}			// slot 1, the shared empty body 0x000B3FD0
	virtual const char *GetSnapshotName() const;	// slot 2, 0x002AD0C0
	virtual void xfer(Xfer *xfer);				// slot 3, 0x002ADEBE
public:
	PlayerRelationHash m_map;					// +0x04
};
// SIZE 0x18

// The Player byte getter the map names Rva002AA22AByteField (retail 0x002AA22A, reads +0x734).
// Callers pass the Player as `this`; the class is only the placeholder spelling of the symbol.
class Rva002AA22AByteField
{
public:
	UnsignedByte get() const;
};

// Zero Hour's PlayerTeamList, on the TeamPrototype * pool in BFME 2 (see m_playerTeamPrototypes).
typedef _STL::list<TeamPrototype *, FreelistAllocator<TeamPrototype *> > PlayerTeamList;

struct SpecialPowerReadyTimerType
{
	UnsignedInt m_templateID;
	UnsignedInt m_readyFrame;
};

class Player : public Snapshot, public ScienceQueryInterface, public SciencePurchasePointTracker
{
public:
	Player(Int playerIndex);		// 0x002B0F3C
	virtual ~Player();			// 0x002B11A7

protected:
	virtual void loadPostProcess() {}	// slot 1, the shared empty body 0x000B3FD0
	virtual const char *GetSnapshotName() const;	// slot 2, 0x002B117C
public:	// the map's ?DoXfer@Player@@QAEXPAVXfer@@@Z for slot 3 is public
	virtual void xfer(Xfer *xfer);		// slot 3, 0x002B1522

public:
	virtual void computeSuperweaponTarget(const SpecialPowerTemplate *power, Coord3D *pos, Int playerIndex, Real weaponRadius);	// slot 4, 0x002A9B92
	virtual Bool checkBridges(Object *unit, Waypoint *way);	// slot 5, 0x002A9C0D
	virtual Bool getAiBaseCenter(Coord3D *pos);		// slot 6, 0x002A9C26
	virtual void repairStructure(ObjectID structureID);	// slot 7, 0x002A9C48

	// ScienceQueryInterface (+0x04); inline, emitted with Player's tables.
	virtual Bool queryHasScience(ScienceType science) const { return hasScience(science); }	// 0x002B118A
	virtual Int queryPurchasePoints() const { return getSciencePurchasePoints(); }		// 0x0030D377 (fold)
	virtual void queryScienceHidden(ScienceType science, Bool *hidden) const { *hidden = isScienceHidden(science); }	// 0x002B1192

	// SciencePurchasePointTracker (+0x08).
	virtual Bool setRankLevel(Int rankLevel);			// 0x002A9FA0
	virtual void addSciencePurchasePoints(Int delta);	// 0x002A9F5E
	virtual void resetRank();							// 0x002AE318
	virtual void grantRankBonuses(const RankInfo *rank);	// 0x002AD7E9
	virtual Bool usesRankLevelLimitOverride() const;	// 0x002A9F53

	Int iterateObjects(Int (*func)(Object *obj, void *userData), void *userData) const;	// 0x002AB08B
	UnsignedByte rva002AA245() const;	// 0x002AA245: the template's PlayableSide (CrateCollide calls it); UnsignedByte as the retail movzx shows
	AsciiString getBaseSide() const;	// 0x0004F833
	AcademyStats * getAcademyStats();	// 0x000B6464
	bool isPlayerDead() const;	// 0x0028A7D8
	void setPlanningModeState(Int state);	// 0x002AA257 (caller 0x00377613; row_attributions: Player)
	Bool isPlayerActive();	// 0x002AA231 Zero Hour's body: !m_observer && !m_isPlayerDead (Zero Hour's name, map ?isPlayerActive@Player@@QAE_NXZ; RebuildHoleExposeDie::onDie 0x004867FE)
	void accumulateRTSBattleStatsIntoLivingWorldScoreKeeper();	// 0x002A9994
	void setBuildList(BuildListInfo *buildList);	// 0x002A99D2 (PlayerList::newGame 0x002A8008)
	void rva002A99FA();	// 0x002A99FA
	void setPlayerType(PlayerType, bool);	// 0x002A9A18
	Int getPlayerDifficulty() const;	// 0x002A9BF2 (Team::setTeamTargetObject 0x0039D84A)
	Bool removeTeamRelationship(const Team *that);	// 0x002AD19E (Zero Hour's name; PlayerList 0x002A7AFE)
	void setTeamRelationship(const Team *that, Relationship r);	// 0x002ADFC7 (Zero Hour's name; ScriptActions::doPlayerSetOverrideRelationToTeam 0x003C4350)
	void guardSupplyCenter(Team *team, Int minSupplies);	// 0x002A9CDE: forwards to the AIPlayer at +0x2DC (ScriptActions 0x003C1F6C)
	Bool okToPlayRadarEdgeSound();	// 0x002AA04C (Zero Hour's name; addRadar 0x002AA91F, removeRadar 0x002AA9C2, disableRadar 0x002AAA65, enableRadar 0x002AAAF9)
	bool isSkirmishAIPlayer();	// 0x002A9B7B
	Energy *getEnergy() { return &m_energy; }			// Zero Hour's; inline (ScriptConditions 0x003E417D: lea ecx,[player+0x1BC])
	const Energy *getEnergy() const { return &m_energy; }
	// Zero Hour's AI forwarders (each tests the AIPlayer at +0x2DC first):
	Player *getCurrentEnemy();	// 0x002A9BBD (AIPlayer slot 12, getAiEnemy)
	void preTeamDestroy(const Team *team);	// 0x002A9CF0 (AIPlayer::aiPreTeamDestroy 0x004F0819)
	void buildUpgrade(const AsciiString &upgrade);	// 0x002A9DEC (AIPlayer::buildUpgrade 0x004F2C14)
	Bool getListInScoreScreen();	// 0x002A9DA5 (+0x33C)
	void setListInScoreScreen(Bool listInScoreScreen);	// 0x002A9D98 (+0x33C; Zero Hour's name)
	void addNewSharedSpecialPowerTimer(const SpecialPowerTemplate *power, UnsignedInt frame);
	void resetOrStartSpecialPowerReadyFrame(const SpecialPowerTemplate *power);
	void expressSpecialPowerReadyFrame(const SpecialPowerTemplate *power, UnsignedInt frame);
	UnsignedInt getOrStartSpecialPowerReadyFrame(const SpecialPowerTemplate *power);
	Real getSpellRechargeModifier() const;	// 0x002AA0B1
	Int getSquadNumberForObject(const Object *objToFind) const;	// 0x002AA191
	void setColorsFromDict(const Dict *d);	// 0x002A9ACC (Zero Hour's colour block of initFromDict; the map types the argument const Dict *)
	void removeObjectFromHotkeySquad(Object *objToRemove);	// 0x002AA1BF
	Int getSpellRechargeModifierCount() const;	// 0x002AA0B8
	void incSpellRechargeModifierCount();	// 0x002AA0BF
	void decSpellRechargeModifierCount();	// 0x002AA0C6
	void setSpellRechargeModifier(Real modifier);	// 0x002AA0CD
	void onUnitCreated(Object *, Object *);	// 0x002A9C5F
	Bool isSupplySourceSafe(Int minSupplies);	// 0x002A9CA4 (Zero Hour's AI forwarder: the AIPlayer's answer, else true; map placeholder rva002A9CA4)
	bool rva002A9CB8();	// 0x002A9CB8
	void removePhantomStructureID(ObjectID objectID); // 0x002ACECC
	void onStructureConstructionComplete(Object *, Object *, bool);	// 0x002A9D02
	bool isLocalPlayer() const;	// 0x002A9D89
	int getSupplyBoxValue();	// 0x002A9DAC
	void rva002A9E25(float);	// 0x002A9E25
	int ScaleMoney(int);	// 0x002A9E36
	bool hasPrereqsForScience(ScienceType) const;	// 0x002A9EAC
	bool rva002A9ED9(SpecialPowerTemplate const *);	// 0x002A9ED9
	bool hasAnyRequiredSciences(const _STL::vector<ScienceType> &) const;	// 0x002AB7FD (SpecialPowerStore::canUseSpecialPower proves it)
	unsigned char rva002AA00C(ThingTemplate *, int);	// 0x002AA00C
	Upgrade * findUpgrade(UpgradeTemplate const *);	// 0x002AA031
	bool hasRadar() const;	// 0x002AA08E
	bool doesObjectQualifyForBattlePlan(Object *) const;	// 0x002AA0DE
	void getCurrentSelectionAsAIGroup(AIGroup *);	// 0x002AA164
	void onStructureCreated(Object *, Object *);	// 0x002AA559
	bool addSkillPointsForDamage(Object *, Object const *, float);	// 0x002AA5E1
	bool addSkillPointsForKill(Object const *, Object const *);	// 0x002AA6DE
	void rva002AA756(int);	// 0x002AA756
	void deleteUpgradeList();	// 0x002AA7E6
	ThingTemplate const * rva002AA857(int);	// 0x002AA857
	Bool hasUpgradeInProduction(const UpgradeTemplate *upgradeTemplate) const;	// 0x002AA8EF (Zero Hour's name: the template's bit in the set at +0xBC; map placeholder rva002AA8EF)
	// 0x002AA8BF: the completed-upgrade bit maskIndex in the set at +0x13C (GenericObjectCreationNugget 0x001F1513 tests it).
	Bool testBit(Int maskIndex) const;		// 0x002AA8BF (map ?testBit@Player@@QBE_NH@Z)
	void addRadar(bool);	// 0x002AA91F
	void removeRadar(bool);	// 0x002AA9C2
	void disableRadar();	// 0x002AAA65
	void enableRadar();	// 0x002AAAF9
	void rva002AABBC();	// 0x002AABBC
	void addToBuildList(Object *);	// 0x002AAEC5
	void addToPriorityBuildList(AsciiString const &, Coord3D *, float);	// 0x002AAF72
	void healAllObjects();	// 0x002AB06A
	void countObjectsByThingTemplate(int, ThingTemplate const *const *, bool, int *, bool) const;	// 0x002AB0C5
	int countBuildings();	// 0x002AB10F
	int countObjects(KindOfMaskType setMask, KindOfMaskType clearMask);	// 0x002AB138 (sums TeamPrototype::countObjects over the prototype list at +0x32C)
	Object * findClosestObjectToPosWithValidLivingWorldArmyID(void const *) const;	// 0x002AB22A
	bool rva002AB260(bool) const;	// 0x002AB260
	bool rva002AB312() const;	// 0x002AB312
	bool hasAnyObjects(bool) const;	// 0x002AB3C5
	bool rva002AB3FA() const;	// 0x002AB3FA
	void updateTeamStates();	// 0x002AB429
	void doBountyForKill(Object const *, Object const *);	// 0x002AB63D
	bool hasScience(ScienceType) const;	// 0x002AB7D5
	Bool isScienceDisabled(ScienceType science) const;	// 0x002AB82D (searches m_sciencesDisabled; the map types the argument CreateAHeroData *)
	Bool isScienceHidden(ScienceType science) const;	// 0x002AB855 (searches m_sciencesHidden; the map types the argument CreateAHeroData *)
	bool rva002AB87D(UpgradeTemplate const *) const;	// 0x002AB87D
	void onPowerBrownOutChange(bool);	// 0x002AB8D0
	void rva002AB8FB(Object *, bool);	// 0x002AB8FB
	void setUnitsVisionSpied(bool, int);	// 0x002ABA3D
	void rva002ABA9B();	// 0x002ABA9B
	void rva002ABAA6();	// 0x002ABAA6
	void rva002ABAB3();	// 0x002ABAB3
	void setDefaultTeam();	// 0x002ABC7E
	int rva002ABCF0(unsigned char, int *);	// 0x002ABCF0
	int rva002ABD1D(int, int);	// 0x002ABD1D
	void removeTeamFromList(TeamPrototype *);	// 0x002ABD48
	void setObjectsEnabled(AsciiString const &, bool);	// 0x002ABD93
	void sellEverythingUnderTheSun();	// 0x002AB5C7 (ScriptActions::doPlayerSellEverything 0x003BB2DF)
	void killPlayer();	// 0x002AB44A (ScriptActions::doPlayerKill 0x003BB518)
	void setPlayerRelationship(const Player *that, Relationship relationship);	// 0x002ADF9C (PlayerList::newGame 0x002A8008)
	bool allowedToBuild(ThingTemplate const *) const;	// 0x002ABE24
	void allowToBuild(const ThingTemplate *tmpl);	// 0x002ABFA0: takes the template ID off m_forbiddenTemplateIDs
	void forbidToBuild(const ThingTemplate *tmpl);	// 0x002ACEDF: appends the template ID to m_forbiddenTemplateIDs
	bool isCapableOfPurchasingScience(ScienceType) const;	// 0x002ABE86
	ScienceAvailabilityType getScienceAvailabilityTypeFromString(AsciiString const &);	// 0x002ABEC9
	bool canBuild(ThingTemplate const *) const;	// 0x002ABEF1
	Relationship getRelationship(Player const *) const;	// 0x002AC3E0
	bool rva002AC43F(int *);	// 0x002AC43F
	bool rva002AC4D4(int *);	// 0x002AC4D4
	Object * findNaturalCommandCenter();	// 0x002AC60A
	Object * rva002AC629();	// 0x002AC629
	void rva002AC673();	// 0x002AC673
	void changeBattlePlan(BattlePlanStatus plan, Int delta, BattlePlanBonuses *bonuses);	// 0x002ADB4F (map: ?changeBattlePlan@Player@@QAEXW4BattlePlanStatus@@HPAVRva002AA14DBonuses@@@Z; BattlePlanUpdate 0x00497AE8 calls it)
	void applyBattlePlanBonusesForObject(Object *) const;	// 0x002AC917
	void removeBattlePlanBonusesForObject(Object *) const;	// 0x002AC92B
	void processCreateTeamGameMessage(int, GameMessage const *);	// 0x002ACA86
	void processSelectTeamGameMessage(int, GameMessage *);	// 0x002ACAFB
	void processAddTeamGameMessage(int, GameMessage *);	// 0x002ACBA8
	void setCurrentlySelectedAIGroup(AIGroup *);	// 0x002ACC53
	void addAIGroupToCurrentSelection(AIGroup *);	// 0x002ACCA3
	void rva002ACD09(void *, int);	// 0x002ACD09
	// (CostModifierUpgrade 0x004B5B4C, 0x004B5A65, 0x004B5AC0):
	void removeProductionCostModifierByID(void *data, int objectID);	// 0x002ADC03
	void addProductionCostModifierByTemplate(void *data, const AsciiString &templateName);	// 0x002B212A
	void removeProductionCostModifierByTemplate(void *data, const AsciiString *templateName);	// 0x002B0E55
	float getProductionCostChangeBasedOnTemplate(ThingTemplate const *, bool);	// 0x002ACD6B
	int getProductionCostChangeByObjectFilterID(void *);	// 0x002ACE2B
	Relationship getRelationship(Team const *) const;	// 0x002AD0C6
	Relationship getRelationship(Object const *) const;	// 0x002AD11E
	void rva002AD1FD();	// 0x002AD1FD
	void rva002AD25B(AsciiString const &);	// 0x002AD25B
	void becomingTeamMember(Object *, bool);	// 0x002AD4EC
	int rva002AD629(AsciiString const &);	// 0x002AD629
	bool attemptToPurchaseScience(ScienceType);	// 0x002AD826
	bool grantScience(ScienceType);	// 0x002AD85E
	bool forcePurchaseScience(ScienceType);	// 0x002AD883
	void setScienceAvailability(ScienceType, ScienceAvailabilityType);	// 0x002AD8C9
	void onUpgradeCompleted(UpgradeTemplate const *, int);	// 0x002AD93A
	void onUpgradeRemoved(UpgradeTemplate const *, int);	// 0x002AD9FD
	void removeUpgrade(UpgradeTemplate const *, int);	// 0x002ADAC3
	float getProductionCostChangeBasedOnUpgradeDiscount(AsciiString) const;	// 0x002ADC82
	void becomingLocalPlayer(bool);	// 0x002ADFF2
	void addTeamToList(TeamPrototype *);	// 0x002AE228
	void rva002AE252();	// 0x002AE252
	void rva002AE2ED(Player *);	// 0x002AE2ED
	Upgrade * addUpgrade(UpgradeTemplate const *, UpgradeStatusType, int);	// 0x002AE329
	void rva002AE475(void *);	// 0x002AE475
	void update();	// 0x002AE770
	void rva002AE8CF(AsciiString const *, float, int);	// 0x002AE8CF
	void rva002AE930(AsciiString const *);	// 0x002AE930
	void setUnitsShouldHunt(bool, CommandSourceType);	// 0x002AE9B8
	void transferAssetsFromThat(Player *, bool);	// 0x002AEA9F
	void garrisonAllUnits(CommandSourceType);	// 0x002AEEA7
	void ungarrisonAllUnits(CommandSourceType);	// 0x002AF037
	void setUnitsShouldIdleOrResume(bool);	// 0x002AF0C5
	void initPlayerUpgrades();	// 0x002AF19B
	void rva002AF614(void *);	// 0x002AF614
	bool rva002AB2D9(BfmeTab1026 *filter, bool flag) const;	// 0x002AB2D9 (SpecialPowerModule::rva00493251 passes a module-data ObjectFilter)
	void init(PlayerTemplate const *);	// 0x002AF729
	void initFromDict(Dict const *);	// 0x002AFCDF
	bool cloneFrom(Player *);	// 0x002B07A9
	void initBuildableHeroes();	// 0x002B0B96
	void newMap();	// 0x002B0D00

private:
	Bool addScience(ScienceType science);	// 0x002AD661 (the map spells it private)
public:
	const PlayerTemplate *getPlayerTemplate() const { return m_playerTemplate; }
	// Colour getters are inline: their out-of-line COMDATs are 0x0004228B (+0x280) and
	// 0x0028A7B0 (+0x284), which the map names only by placeholder. The map's
	// getPlayerNightColor at 0x00509D29 reads +0x128: it is a folded getter of another class.
	Int getPlayerColor() const { return m_color; }
	Int getPlayerNightColor() const { return m_playerNightColor; }
	NameKeyType getPlayerNameKey() const { return m_playerNameKey; }
	Int getPlayerIndex() const { return m_playerIndex; }
	UnsignedInt getAttackedFrame() const { return m_attackedFrame; }	// inline (AIPlayer::isSupplySourceAttacked 0x004F1138 reads +0x354)
	Bool getCanBuildUnits() const { return m_canBuildUnits; }
	void setCanBuildUnits(Bool canBuild) { m_canBuildUnits = canBuild; }
	const PlayerTeamList *getPlayerTeams() const { return &m_playerTeamPrototypes; }	// inline (AIPlayer::isSupplySourceAttacked walks it)
	Int getNumBattlePlansActive() const { return m_bombardBattlePlans + m_holdTheLineBattlePlans + m_searchAndDestroyBattlePlans; }	// Zero Hour's
	UnsignedInt getPlayerMask() const { return 1 << m_playerIndex; }	// Zero Hour's (processSelectTeamGameMessage 0x002ACAFB: 1 << +0x54)
	Int getLightPoints() const;	// 0x002A9ED2 (ScriptConditions evaluatePlayerCompareLightPoints 0x003E4F79)
	UnsignedByte getAttackedByFlag(Int playerIndex) const;	// 0x002AA201 (ScriptConditions 0x003E4BFA)
	void setAttackedByFlag(Int playerIndex);	// 0x002AA1E4
	const AsciiString &getSide() const { return m_side; }
	Team *getDefaultTeam() const { return m_defaultTeam; }
	Bool isPlayerObserver() const;	// 0x002AA223: reads +0x33A (an ICF fold: the map spells the shared body ParticleSystem::isSaveable)
	ScoreKeeper *getScoreKeeper() { return &m_scoreKeeper; }	// inline: ScoredKillTracker 0x0055A962 adds 0x3BC to the player
	Command *getCommandPoints() { return &m_command; }
	// 0x002A9B35 / 0x002A9B58: an object's command points join or leave the account (Object::addAttributeModifierToPool
	// 0x0028EA91 calls both around a pool change; the map spells the parameter with a placeholder struct).
	void addCommandPoints(Object *obj);
	void removeCommandPoints(Object *obj);
	Money *getMoney() { return &m_money; }
	const Money *getMoney() const { return &m_money; }

	const PlayerTemplate *m_playerTemplate;	// +0x34
	UnicodeString m_playerDisplayName;	// +0x38
	Handicap m_handicap;			// +0x3C (built by 0x003B0E83)
	AsciiString m_playerName;		// +0x4C
	NameKeyType m_playerNameKey;		// +0x50
	Int m_playerIndex;			// +0x54 the ctor's argument
	AsciiString m_side;			// +0x58
	Int m_playerType;			// +0x5C a PlayerType
	Command m_command;			// +0x60 the command-point account (Common/Command.h; ctor 0x002AE5EB); addCommandPoints 0x002A9B35 forwards to it
	Money m_money;		// +0x90 table 0x00BE1AA0, balance94 and owner index98
	Upgrade *m_upgradeList;			// +0x9C head of the player's Upgrade chain (findUpgrade 0x002AA031 follows +0xC)
	Int m_radarCount;			// +0xA0 addRadar 0x002AA91F increments it; hasRadar 0x002AA08E tests > 0
	Int m_disableProofRadarCount;		// +0xA4 addRadar(true) also increments this one
	Bool m_radarDisabled;			// +0xA8 disableRadar 0x002AAA65 sets it
	unsigned char m_unk0xA9[3];		// +0xA9
	// Zero Hour's three battle-plan counters (becomingTeamMember 0x002AD4EC sums +0xAC, +0xB0 and +0xB4).
	Int m_bombardBattlePlans;		// +0xAC
	Int m_holdTheLineBattlePlans;		// +0xB0
	Int m_searchAndDestroyBattlePlans;	// +0xB4
	BattlePlanBonuses *m_battlePlanBonuses;	// +0xB8 applyBattlePlanBonusesForObject 0x002AC917 hands it to localApplyBattlePlanBonusesToObject
	UpgradeMaskType m_upgradesInProgress;	// +0xBC constructed by 0x001EAE6F; addUpgrade 0x002AE329 sets the template's bit for UPGRADE_STATUS_IN_PRODUCTION
	UpgradeMaskType m_upgradesCompleted;	// +0x13C constructed by 0x001EAE6F; addUpgrade 0x002AE329 moves the bit here on UPGRADE_STATUS_COMPLETE (Zero Hour's two members)
	Energy m_energy;			// +0x1BC the ctor 0x002B0F3C builds it inline at 0x002B0FD9 (table 0x00BFDC48, three zeroed words, then an EH state: it has a destructor); ScriptConditions 0x003E417D takes its address
	unsigned char m_unk0x1CC[0xAC];		// +0x1CC ctor 0x004F599E
	BuildListInfo *m_buildList;		// +0x278
	Int m_unk0x27C;				// +0x27C
	Int m_color;				// +0x280
	Int m_playerNightColor;			// +0x284
	unsigned char m_unk0x288[0xC];		// +0x288 _STL::map<unsigned char, int> (ctor 0x0033C432)
	unsigned char m_unk0x294[0x14];		// +0x294 _STL::hash_map<int, ?> (ctor 0x001FEAB2)
	unsigned char m_unk0x2A8[0x14];		// +0x2A8 _STL::hash_map<int, ?> (ctor 0x001FEAB2)
	unsigned char m_unk0x2BC[0x14];		// +0x2BC _STL::hash_map<int, ?> (ctor 0x002AF625)
	_STL::map<Int, Int> m_intValuesByNameKey;	// +0x2D0 The player looks up integer values here by the key of an AsciiString.
	AIPlayer *m_ai;				// +0x2DC
	Int m_mpStartIndex;			// +0x2E0
	ResourceGatheringManager *m_resourceGatheringManager;	// +0x2E4
	TunnelTracker *m_tunnelSystem;		// +0x2E8
	Team *m_defaultTeam;			// +0x2EC
	_STL::vector<ScienceType> m_sciences;		// +0x2F0 hasScience 0x002AB7D5 searches it (ctor: _Vector_base 0x00211E58)
	_STL::vector<ScienceType> m_sciencesDisabled;	// +0x2FC
	_STL::vector<ScienceType> m_sciencesHidden;	// +0x308
	Real m_scavengerBountyPercent;	// +0x314 ScavengerSpecialPower sets this percentage and clears it while the power is paused.
	Int m_unk0x318;						// +0x318
	Int m_lightPoints;					// +0x31C getLightPoints 0x002A9ED2
	unsigned char m_unk0x320[0xC];		// +0x320 ctor 0x002AC340, virtual dtor 0x002AC366
	// Pooled, like ObjectIDList: _M_create_node 0x002AC04E takes its cell from the TeamPrototype * pool.
	PlayerTeamList m_playerTeamPrototypes;	// +0x32C (the dtor clears it)
	PlayerRelationMap *m_playerRelations;	// +0x330 getRelationship(const Player *) 0x002AC3E0 looks players up here; the dtor deletes it
	TeamRelationMap *m_teamRelations;	// +0x334 the dtor deletes it
	Bool m_canBuildUnits;			// +0x338
	Bool m_canBuildBase;			// +0x339
	Bool m_observer;			// +0x33A
	Bool m_unk0x33B;			// +0x33B
	Bool m_listInScoreScreen;		// +0x33C
	Bool m_unitsShouldHunt;			// +0x33D
	unsigned char m_unk0x33E[0x1];		// +0x33E
	Bool m_unk0x33F;			// +0x33F
	Bool m_attackedBy[0x10];		// +0x340 Zero Hour's m_attackedBy[MAX_PLAYER_COUNT] after m_unitsShouldHunt; getAttackedByFlag 0x002AA201 reads [index]
	unsigned char m_unk0x350[4];	// +0x350
	UnsignedInt m_attackedFrame;	// +0x354 (AIPlayer::isSupplySourceAttacked 0x004F1138 adds 10 and compares it with the frame)
	Int m_visionSpiedBy[20];	// +0x358 Generals' reference count per player (setUnitsVisionSpied 0x002ABA3D walks 20 counters)
	UnsignedInt m_visionSpiedMask;	// +0x3A8 rebuilt from the counters by setUnitsVisionSpied; read by 0x002AA21C
	Int m_livingWorldPlayerID;		// +0x3AC -1 in the ctor; ScaleMoney 0x002A9E36 passes it to TheLivingWorldLogic->findPlayer, PlayerList 0x002A7A6F looks players up by it
	unsigned char m_unk0x3B0[0xC];		// +0x3B0 a vector
	ScoreKeeper m_scoreKeeper;		// +0x3BC
	unsigned char m_unk0x6F4[0x4];		// +0x6F4 a list
	// SpellRechargeModifierUpgrade's player state: upgradeImplementation 0x004B5F9F counts up
	// (0x002AA0BF) and sets the modifier from its table by the count (0x002AA0CD);
	// upgradeRemovalImplementation 0x004B5E46 counts down (0x002AA0C6); SpecialPowerModule::
	// startPowerRecharge 0x0049369D reads the modifier (0x002AA0B1).
	Real m_spellRechargeModifier;		// +0x6F8
	Int m_spellRechargeModifierCount;	// +0x6FC
	_STL::list<UnsignedShort> m_forbiddenTemplateIDs;	// +0x700 template IDs this player may not build: allowedToBuild 0x002ABE24
									// searches it for ThingTemplate +0x5D8 (find<_List_iterator<short> >); the ctor 0x002B0F3C builds it
	typedef _STL::list<SpecialPowerReadyTimerType> SpecialPowerReadyTimerList;
	SpecialPowerReadyTimerList m_specialPowerReadyTimerList;	// +0x704
	Squad *m_squads[10];			// +0x708 Zero Hour's NUM_HOTKEY_SQUADS hotkey squads (zeroed by the ctor; 0x002AA191, 0x002AA1BF)
	Squad *m_currentSelection;		// +0x730 Zero Hour's: the dtor deletes it; getCurrentSelectionAsAIGroup 0x002AA164 calls 0x004D6E95, which sits among Squad's methods (isOnSquad 0x004D6BCD, xfer 0x004D6EC4)
	Bool m_isPlayerDead;			// +0x734 Zero Hour's member after m_currentSelection; isPlayerActive 0x002AA231 tests it with m_observer; read by 0x002AA22A (14 callers; map: placeholder ?get@Rva002AA22AByteField)
	unsigned char m_unk0x735[0x3];		// +0x735
	UnitRevivalTracker m_unitRevivalTracker;	// +0x738
	AsciiString m_unk0x74C;			// +0x74C
	Int m_planningModeState;		// +0x750 planning-mode state (evaluatePlayerIsInPlanningMode 0x003E54B4 tests == 2; setPlanningModeState 0x002AA257)
	ObjectIDList m_phantomStructureIDs;	// +0x754; 0x002ACECC removes an ID with pooled erase 0x002ABB20
};
// SIZE 0x758
typedef char PlayerTemplateOffset[(offsetof(Player, m_playerTemplate) == 0x34) ? 1 : -1];
typedef char PlayerRelationsOffset[(offsetof(Player, m_playerRelations) == 0x330) ? 1 : -1];
typedef char PlayerSize[(sizeof(Player) == 0x758) ? 1 : -1];
typedef char PlayerCommandOffset[(offsetof(Player, m_command) == 0x60 && sizeof(Command) == 0x30) ? 1 : -1];
typedef char PlayerIndexOffset[(offsetof(Player, m_playerIndex) == 0x54) ? 1 : -1];
typedef char PlayerDefaultTeamOffset[(offsetof(Player, m_defaultTeam) == 0x2EC) ? 1 : -1];
typedef char PlayerCanBuildUnitsOffset[(offsetof(Player, m_canBuildUnits) == 0x338) ? 1 : -1];
typedef char PlayerRevivalOffset[(offsetof(Player, m_unitRevivalTracker) == 0x738) ? 1 : -1];
typedef char PlayerSciencesOffset[(offsetof(Player, m_sciences) == 0x2F0 && offsetof(Player, m_scavengerBountyPercent) == 0x314) ? 1 : -1];
typedef char PlayerRadarOffset[(offsetof(Player, m_radarDisabled) == 0xA8) ? 1 : -1];
typedef char PlayerScoreKeeperOffset[(offsetof(Player, m_scoreKeeper) == 0x3BC) ? 1 : -1];

#endif
