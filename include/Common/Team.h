// Team.h: a team instance (TeamFactory builds them from TeamPrototypes). SIZE 0x13C.
// Target facts (code RVAs, tables VAs):
//  - Size 0x13C: TeamFactory::createEmptyTeam allocates 0x13C before
//    ??0Team(TeamPrototype *, Int) 0x003A39A7.
//  - Bases: the ctor stores Snapshot's table at +0x00, constructs a base at
//    +0x04 (ctor 0x0055B048, virtual dtor 0x0055B0CC), then stores Team's
//    tables 0x00C1AEFC (+0x00, 4 slots) and 0x00C1AEB8 (+0x04, 17 slots). The
//    +0x04 table carries AIBuildable's addNeed and registerWithBuilder, and
//    Team's getLocation 0x0039D7DB (slot 13): the base is AIBuildable
//    (include/GameLogic/SkirmishAI/AIBuildable.h, SIZE 0x2C).
//  - Members: the ctor's stores and constructors and the dtor 0x003A354F:
//    prototype +0x30 and id +0x34 (the ctor's arguments), a string at +0x44
//    (StringBase<char> release), +0x48 a sub-object (ctor 0x003A393A),
//    TeamRelationMap* +0x118 and PlayerRelationMap* +0x11C (the ctor news
//    0x18 bytes and calls each class's ctor), a list at +0x12C and a
//    set<AsciiString> at +0x130.
// Methods: every Team row of the map with the signature its decorated name
// states, except rows that need placeholder (Rva/Bfme) or template types.
#pragma once
#ifndef BFME2_TEAM_H
#define BFME2_TEAM_H

#include <stddef.h>
#include "Common/GameType.h"
#include "Common/AsciiString.h"
#include "Common/Coord3D.h"
#include "Common/Snapshot.h"
#include "Common/DLinkIterator.h"
#include "Common/BitFlags.h"
#include <list>
#include "GameLogic/SkirmishAI/AIBuildable.h"
#include "Common/STLTypedefs.h"
#include "Common/StlHashMap.h"

class Xfer;
class AIGroup;
class Dict;
class Script;
class TeamFactory;
class Object;
class ObjectTypes;
class Player;
class PlayerRelationMap;
class PolygonTrigger;
class TeamPrototype;
class TeamRelationMap;
class ThingTemplate;
class ObjectFilter;
enum Relationship;

class Team : public Snapshot, public AIBuildable
{
	friend void deleteTeamCallback(Team *team);	// 0x003A33A7 deletes through the protected dtor
	friend class TeamPrototype;			// updateState 0x003A34AC deletes retired instances
public:
	Team(TeamPrototype *proto, Int id);	// 0x003A39A7
protected:
	virtual ~Team();			// 0x003A354F (the map spells it protected)

	virtual void loadPostProcess();			// slot 1, 0x003A2339
	virtual const char *GetSnapshotName() const;	// slot 2, 0x003A3709
	virtual void xfer(Xfer *xfer);			// slot 3, 0x003A2072

