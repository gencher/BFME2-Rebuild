// OpenContain.h: the base of every contain module. SIZE 0xFC.
//
// Target facts:
// - Bases: the constructor 0x004649F8 calls UpdateModule's 0x00253390, then
//   stores six interface tables at +0x20..+0x34 twice (the interfaces' own
//   tables, then OpenContain's): +0x20 ContainModuleInterface (0x00C430E8 ->
//   0x00C433B0, 94 slots), +0x24 CollideModuleInterface (0x00C40818, 6 slots,
//   the CollideModule table), +0x28 DieModuleInterface (one slot, onDie
//   0x004641B1), +0x2C DamageModuleInterface (0x00BE2B78 -> the folded
//   0x00C58790, as in UpgradeModule), +0x30 ExitInterface (0x00C430B4 ->
//   0x00C43380, 12 slots) and +0x34 CreateModuleInterface (0x00C4EF80, 4
//   __purecall slots -> 0x00C48B28). getDamage 0x004647C0 returns +0x2C and
//   getCreate 0x0046644A returns +0x34 from the +0x0C behavior interface;
//   the behavior table 0x00C43528 returns +0x24/+0x20/+0x28 for its
//   getCollide/getContain/getDie slots (folded bodies).
// - Size: the factory 0x0024B6E9 allocates 0xFC.
// - Members: the constructor's stores and member constructors, typed by xfer
//   0x00465EF1: STLport map constructors at +0x3C/+0x48/+0x5C/+0xF0 (one
//   folded body), list constructors at +0x54/+0x74, zeroed dwords +0x38,
//   +0x58, +0x68, +0x70, +0x78..+0x80, +0xE4..+0xEC, +0x6C = 1, the 591-bit
//   model-condition set at +0x84 (memset 0x4C, xferred through
//   BitFlags<591>::xfer), a zeroed Coord3D at +0xD0, and bools +0xDC..+0xE1
//   (+0xDD and +0xE1 true, +0xDE copied from the module data's +0x84).
//   xfer reads the +0x74 list as ObjectIDs (XferObjectID) and the maps
//   through map<int,int>::operator[].
// - Primary-table slots 13..26 are OpenContain's own virtuals (table
//   0x00C435E8): 13 removeFromContainList 0x00462F75, 17
//   monitorConditionChanges 0x00462C72, 18 redeployOccupants 0x00464286,
//   19 0x00464EE7 (Object*, returns bool), 21 doLoadSound 0x0046279C,
//   22 doUnloadSound 0x0046282B, 24 0x00462E3A (Object*, int, int),
//   26 0x00465972 (the redeploy worker; the map spells it non-virtual
//   privateRedeployOccupants, but it sits in the table).
// Donor facts: Zero Hour's OpenContain names the base list, the contain list
// (+0x54, with its cached size at +0x58), the ID list kept for xfer (+0x74),
// m_conditionState and m_rallyPoint; members without a donor match keep
// offset names.
#pragma once
#ifndef BFME2_OPENCONTAIN_H
#define BFME2_OPENCONTAIN_H

#include <stddef.h>
#include <list>
#include <map>
#include "Common/GameType.h"
#include "Common/Coord3D.h"
#include "Common/BitFlags.h"
#include "GameLogic/Module/UpdateModule.h"
#include "GameLogic/Module/ContainModule.h"
#include "GameLogic/Module/CollideModule.h"
#include "GameLogic/Module/DieModuleInterface.h"
#include "GameLogic/Module/DamageModule.h"
#include "GameLogic/Module/ExitInterface.h"
#include "GameLogic/Module/CreateModule.h"
#include "GameLogic/Module/OpenContainModuleData.h"

class DamageInfo;
class Object;
class Player;
class Team;
class Thing;
class ModuleData;
class Xfer;
class Coord3D;

typedef std::list<Object *> ContainedItemsList;
typedef std::list<ObjectID> XferContainIDList;
typedef std::map<ObjectID, ObjectEnterExitType> ObjectEnterExitMap;