	// AIBuildable overrides (table 0x00C1AEB8 at +4)
	virtual Real getPriority() const;		// slot 2, 0x0039D840 (the prototype's production priority)
public:
	virtual Coord3D getLocation() const;		// slot 13, 0x0039D7DB
	virtual Bool vslot14() const { return false; }	// slot 14, folded 0x0047A699
	virtual Bool vslot15(Int) { return true; }	// slot 15, folded 0x005CB9FA
	virtual Int vslot16(Int) { return 0; }		// slot 16, folded 0x005DAA6C

public:
	DLINK_ITERATOR<Object> iterate_TeamMemberList() const;	// 0x00263864 (COMDAT)
	bool isInList_TeamMemberList(Object *) const;	// 0x0028A74F
	void prependTo_TeamMemberList(Object *);	// 0x0028A75F
	void removeFrom_TeamMemberList(Object *);	// 0x0028A77F
	// The instance-list links are inline: retail emits them as out-of-line copies
	// (0x0039D40F, 0x0039D429, 0x0039D440) and their callers reload ecx after the
	// call, as a caller does when the callee's body is a COMDAT.
	bool dlink_isInList_TeamInstanceList(Team *const *listHead) const
	{
		return *listHead == this || m_dlinkPrev_TeamInstanceList || m_dlinkNext_TeamInstanceList;
	}
	void dlink_prependTo_TeamInstanceList(Team **listHead)
	{
		m_dlinkNext_TeamInstanceList = *listHead;
		if (*listHead)
			(*listHead)->m_dlinkPrev_TeamInstanceList = this;
		*listHead = this;
	}
	void dlink_removeFrom_TeamInstanceList(Team **listHead)
	{
		if (m_dlinkNext_TeamInstanceList)
			m_dlinkNext_TeamInstanceList->m_dlinkPrev_TeamInstanceList = m_dlinkPrev_TeamInstanceList;
		if (m_dlinkPrev_TeamInstanceList)
			m_dlinkPrev_TeamInstanceList->m_dlinkNext_TeamInstanceList = m_dlinkNext_TeamInstanceList;
		else
			*listHead = m_dlinkNext_TeamInstanceList;
		m_dlinkPrev_TeamInstanceList = 0;
		m_dlinkNext_TeamInstanceList = 0;
	}
	Player * getControllingPlayer() const;	// 0x0039D7CF
	void setTeamTargetObject(Object *);	// 0x0039D84A
	void rva0039D889(bool);	// 0x0039D889
	void rva0039D8BA(Object *);	// 0x0039D8BA
	void rva0039D8D3(unsigned int);	// 0x0039D8D3
	void setControllingPlayer(Player *);	// 0x0039D97E
	int getTargetableCount() const;	// 0x0039D9E3
	Coord3D * rva0039DA2A(Coord3D *) const;	// 0x0039DA2A
	bool rva0039DAF4() const;	// 0x0039DAF4
	void rva0039DB31(void const *);	// 0x0039DB31
	void countObjectsByThingTemplate(int, ThingTemplate const *const *, bool, int *, bool) const;	// 0x0039DB89
	int rva0039DC05(int, bool, bool) const;	// 0x0039DC05
	int countBuildings() const;	// 0x0039DC63
	int rva0039DD12(int (*visit)(Object *member, void *userData), void *userData) const;	// 0x0039DD12: 0 as soon as the callback returns 0 for a member, else 1
	void healAllObjects();	// 0x0039DCE9
	bool rva0039DD4B(bool);	// 0x0039DD4B
	bool hasAnyUnits();	// 0x0039DEC4
	Bool hasAnyBuildings(KindOfMaskType kindOf, Bool ignoreIncomplete);	// 0x0039DDC2
	Bool hasAnyBuildings(ObjectFilter *filter, Bool ignoreIncomplete);	// 0x0039DE46
	Bool hasAnyBuildings(KindOfMaskType setMask, KindOfMaskType clearMask);	// 0x0039DF1C
	Bool hasAnyUnits(ObjectFilter *filter);	// 0x0039DF87
	bool isIdle() const;	// 0x0039DFF8
	bool hasAnyObjects(bool);	// 0x0039E042
	bool rva0039E0D0();	// 0x0039E0D0
	bool didAllEnter(PolygonTrigger *, unsigned int);	// 0x0039E15D
	bool didPartialEnter(PolygonTrigger *, unsigned int);	// 0x0039E20D
	bool didPartialExit(PolygonTrigger *, unsigned int);	// 0x0039E288
	bool didAllExit(PolygonTrigger *, unsigned int);	// 0x0039E303
	bool allInside(PolygonTrigger *, unsigned int);	// 0x0039E3C8
	bool noneInside(PolygonTrigger *, unsigned int);	// 0x0039E474
	bool someInsideSomeOutside(PolygonTrigger *, unsigned int);	// 0x0039E50D
	void rva0039E5B9(Coord3D *);	// 0x0039E5B9
	void transferUnitsTo(Team *);	// 0x0039E660
	bool damageTeamMembers(float);	// 0x0039E76C
	void moveTeamTo(Coord3D);	// 0x0039E7F2
	bool hasAnyBuildFacility();	// 0x0039E815
	float rva0039E84B(Object *);	// 0x0039E84B
	Object * rva0039E8EB();	// 0x0039E8EB
	bool rva0039E8FF(int);	// 0x0039E8FF
	Object * rva0039E968(int);	// 0x0039E968
	Coord3D * rva0039E9A1(Coord3D *) const;	// 0x0039E9A1
	void disband();	// 0x0039E9E0
	void setAttackPriorityName(AsciiString);	// 0x0039F094
	bool rva0039F0D2();	// 0x0039F0D2
	void updateState();	// 0x0039F0E3
	bool rva0039F824();	// 0x0039F824
	void notifyTeamOfObjectDeath(Object *);	// 0x0039F835
	void updateGenericScripts();	// 0x0039F8F7
	void getTeamAsAIGroup(AIGroup *);	// 0x003A0F62
	Relationship getRelationship(Team const *) const;	// 0x003A0FD2
	Object * getTeamTargetObject();	// 0x003A105B
	bool rva003A10C0(UnsignedInt teamID) const;	// 0x003A10C0 (TeamRelationHash is keyed UnsignedInt: retail hands the argument slot to _M_find by reference, no converted copy)
	bool rva003A10DF(int) const;	// 0x003A10DF
	void deleteTeam(bool);	// 0x003A10FE
	bool recruitUnit(Object **, float *, Object *, ThingTemplate const *, Coord3D const *);	// 0x003A123C
	Object * tryToRecruit(ThingTemplate const *, Coord3D const *, float, int, int, int);	// 0x003A135B
	bool rva003A1542(ThingTemplate const *, int);	// 0x003A1542
	int rva003A178D(ObjectTypes *, int, Team *);	// 0x003A178D
	int rva003A1AA3(ThingTemplate const *, ObjectTypes *, int, float);	// 0x003A1AA3
	void evacuateTeam(bool);	// 0x003A1C3A
	void killTeam();	// 0x003A1D1A
	bool enableGenericScript(AsciiString const &, AsciiString const &, bool);	// 0x003A1F65
	void rva003A23A5(int, bool);	// 0x003A23A5
	void rva003A2CEB(int, Object *);	// 0x003A2CEB
	Bool removeOverrideTeamRelationship(NameKeyType teamID);	// 0x003A2897 (ScriptActions 0x003C09DA and 0x003C0A2E)
	Bool removeOverridePlayerRelationship(Int playerIndex);	// 0x003A28ED
	void rva003A2D53(int, Object *);	// 0x003A2D53
	void rva003A2DBB(float);	// 0x003A2DBB
	bool rva003A3717(AsciiString const &);	// 0x003A3717
	float rva003A3736(float);	// 0x003A3736
	void rva003A38E9(AsciiString const &, unsigned char);	// 0x003A38E9
	void setRecruitable(bool);	// 0x003BA5F9
	Team * dlink_next_TeamInstanceList() const;	// 0x005C4AF5

	TeamPrototype *getPrototype() const { return m_proto; }
	Int getID() const { return m_id; }
	Bool isCreated() const { return m_created; }
	void setActive() { if (!m_unk0x5C[1]) { m_unk0x5C[2] = true; m_unk0x5C[1] = true; } }	// Zero Hour's setActive (TeamInQueue::~TeamInQueue 0x004F05F0)
	Bool isActive() const { return m_unk0x5C[1]; }	// the byte at +0x5D that setActive tests and sets

	TeamPrototype *m_proto;			// +0x30
	Int m_id;				// +0x34
	Object *m_dlinkhead_TeamMemberList;	// +0x38 the member list head iterate_TeamMemberList 0x00263864 starts from
	// +0x3C/+0x40: this team's links in its prototype's instance list
	// (dlink_prependTo 0x0039D429 stores the old head at +0x40 and itself at the
	// old head's +0x3C; dlink_removeFrom 0x0039D440 clears both).
	Team *m_dlinkPrev_TeamInstanceList;	// +0x3C
	Team *m_dlinkNext_TeamInstanceList;	// +0x40
	AsciiString m_unk0x44;			// +0x44
	unsigned char m_unk0x48[0x14];		// +0x48 ctor 0x003A393A, dtor 0x003A2EBD
	Bool m_unk0x5C[8];			// +0x5C bytes the ctor clears (+0x60 is set when the prototype's two strings are not both empty)
	Int m_unk0x64;				// +0x64
	Int m_unk0x68;				// +0x68
	Int m_unk0x6C;				// +0x6C
	unsigned char m_unk0x70[0xA0];		// +0x70 a byte array at +0x70 and a word array at +0x90 the ctor fills in one loop
	Bool m_unk0x110[4];			// +0x110
	ObjectID m_teamTargetID;		// +0x114 setTeamTargetObject 0x0039D84A stores the target's ID; getTeamTargetObject 0x003A105B clears it once the target is gone
	// The ctor 0x003A39A7 news the PlayerRelationMap first and stores it at +0x11C, then the
	// TeamRelationMap at +0x118; getRelationship(const Team *) 0x003A0FD2 looks the other team's
	// ID up at +0x118 and its player's index up at +0x11C.
	TeamRelationMap *m_teamRelations;	// +0x118
	PlayerRelationMap *m_playerRelations;	// +0x11C
	Real m_unk0x120;			// +0x120
	Int m_unk0x124;				// +0x124
	Bool m_created;				// +0x128 evaluateIsDestroyed 0x003E5DE9 tests it before hasAnyObjects, as Zero Hour tests isCreated()
	_STL::list<Int> m_xferMemberIDs;	// +0x12C member IDs read by xfer; loadPostProcess 0x003A2339 checks and clears them (clear is _List_base<int>::clear 0x0023DAA5)
	unsigned char m_unk0x130[0xC];		// +0x130 _STL::set<AsciiString> (ctor 0x000D3A71)
};
// SIZE 0x13C
typedef char TeamSize[(sizeof(Team) == 0x13C) ? 1 : -1];
typedef char TeamRelationsOffset[(offsetof(Team, m_teamRelations) == 0x118) ? 1 : -1];