class OpenContain : public UpdateModule,
	public ContainModuleInterface,		// +0x20
	public CollideModuleInterface,		// +0x24
	public DieModuleInterface,			// +0x28
	public DamageModuleInterface,		// +0x2C
	public ExitInterface,				// +0x30
	public CreateModuleInterface		// +0x34
{
public:
	OpenContain(Thing *thing, const ModuleData *moduleData);			// 0x004649F8
	static Module *friend_newModuleInstance(Thing *thing, const ModuleData *moduleData);
	static ModuleData *friend_newModuleData(INI *ini);
	const OpenContainModuleData *getOpenContainModuleData() const { return (const OpenContainModuleData *)getModuleData(); }
	void privateRemoveFromContain(Object *obj, Bool exposeStealthUnits);	// 0x00462FB3
	ContainmentListView getContainList() const
	{
		ContainmentListView view;
		view.m_owner = static_cast<const ContainModuleInterface *>(this);
		view.m_items = &m_containList;
		return view;
	}
	const char *name() const;											// 0x004647BA
	void pruneDeadWanters();											// 0x00463A81 (Zero Hour's name; the map spells it Rva00463A81)
	void updateTransientPassenger();									// 0x0046391B (map: ?rva0046391B@Rva00463A81@@QAEXXZ): update 0x004640BE calls it every frame; counts m_unk0xEC down and, at zero, takes the rider whose ID is m_unk0xE4 out of the container, unhides it and destroys it if it left the list
	void recountStealthUnitsContained();								// 0x00462F34 (counts the riders with KINDOF_STEALTH_GARRISON again)
	Int shouldKillPassengersOnContainerDeath() const;				// 0x00462785 (the map files it under the placeholder class Rva00462785; the primary this is the caller's interface this minus 0x20): m_unk0xE1 and the module data's KillPassengersOnDeath; GarrisonContain::onRemoving 0x00478D0A tests it
	virtual ~OpenContain();												// 0x00464B7A (deleting)

	// Module
	virtual NameKeyType getModuleNameKey() const;						// 0x00464775
	virtual void onDelete();											// 0x00464120

	// BehaviorModuleInterface
	virtual CollideModuleInterface *getCollide() { return this; }
	virtual ContainModuleInterface *getContain() { return this; }
	virtual CreateModuleInterface *getCreate() { return this; }			// 0x0046644A
	virtual DamageModuleInterface *getDamage() { return this; }			// 0x004647C0
	virtual DieModuleInterface *getDie() { return this; }

	// UpdateModuleInterface
	virtual UpdateSleepTime update();									// 0x004640BE

	// CollideModuleInterface
	virtual void onCollide(Object *other, const Coord3D *loc, const Coord3D *normal);
	virtual Bool wouldLikeToCollideWith(const Object *other) const { return false; }
	virtual Bool isHijackedVehicleCrateCollide() const { return false; }
	virtual Bool isCarBombCrateCollide() const { return false; }
	virtual Bool isRailroad() const { return false; }
	virtual Bool isSalvageCrateCollide() const { return false; }

	// DieModuleInterface
	virtual void onDie(const DamageInfo *damageInfo);					// 0x004641B1

	// DamageModuleInterface
	virtual void onDamage(DamageInfo *damageInfo) {}
	virtual void onHealing(DamageInfo *damageInfo) {}
	virtual void onBodyDamageStateChange(const DamageInfo *damageInfo,
		BodyDamageType oldState, BodyDamageType newState) {}

	// CreateModuleInterface
	virtual void onCreate() {}
	virtual void onBuildComplete() {}
	virtual void vslot02() {}
	virtual Bool shouldDoOnBuildComplete() const { return false; }

	// ExitInterface
	virtual Bool isExitBusy() const { return false; }
	virtual ExitDoorType reserveDoorForExit(const ThingTemplate *objType, Object *specificObject);
	virtual void exitObjectViaDoor(Object *newObj, ExitDoorType exitDoor);	// 0x0046525F
	virtual void exitObjectByBudding(Object *newObj, Object *budHost) {}
	virtual void unreserveDoorForExit(ExitDoorType exitDoor) {}
	virtual void exitObjectInAHurry(Object *newObj);					// 0x004656DF
	virtual Bool useSpawnRallyPoint() const { return false; }
	virtual void setRallyPoint(const Coord3D *pos);						// 0x0046255E
	virtual const Coord3D *getRallyPoint() const;						// 0x00462579
	virtual Bool getNaturalRallyPoint(Coord3D &rallyPoint, Bool offset) const;	// 0x0046363F
	virtual Bool getExitPosition(Coord3D &exitPosition, Real &exitAngle) const;	// 0x004624C1
	virtual void notifyProductionBurstFinished() {}

	// ContainModuleInterface
	virtual void containSlot00(Bool);											// 0x0046261B
	virtual OpenContain *asOpenContain() { return this; }				// 0x004647CD
	virtual TunnelContain *asTunnelContain() { return 0; }
	virtual void containReactToTransformChange();						// 0x004624E3
	virtual bool isGarrisonable() const { return false; }
	virtual Bool containSlot05() { return false; }			// 5: the shared `xor al,al; ret` 0x0047A699
	virtual Bool containSlot06() { return false; }			// 6: the shared `xor al,al; ret` 0x0047A699
	virtual Bool isImmuneToClearBuildingAttacks() const { return true; }	// 7: the shared `mov al,1` 0x0050B5C6
	virtual Bool containSlot08() { return false; }			// 8: the shared `xor al,al; ret` 0x0047A699
	virtual Bool containSlot09() { return false; }			// 9: the shared `xor al,al; ret` 0x0047A699
	virtual Object *containSlot10() { return 0; }			// 10: the shared `xor eax,eax; ret` 0x000D43D0
	virtual Bool containSlot11() { return false; }			// 11: the shared `xor al,al; ret` 0x0047A699
	virtual Bool containSlot12() { return false; }			// 12: the shared `xor al,al; ret` 0x0047A699
	virtual Bool containSlot13() { return false; }			// 13: the shared `xor al,al; ret` 0x0047A699
	virtual Bool containSlot14() const;
	virtual void containSlot15() {}			// 15: the shared `ret` 0x000B3FD0
	virtual void containSlot16();
	virtual Bool onObjectWantsToEnterOrExit(Object *obj, ObjectEnterExitType wants);	// 0x00464FC7
	virtual Bool hasObjectsWantingToEnterOrExit() const;
	virtual const Player *getApparentControllingPlayer(const Player *observingPlayer) const { return 0; }	// 19: the shared `xor eax,eax; ret 4` 0x005DAA6C
	virtual void recalcApparentControllingPlayer() {}					// 20: the shared `ret` 0x000B3FD0
	virtual void containSlot21(Team *team) {}			// 21: the shared `ret 4` 0x0047A69C
	virtual void onContaining(Object *obj, Bool wasSelected);			// 0x00463097
	virtual void onRemoving(Object *obj);								// 0x00464D02
	virtual void containSlot24();
	virtual void onSelling();
	virtual void containSlot26();
	virtual void containSlot27(Int, Int) {}			// 27: the shared `ret 8` 0x0050B238
	virtual Int getContainMax() const;
	virtual ExitInterface *getContainExitInterface();
	virtual HordeContainInterface *getHordeContainInterface() { return 0; }				// 31: the shared `xor eax,eax; ret` 0x000D43D0
	virtual const HordeContainInterface *getHordeContainInterface() const { return 0; }	// 30: the same body
	virtual void orderAllPassengersToExit(CommandSourceType cmdSource);	// 32: 0x00465011
	virtual void containSlot33(CommandSourceType cmdSource);			// 33: 0x00465078
	virtual void containSlot34(Int) {}			// 34: the shared `ret 4` 0x0047A69C
	virtual void containSlot35(CommandSourceType cmdSource) {}		// 35: the shared `ret 4` 0x0047A69C
	virtual void containSlot36();
	virtual void containSlot37() {}			// 37: the shared `ret` 0x000B3FD0
	virtual Bool isValidContainerFor(Object *obj, Bool checkCapacity, Bool flag);	// 0x00462977
	virtual void addToContain(Object *obj);								// 0x00464830
	virtual void addToContainList(Object *obj);							// 0x004638F1
	virtual void removeFromContain(Object *obj, Bool exposeStealthUnits);	// 0x00463509
	virtual void removeAllContained(Bool exposeStealthUnits);			// 0x004635C0
	virtual void containSlot43(Object *first, Int count, Object *second);	// 0x0046274C
	virtual ObjectStatusMaskType getObjectStatusOfContained(const Object *rider) const;		// 44, 0x00462CE1
	virtual Bool isPassengerAllowedToFire() const;						// 0x00462B4A
	virtual Bool containSlot46() { return false; }			// 46: the shared `xor al,al; ret` 0x0047A699
	virtual void containSlot47();
	virtual Bool containSlot48(const Object *rider, const Object *victim) const;	// 48, 0x00462BFF
	virtual void containSlot49(Int) {}			// 49: the shared `ret 4` 0x0047A69C
	virtual Bool containSlot50() { return false; }			// 50: the shared `xor al,al; ret` 0x0047A699
	virtual Int containSlot51() const { return 0; }
	virtual Bool containSlot52() { return true; }			// 52: the shared `mov al,1; ret` 0x0050B5C6
	virtual Real containSlot53();	// 53, 0x004647D1
	virtual Bool containSlot54() { return false; }			// 54: the shared `xor al,al; ret` 0x0047A699
	virtual Bool containSlot55(Int a, Int b);	// 55, 0x004628AB
	virtual void *containSlot56() { return 0; }			// 56: the shared `xor eax,eax; ret` 0x000D43D0
	virtual Bool containSlot57(Int) { return false; }			// 57: the shared `xor al,al; ret 4` 0x005CB9FF
	virtual Bool containSlot58(Int object);	// 58, 0x004631FA
	virtual void containSlot59(Object *object) {}			// 59: the shared `ret 4` 0x0047A69C
	virtual void containSlot60() {}			// 60: the shared `ret` 0x000B3FD0
	virtual Object *getClosestRider(const Coord3D *pos);				// 0x004632E0
	virtual void containSlot62() {}			// 62: the shared `ret` 0x000B3FD0
	virtual void containSlot63(Bool enabled);
	virtual void containSlot64();
	virtual Bool containSlot65() const;
	virtual void containSlot66();
	virtual void containSlot67();
	virtual void iterateContained(ContainIterateFunc func, void *userData, UnsignedInt flags);	// 0x004635EC
	virtual UnsignedInt getCount(bool);											// 0x00462DE0
	virtual ContainmentListView getList();								// 0x004647D8
	virtual ContainmentListView containSlot71();
	virtual const Object *friend_getRider() const { return 0; }	// 72: the shared `xor eax,eax; ret` 0x000D43D0
	virtual int getStealthUnitsContained() { return m_stealthUnitsContained; }	// 73: `mov eax,[ecx+0x48]; ret` 0x005C4AFD, folded with other classes' getters
	virtual void *containSlot74() { return 0; }			// 74: the shared `xor eax,eax; ret` 0x000D43D0
	virtual void *containSlot75() { return 0; }			// 75: the shared `xor eax,eax; ret` 0x000D43D0
	// Only a garrison implementation can supply a position.
	virtual Bool calcBestGarrisonPosition(Coord3D *sourcePos, const Coord3D *targetPos) { return false; }
	virtual Bool attemptBestFirePointPosition(Object *source, Weapon *weapon, Object *victim) { return false; }			// 78: the shared `xor al,al; ret 0xC` 0x0041BA5C
	virtual Bool attemptBestFirePointPosition(Object *source, Weapon *weapon, const Coord3D *targetPos) { return false; }	// 77: the same body
	virtual void containSlot79();
	virtual Bool containSlot80() { return false; }			// 80: the shared `xor al,al; ret` 0x0047A699
	virtual UnsignedInt getPlayerWhoEntered() const;					// 81: `mov eax,[ecx+0x58]; ret` 0x003F3FCE (returns m_playerEnteredMask), folded with Pathfinder's getter; stays out of line until the map accepts this name on the fold
	virtual void containSlot82();
	virtual void enableLoadSounds(Bool enable);							// 0x004647FD
	virtual void containSlot84();
	virtual Object *containSlot85() const;	// 85, 0x00464825
	virtual const Coord3D *containSlot86();	// 86, 0x004626CA
	virtual const Coord3D *containSlot87();	// 87, 0x004626AF
	virtual const Coord3D *containSlot88() const;
	virtual Int containSlot89(Int);	// 89, 0x0046480A
	virtual Bool setWeaponLock(WeaponSlotType weaponSlot, WeaponLockType lockType);	// 90, 0x004625BB
	virtual void releaseWeaponLock(WeaponLockType lockType);			// 0x004625F0
	virtual Bool containSlot92(Object *obj, Int);	// 92, 0x00462955
	virtual void containSlot93();

protected:
	// Snapshot
	virtual void loadPostProcess();										// 0x00463C6E
	virtual const char *GetSnapshotName() const;						// slot; retail's body is the map's `name` 0x004647BA
	virtual void xfer(Xfer *xfer);										// 0x00465EF1

public:
	// OpenContain's own primary slots 13..26 (placeholder names: vslotNN)
	virtual void removeFromContainList(Object *obj);					// 13, 0x00462F75
	virtual Bool vslot14(Int) { return true; }							// 14: the shared `mov al,1; ret 4` 0x005CB9FA
	virtual void vslot15(void *, Int) {}								// 15: the shared `ret 8` 0x0050B238
	virtual Bool vslot16(Int) { return false; }							// 16: the shared `xor al,al; ret 4` 0x005CB9FF
protected:
	virtual void monitorConditionChanges();								// 17, 0x00462C72
	virtual void redeployOccupants();									// 18, 0x00464286
public:
	virtual Bool vslot19(Object *obj);									// 19, 0x00464EE7
	virtual Bool vslot20(Object *) { return false; }							// 20: the shared `xor al,al; ret 4` 0x005CB9FF
protected:
	virtual void doLoadSound();											// 21, 0x0046279C
	virtual void doUnloadSound();										// 22, 0x0046282B
public:
	virtual void vslot23() {}											// 23: the shared `ret` 0x000B3FD0
	virtual void vslot24(Object *obj, Int, Int);						// 24, 0x00462E3A
	virtual void vslot25() {}											// 25: the shared `ret` 0x000B3FD0
	virtual void vslot26(const ContainedItemsList *list, std::map<ObjectID, Int> *map);	// 26, 0x00465972

public:
	Int m_unk0x38;										// 0x38 zeroed, not xferred
	std::map<ObjectID, AsciiString> m_map0x3C;			// 0x3C xfer writes each key with XferObjectID and each value with xferAsciiString
	std::map<ObjectID, Int> m_map0x48;					// 0x48 the destructor runs the map<ObjectID,int> tree destructor on it
	ContainedItemsList m_containList;					// 0x54
	UnsignedInt m_containListSize;						// 0x58
	ObjectEnterExitMap m_objectEnterExitInfo;			// 0x5C object id -> enter/exit wish (onObjectWantsToEnterOrExit 0x00464FC7, pruneDeadWanters 0x00463A81)
	UnsignedInt m_stealthUnitsContained;				// 0x68 addToContainList 0x004638F1 counts riders with KINDOF_STEALTH_GARRISON; privateRemoveFromContain 0x00462FB3 decrements it
	Int m_unk0x6C;										// 0x6C = 1
	UnsignedInt m_doorCloseCountdown;					// 0x70 update 0x004640BE counts it down and clears the door-opening state at zero
	XferContainIDList m_xferContainIDList;				// 0x74
	UnsignedInt m_playerEnteredMask;					// 0x78 getPlayerWhoEntered returns it (Zero Hour's name); the constructor clears it
	UnsignedInt m_lastUnloadSoundFrame;					// 0x7C doUnloadSound 0x0046282B compares and stores the game frame
	UnsignedInt m_lastLoadSoundFrame;						// 0x80 doLoadSound 0x0046279C
	BitFlags<591> m_conditionState;						// 0x84
	Coord3D m_rallyPoint;								// 0xD0
	Bool m_rallyPointExists;							// 0xDC set true by setRallyPoint 0x0046255E, tested by getRallyPoint 0x00462579
	Bool m_loadSoundsEnabled;							// 0xDD = true; doLoadSound tests it, enableLoadSounds 0x004647FD stores it
	Bool m_enabled;										// 0xDE the module data's Enabled (+0x84) at creation; slot 63 sets it, slot 64 resets it
	Bool m_unk0xDF;										// 0xDF
	Bool m_unk0xE0;										// 0xE0
	Bool m_unk0xE1;										// 0xE1 = true
	ObjectID m_unk0xE4;									// 0xE4 slot 43's first object id
	ObjectID m_unk0xE8;									// 0xE8 slot 43's second object id
	Int m_unk0xEC;										// 0xEC slot 43's count, at least 1
	std::map<ObjectID, UnsignedInt> m_map0xF0;			// 0xF0 the modifier to give on exit: object id to the game frame it was added
};
// SIZE 0xFC