// A team's standing towards other teams, keyed by team ID (unsigned: xfer 0x003A2AA6 transfers the
// key through the UnsignedInt overload, slot 30, and passes that variable to operator[] as the key).
// Target facts: table VA 0x00C1ADFC (Snapshot's empty loadPostProcess 0x000B3FD0 in slot 1,
// GetSnapshotName 0x003A25AB, xfer 0x003A2AA6); ctor 0x003A3959 (Team's ctor news 0x18 bytes);
// dtor 0x003A2563 clears the table (0x001DBCDC) and destroys it. The table is STLport's
// hash_map at +0x04 (Team::getRelationship 0x003A0FD2 tests its element count at +0x14 and
// calls _M_find on it), the same shape as PlayerRelationMap's.
typedef _STL::hash_map<UnsignedInt, Relationship, rts::hash<UnsignedInt>, _STL::equal_to<UnsignedInt> > TeamRelationHash;
class TeamRelationMap : public Snapshot
{
public:
	TeamRelationMap();							// 0x003A3959
protected:
	virtual ~TeamRelationMap();					// 0x003A2563
	virtual void loadPostProcess() {}			// slot 1, the shared empty body 0x000B3FD0
	virtual const char *GetSnapshotName() const;	// slot 2, 0x003A25AB
	virtual void xfer(Xfer *xfer);				// slot 3, 0x003A2AA6
public:
	TeamRelationHash m_map;						// +0x04
};
// SIZE 0x18
typedef char TeamRelationMapSize[(sizeof(TeamRelationMap) == 0x18) ? 1 : -1];

// TCreateUnitsInfo: one unit line of a team template. SIZE 0x18.
// Name provisional (role label: TeamTemplateInfo's array of unit lines; the map
// has only placeholder names Rva0039EA9C/Rva0039D769 for its ctor and operator=).
//  - ctor 0x0039EA9C (COMDAT, run by the vector constructor iterator in
//    TeamTemplateInfo's ctor 0x003A26E6 for 7 elements of 0x18): +0 = 0, +4 = 0,
//    +8 = 1, two empty strings at +0xC/+0x10, +0x14 = -1.
//  - dtor 0x0039EAB7 releases the two strings (compiler-generated).
//  - operator= 0x0039D769 copies the three words, the two strings, then +0x14
//    (compiler-generated).
//  - TeamPrototype::addUnitInfo 0x003A0D62 matches a line on the string at +0x10
//    and the word at +0x14, then adds the argument's +0 and +4 to the line's.
class TCreateUnitsInfo
{
public:
	TCreateUnitsInfo() : m_minUnits(0), m_maxUnits(0), m_unk0x08(1), m_unk0x14(-1) {}

	void xfer(Xfer *xfer);			// 0x0039D533 (the unit line's snapshot data: both counts, +0x08, the two strings, +0x14)

	Int m_minUnits;			// +0x00 (provisional, added by addUnitInfo)
	Int m_maxUnits;			// +0x04 (provisional, added by addUnitInfo)
	Int m_unk0x08;			// +0x08 1 on construction
	AsciiString m_unk0x0C;		// +0x0C
	AsciiString m_unitThingName;	// +0x10 (provisional; addUnitInfo's match key)
	Int m_unk0x14;			// +0x14 -1 on construction; addUnitInfo's second key
};
typedef char TCreateUnitsInfoSize[(sizeof(TCreateUnitsInfo) == 0x18) ? 1 : -1];

enum { MAX_UNIT_TYPES = 7, MAX_GENERIC_SCRIPTS = 32 };

// TeamTemplateInfo: a team template's parsed properties. SIZE 0x1EC (TeamPrototype
// embeds one at +0x12C, and its +0x318 member follows).
//  - table 0x00C1AE70 (ctor 0x003A26E6 stores it; dtor 0x0039EAED restores
//    Snapshot's): slot 0 ??_G 0x003A287B, slot 1 loadPostProcess (folded empty
//    0x000B3FD0), slot 2 GetSnapshotName 0x003A2875, slot 3 xfer 0x0039D5C1.
//  - members from the ctor's stores and the dtor's releases: the 7 unit lines at
//    +4, a count at +0xAC, a zeroed Coord3D at +0xB0, strings at
//    +0xC0/+0xC4/+0xC8/+0xD0/+0xD4/+0xD8/+0xDC/+0x100/+0x104/+0x110 and the 32 at
//    +0x118, a float default (VA 0x00BC26F0) at +0xE0, seven Bools +0xE4..+0xEA,
//    +0xEC = 1, two 218-bit masks at +0x1B4/+0x1D0 (member ctor 0x0024C7B3).
//  - names: TeamPrototype's priority methods 0x0039D72D..0x0039D75E use
//    +0xF0/+0xF4/+0xF8; AITNGuardOuterState::update 0x00546577 tests +0xEA through
//    the prototype; Team::updateGenericScripts 0x0039F8F7 reads the 32 strings.
//    Names marked provisional are role labels taken from the Zero Hour team
//    template where its members line up with this binary's stores (string,
//    Bool and Int runs in the same order); nothing in this binary names them yet.
class TeamTemplateInfo : public Snapshot
{
public:
	TeamTemplateInfo(Dict *dict);			// 0x003A26E6
	virtual ~TeamTemplateInfo();			// COMDAT 0x0039EAED