// The block setWeaponLock 0x004625BB hands to its visitor 0x0046258C through iterateContained:
// [ebp-0xC] slot, [ebp-8] lockType, the byte [ebp-4] cleared before and tested after the walk.
struct SetWeaponLockData
{
	WeaponSlotType weaponSlot;
	WeaponLockType lockType;
	Bool failed;
};

// The block releaseWeaponLock 0x004625F0 hands to its visitor 0x004625AA through iterateContained.
struct ReleaseWeaponLockData
{
	Int unused;
	WeaponLockType lockType;
	Bool done;
};

typedef char OpenContain_off_m_map0x3C[(offsetof(OpenContain, m_map0x3C) == 0x3C) ? 1 : -1];
typedef char OpenContain_off_m_containList[(offsetof(OpenContain, m_containList) == 0x54) ? 1 : -1];
typedef char OpenContain_off_m_xferContainIDList[(offsetof(OpenContain, m_xferContainIDList) == 0x74) ? 1 : -1];
typedef char OpenContain_off_m_conditionState[(offsetof(OpenContain, m_conditionState) == 0x84) ? 1 : -1];
typedef char OpenContain_off_m_rallyPoint[(offsetof(OpenContain, m_rallyPoint) == 0xD0) ? 1 : -1];
typedef char OpenContain_off_m_unk0xE4[(offsetof(OpenContain, m_unk0xE4) == 0xE4) ? 1 : -1];
typedef char OpenContain_off_m_map0xF0[(offsetof(OpenContain, m_map0xF0) == 0xF0) ? 1 : -1];
typedef char OpenContain_size[(sizeof(OpenContain) == 0xFC) ? 1 : -1];

#endif