	void loadFromDict(Dict *dict);			// 0x0039FEBB
	Int countMinimumUnits() const;			// 0x0039D5A9 (the sum of the unit lines' minimum counts)

protected:
	virtual void loadPostProcess() {}		// slot 1 (folded 0x000B3FD0)
	virtual const char *GetSnapshotName() const;	// slot 2, 0x003A2875
	virtual void xfer(Xfer *xfer);			// slot 3, 0x0039D5C1

public:
	TCreateUnitsInfo m_unitsInfo[MAX_UNIT_TYPES];	// +0x04
	Int m_numUnitsInfo;				// +0xAC addUnitInfo appends while below 7
	Coord3D m_homeLocation;				// +0xB0 (provisional)
	Bool m_hasHomeLocation;				// +0xBC (provisional)
	AsciiString m_scriptOnCreate;			// +0xC0 (provisional)
	AsciiString m_unk0xC4;				// +0xC4
	AsciiString m_scriptOnIdle;			// +0xC8 (provisional)
	Int m_initialIdleFrames;			// +0xCC (provisional; the dtor skips it)
	AsciiString m_scriptOnEnemySighted;		// +0xD0 (provisional)
	AsciiString m_scriptOnAllClear;			// +0xD4 (provisional)
	AsciiString m_scriptOnUnitDestroyed;		// +0xD8 (provisional)
	AsciiString m_scriptOnDestroyed;		// +0xDC (provisional)
	Real m_destroyedThreshold;			// +0xE0 (provisional)
	Bool m_isAIRecruitable;				// +0xE4 (provisional)
	Bool m_isBaseDefense;				// +0xE5 (provisional)
	Bool m_isPerimeterDefense;			// +0xE6 (provisional)
	Bool m_automaticallyReinforce;			// +0xE7 (provisional)
	Bool m_transportsReturn;			// +0xE8 (provisional)
	Bool m_avoidThreats;				// +0xE9 (provisional)
	Bool m_attackCommonTarget;			// +0xEA (provisional; AITNGuardOuterState::update tests it before taking the team target)
	Int m_maxInstances;				// +0xEC 1 on construction (provisional)
	Int m_productionPriority;			// +0xF0 raised/lowered by the priority methods
	Int m_productionPrioritySuccessIncrease;	// +0xF4 increaseAIPriorityForSuccess's step
	Int m_productionPriorityFailureDecrease;	// +0xF8 decreaseAIPriorityForFailure's step
	Int m_initialTeamAttitude;			// +0xFC (provisional)
	AsciiString m_transportUnitType;		// +0x100 (provisional)
	AsciiString m_startReinforceWaypoint;		// +0x104 (provisional)
	Bool m_teamStartsFull;				// +0x108 (provisional)
	Bool m_transportsExit;				// +0x109 (provisional)
	Int m_veterancy;				// +0x10C (provisional)
	AsciiString m_productionCondition;		// +0x110 (provisional)
	Bool m_executeActions;				// +0x114 (provisional)
	AsciiString m_teamGenericScripts[MAX_GENERIC_SCRIPTS];	// +0x118
	Int m_unk0x198;					// +0x198 -1 on construction
	Int m_unk0x19C;					// +0x19C
	Int m_unk0x1A0;					// +0x1A0 -1 on construction
	Int m_unk0x1A4;					// +0x1A4 -1 on construction
	Int m_unk0x1A8;					// +0x1A8 (prototype +0x2D4) 1 on construction; addUnitInfo adds the line's +0 to it; AITactic::initializeTeamTemplate 0x004ECE61 stores a random 3..5
	Int m_unk0x1AC;					// +0x1AC -1 on construction
	Int m_unk0x1B0;					// +0x1B0
	KindOfMaskType m_unk0x1B4;			// +0x1B4
	KindOfMaskType m_unk0x1D0;			// +0x1D0
};
// SIZE 0x1EC
typedef char TeamTemplateInfoSize[(sizeof(TeamTemplateInfo) == 0x1EC) ? 1 : -1];
typedef char TeamTemplateInfoAttackCommonTarget[(offsetof(TeamTemplateInfo, m_attackCommonTarget) == 0xEA) ? 1 : -1];

// TeamPrototype: one team definition (a map's team list entry). SIZE 0x338.
//  - Size: TeamFactory allocates 0x338 before each ctor call (0x003A3D16,
//    0x003A4093, 0x003A42BA).
//  - table 0x00C1AE94, stored by ctor 0x003A3299 and dtor 0x003A33CD (which
//    restores Snapshot's 0x00BBB554): slot 0 ??_G 0x003A38B1, slot 1
//    loadPostProcess (folded empty 0x000B3FD0), slot 2 GetSnapshotName
//    0x003A33A1 ("TeamPrototype"), slot 3 xfer 0x003A3E74.
//  - members from the ctor's stores in order: factory +4, player +8, id +0xC,
//    the owner and team name strings +0x10/+0x14, flags +0x18 (1 when the
//    singleton argument is set), +0x1C, a string +0x20, script +0x24, +0x28,
//    32 script pointers +0x2C (memset 0x80 in the body; the dtor deletes each),
//    32 strings +0xAC (vector ctor), the template +0x12C, a string +0x318,
//    Bools +0x31C..+0x31E, the logic frame +0x320, +0x324, a zeroed Coord3D
//    +0x328 and the instance list head +0x334.
//  - xfer 0x003A3E74 saves the player index, +0x318, +0x1C, the template, the
//    instance count, +0x324, +0x328..+0x330, and from version 2 +0x31C..+0x320.
class TeamPrototype : public Snapshot
{
public:
	TeamPrototype(TeamFactory *factory, const AsciiString &ownerName, const AsciiString &name,
		Player *owningPlayer, Bool isSingleton, Dict *dict, Int id);	// 0x003A3299
	friend class TeamFactory;			// TeamFactory::clear 0x003A2F4C deletes the prototypes through the protected destructor
protected:
	virtual ~TeamPrototype();			// 0x003A33CD

	virtual void loadPostProcess() {}		// slot 1 (folded 0x000B3FD0)
	virtual const char *GetSnapshotName() const;	// slot 2, 0x003A33A1
	virtual void xfer(Xfer *xfer);			// slot 3, 0x003A3E74

public:
	Script *getGenericScript(Int scriptToRetrieve, AsciiString *nameOut);	// 0x0039EC1E
	void setRecruitPosition(const Coord3D *position);	// 0x0039D7A6 (sets +0x324 and copies the position to +0x328)
	void addUnitInfo(const TCreateUnitsInfo &info);	// 0x003A0D62 (finds the unit line by name and the int at +0x14, adds the counts and the minimum, or appends below MAX_UNIT_TYPES)
	void removeUnitInfo(const TCreateUnitsInfo &info);	// 0x003A0E08 (role name: the inverse of addUnitInfo on a hit; Zero Hour has none)
	void addToLists();				// 0x003A2BD4
	void setControllingPlayer(Player *newController);
	void rva003A2C0D(Player *owner, const AsciiString &name, Bool isSingleton, Dict *dict);	// 0x003A2C0D re-initialises the prototype for a new owner
	void increaseAIPriorityForSuccess();		// 0x0039D72D
	void decreaseAIPriorityForFailure();		// 0x0039D73A
	void rva0039D747(Int amount);			// 0x0039D747 raises +0x21C by the argument
	void rva0039D754(Int amount);			// 0x0039D754 lowers +0x21C by the argument
	Int countTeamInstances();			// 0x0039D954
	Int countObjects(KindOfMaskType setMask, KindOfMaskType clearMask);	// 0x0039ED11 (Player::countObjects 0x002AB138 sums it over its prototypes)
	void countObjectsByThingTemplate(Int numTemplates, const ThingTemplate *const *templates, Bool ignoreDead, Int *counts, Bool ignoreUnderConstruction) const;	// 0x0039EBD9
	void moveTeamTo(Coord3D destination);	// 0x0039F03A: every instance's moveTeamTo
	Int rva0039ED9C(Int (*visit)(Object *member, void *userData), void *userData);	// 0x0039ED9C, runs the callback over every instance's members
	Bool rva0039EDE1(Bool);				// 0x0039EDE1
	Bool hasAnyObjects(Bool);			// 0x0039EF86
	Bool hasAnyUnits() const;			// 0x0039EEB4
	Bool hasAnyBuildings(KindOfMaskType kindOf, Bool ignoreIncomplete);	// 0x0039EE21 (every instance's 0x0039DDC2)
	Bool hasAnyBuildings(ObjectFilter *filter, Bool ignoreIncomplete);	// 0x0039EE70
	Bool hasAnyBuildings(KindOfMaskType setMask, KindOfMaskType clearMask);	// 0x0039EEEE
	Bool hasAnyUnits(ObjectFilter *filter);	// 0x0039EF46
	Bool hasAnyBuildFacility() const;		// 0x0039EFC6
	void healAllObjects();				// 0x0039ED6C
	void damageTeamMembers(Real amount);		// 0x0039F000
	Int countBuildings();				// 0x0039ECD9
	void updateState();				// 0x003A34AC
	Bool evaluateProductionCondition();		// 0x003A0E6E

	Bool isInList_TeamInstanceList(Team *team) const;	// 0x0039D4A2
	void prependTo_TeamInstanceList(Team *team);		// 0x0039D4B5
	void removeFrom_TeamInstanceList(Team *team);		// 0x0039D4D8
	void removeAll_TeamInstanceList(void (*deleteFunc)(Team *));	// 0x0039D4FB

	UnsignedInt getID() const { return m_id; }
	const AsciiString &getName() const { return m_name; }
	Player *getControllingPlayer() const { return m_owningPlayer; }
	Bool getIsSingleton() const { return (m_flags & 1) != 0; }
	const TeamTemplateInfo *getTemplateInfo() const { return &m_teamTemplate; }
	Team *getFirstItemIn_TeamInstanceList() const { return m_dlinkhead_TeamInstanceList; }
	DLINK_ITERATOR<Team> iterate_TeamInstanceList() const
	{
		return DLINK_ITERATOR<Team>(m_dlinkhead_TeamInstanceList, &Team::dlink_next_TeamInstanceList);
	}
	Team *findTeamByID(UnsignedInt id);		// 0x0039D92A
	void teamAboutToBeDeleted(Team *team);		// 0x003A2CA5
	void rva003A0D0A();				// 0x003A0D0A
	void rva003A0CD1();				// 0x003A0CD1 (the dtor runs it after unlinking the instances)

	TeamFactory *m_factory;				// +0x04
	Player *m_owningPlayer;				// +0x08 (xfer saves its index)
	UnsignedInt m_id;				// +0x0C
	AsciiString m_ownerName;			// +0x10 (provisional; 0x003A2C0D copies the player's name here)
	AsciiString m_name;				// +0x14
	Int m_flags;					// +0x18 bit 0: singleton
	Bool m_productionConditionAlwaysFalse;		// +0x1C (provisional)
	AsciiString m_unk0x20;				// +0x20
	Script *m_productionConditionScript;		// +0x24 (provisional; the dtor deletes it)
	Bool m_retrievedGenericScripts;			// +0x28 (provisional)
	Script *m_genericScriptsToRun[MAX_GENERIC_SCRIPTS];	// +0x2C the dtor deletes each
	AsciiString m_genericScriptNames[MAX_GENERIC_SCRIPTS];	// +0xAC (provisional)
	TeamTemplateInfo m_teamTemplate;		// +0x12C
	AsciiString m_attackPriorityName;		// +0x318 (provisional)
	Bool m_unk0x31C;				// +0x31C
	Bool m_unk0x31D;				// +0x31D
	Bool m_unk0x31E;				// +0x31E
	UnsignedInt m_unk0x320;				// +0x320 the logic frame at construction
	Bool m_unk0x324;				// +0x324
	Coord3D m_unk0x328;				// +0x328 zeroed at construction
	Team *m_dlinkhead_TeamInstanceList;		// +0x334
};
// SIZE 0x338
typedef char TeamPrototypeSize[(sizeof(TeamPrototype) == 0x338) ? 1 : -1];
typedef char TeamPrototypeTemplateOffset[(offsetof(TeamPrototype, m_teamTemplate) == 0x12C) ? 1 : -1];
typedef char TeamPrototypeHeadOffset[(offsetof(TeamPrototype, m_dlinkhead_TeamInstanceList) == 0x334) ? 1 : -1];

#include "Common/TeamFactory.h"

extern TeamFactory *TheTeamFactory;	// VA 0x00E028BC

#endif
