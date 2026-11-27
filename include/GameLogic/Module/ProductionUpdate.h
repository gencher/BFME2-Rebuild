// GetSnapshotName 0x0049E13C, requestUniqueUnitID 0x0049E142,
// nextProduction 0x0049E14C, getModuleNameKey 0x0049E15E and isUpgradeInQueue 0x0049CDF9 MATCH against it.
// ProductionUpdate.h: the ProductionUpdate module, its interface and its queue entries.
//
// Target facts:
// - ProductionUpdate (constructor 0x0049E03A, factory size 0x140): UpdateModule, then
//   ProductionUpdateInterface at +0x20 (own table 0x00C513F0, 31 slots -> 0x00C51478) and a
//   DieModuleInterface at +0x24 (pure table 0x00C1C780 -> 0x00C51470, whose slot 0x0049CE63
//   steps back to +0x20 and calls its slot 16). Members from the stores: queue head/tail
//   +0x28/+0x2C, the next ProductionID +0x30 = 1 (requestUniqueUnitID 0x0049E142 returns it
//   post-increment), +0x34, +0x38, four 16-byte door records at +0x3C (three dwords and a
//   bool each, zeroed in a loop), the 591-bit condition sets +0x7C and +0xC8 (BitFlags<591>
//   xfer), bools +0x114/+0x11C/+0x11D, +0x118, +0x120, a list +0x124, +0x128, +0x12C = 1, a
//   vector +0x130 and a bool +0x13C.
// - ProductionEntry (constructor 0x0049D162, allocated 0x54 in queueCreateUnit 0x0049DA34):
//   its own table 0x00C5146C (deleting destructor 0x0049D84B first), dwords +0x04..+0x0C,
//   +0x10 = 1, Reals +0x14/+0x18, dwords +0x1C..+0x30, a bool +0x34, dwords +0x38..+0x40, a
//   bool +0x44, and the queue links +0x48 (next; nextProduction 0x0049E14C returns it) and
//   +0x4C. The map names the class Rva0049D1B1.
// - getProductionUpdateInterfaceFromObject 0x0049CEFC walks Object +0x244's behaviour
//   modules and asks each through BehaviorModuleInterface slot 28.
// - ProductionUpdateInterface slot names come from the rowed overrides in 0x00C51478;
//   unrowed slots keep pslotNN and the signature their row's placeholder spells.
// Donor facts: Zero Hour's ProductionUpdate names DoorInfo, m_productionQueue,
// m_productionQueueTail, m_uniqueID, m_doors, m_clearFlags and m_setFlags, and its
// ProductionEntry's m_next/m_prev; the BFME 2 offsets follow the constructors.
#pragma once
#ifndef BFME2_PRODUCTION_UPDATE_H
#define BFME2_PRODUCTION_UPDATE_H

#include <stddef.h>
#include "Common/GameType.h"
#include "Common/BitFlags.h"
#include "Common/AsciiString.h"
#include "GameLogic/Module/UpdateModule.h"
#include "GameLogic/Module/DieModuleInterface.h"
#include "GameLogic/Module/ProductionUpdateModuleData.h"
#include "Common/Vector.h"

class Thing;
class ThingTemplate;
class UpgradeTemplate;
class ModuleData;
class Object;
class Xfer;
// CanMakeType: what a factory or the build assistant answers when asked whether something can be
// made. Values from retail's returns: ProductionUpdate::canQueueUpgrade 0x0049CFA9 returns 4 when
// the queue count (+0x34) reaches MaxQueueEntries (data +0x28), 3 when the object has
// OBJECT_STATUS_TEMPORARILY_DEFECTED, else 0; BuildAssistant::canMakeUnit 0x00391B08 returns 1
// (no builder or no template), 2 (cost over the player's money), 3, 6 (the template's count at the
// player's maximum) and 7 (the Command points check 0x002A7557 fails). Names 0-4 and 6 are Zero
// Hour's by value; 5 has no BFME 2 return seen; 7 is a role name (PROVISIONAL).
enum CanMakeType
{
	CANMAKE_OK = 0,
	CANMAKE_NO_PREREQ = 1,
	CANMAKE_NO_MONEY = 2,
	CANMAKE_FACTORY_IS_DISABLED = 3,
	CANMAKE_QUEUE_FULL = 4,
	CANMAKE_PARKING_PLACES_FULL = 5,
	CANMAKE_MAXED_OUT_FOR_PLAYER = 6,
	CANMAKE_NO_COMMAND_POINTS = 7
};
enum ProductionID;
enum ExitDoorType;
// The set of production ids slot 20 (0x0049D0DE) counts against: one bit per id, tested with the entry's +0x30 value
// (the id requestUniqueUnitID hands out; the file's id lookup compares the same field).
// Only the word array the test reads is known (its length is not). The struct's tag is the map's placeholder
// name: slot 20's own mangled name carries it, so the tag stays until the map renames that row. Code says ProductionIDSet.
struct Rva0049D0DEMask
{
	UnsignedInt m_words[1];
};
typedef Rva0049D0DEMask ProductionIDSet;

class ProductionEntry
{
public:
	Bool getProductionObject() const;											// True for entry types 1 and 3 (units)
	ProductionEntry();													// 0x0049D162
	virtual ~ProductionEntry();											// deleting 0x0049D84B

	Int m_unk0x04;														// 0x04
	const ThingTemplate *m_unk0x08;										// 0x08
	const UpgradeTemplate *m_unk0x0C;									// 0x0C
	Int m_unk0x10;														// 0x10 = 1
	Real m_unk0x14;														// 0x14
	Real m_unk0x18;														// 0x18
	Int m_unk0x1C;														// 0x1C
	Int m_unk0x20;														// 0x20
	Int m_unk0x24;														// 0x24
	Int m_unk0x28;														// 0x28
	ExitDoorType m_exitDoor;											// 0x2C the door a unit entry reserved (EXIT_DOOR_NONE: none)
	Int m_unk0x30;														// 0x30
	Bool m_unk0x34;														// 0x34
	Int m_unk0x38;														// 0x38
	Int m_unk0x3C;														// 0x3C
	Int m_unk0x40;														// 0x40
	Bool m_unk0x44;														// 0x44
	ProductionEntry *m_next;											// 0x48
	ProductionEntry *m_prev;											// 0x4C
	Int m_unk0x50;														// 0x50 not set by the constructor
};
// SIZE 0x54
typedef char ProductionEntry_off_m_next[(offsetof(ProductionEntry, m_next) == 0x48) ? 1 : -1];
typedef char ProductionEntry_size[(sizeof(ProductionEntry) == 0x54) ? 1 : -1];

class ProductionUpdateInterface
{
public:
	virtual CanMakeType rva0049CFCD() const = 0;																// 0 0x0049CFCD
	virtual CanMakeType canQueueUpgrade(const UpgradeTemplate *upgrade) const = 0;							// 1
	virtual ProductionID requestUniqueUnitID() = 0;															// 2
	virtual Bool queueUpgrade(const UpgradeTemplate *upgrade) = 0;											// 3
	virtual void cancelUpgrade(const UpgradeTemplate *upgrade) = 0;											// 4
	virtual void rva0049CC9D(const ThingTemplate *, Bool) = 0;													// 5 0x0049CC9D
	virtual Bool isUpgradeInQueue(const UpgradeTemplate *upgrade) const = 0;									// 6
	virtual UnsignedInt countUnitTypeInQueue(const ThingTemplate *unitType) const = 0;						// 7
	virtual Bool queueCreateUnit(const ThingTemplate *unitType, Int, ProductionID productionID, Int, Bool,
		const AsciiString &, const AsciiString &) = 0;														// 8
	virtual Bool rva0049CC36(ProductionID) const = 0;															// 9 0x0049CC36
	virtual void pslot10(Int) = 0;																			// 10 0x0049DC50
	virtual void rva0049CC61(const ThingTemplate *) = 0;														// 11 0x0049CC61
	virtual void pslot12(Int) = 0;																			// 12 0x0049CCF0
	virtual void rva0049F8A9(const ThingTemplate *, Bool) = 0;												// 13 0x0049F8A9
	virtual void rva0049CC15(ProductionID, Int) = 0;															// 14 0x0049CC15
	virtual void cancelAndRefundAllProduction() = 0;															// 15
	virtual void rva0049DEC8() = 0;																			// 16 0x0049DEC8
	virtual void pslot17() = 0;																				// 17 folded
	virtual UnsignedInt rva0049CEAE(const ThingTemplate *) const = 0;											// 18 0x0049CEAE
	virtual UnsignedInt rva0049CEE1(Int) const = 0;																// 19 0x0049CEE1
	virtual UnsignedInt rva0049D0DE(const ProductionIDSet *mask) const = 0;									// 20 0x0049D0DE: how many queued entries have their +0x30 bit set in the mask
	virtual const ProductionEntry *firstProduction() const = 0;										// 21 folded getter (queue head +0x28); isUpgradeInQueue 0x0049CDF9 starts its walk with it
	virtual const ProductionEntry *nextProduction(const ProductionEntry *p) const = 0;						// 22
	virtual void setHoldDoorOpen(ExitDoorType exitDoor, Bool holdIt) = 0;										// 23 0x0049D617 (the map spells it Rva0049D617::rva0049D617(int, unsigned char); Zero Hour's name)
	virtual void pslot24(UnsignedByte) = 0;																	// 24 0x003BA5D0 (folded byte setter)
	virtual UnsignedByte pslot25() const = 0;																// 25 0x002C8E83 (folded byte getter)
	virtual Real rva0049D1CA(const ThingTemplate *) = 0;														// 26
	virtual Real rva0049D244(const ThingTemplate *) = 0;														// 27
	virtual Real rva0049D2BE(UnsignedByte) = 0;																	// 28
	virtual Real rva0049D33D(UnsignedByte) = 0;																	// 29
	virtual Bool get() const = 0;																		// 30
};

// The constructor installs the death interface at +0x24.
class ProductionUpdate : public UpdateModule, public ProductionUpdateInterface, public DieModuleInterface
{
public:
	struct DoorInfo
	{
		UnsignedInt m_doorOpenedFrame;									// 0x0 setHoldDoorOpen 0x0049D617 stores the game frame
		UnsignedInt m_doorWaitOpenFrame;								// 0x4
		UnsignedInt m_doorClosedFrame;									// 0x8
		Bool m_holdOpen;												// 0xC
	};

	ProductionUpdate(Thing *thing, const ModuleData *moduleData);		// 0x0049E03A
	static Module *friend_newModuleInstance(Thing *thing, const ModuleData *moduleData);

	// Death interface slot 0 adjusts from +0x24 to the production interface at +0x20.
	virtual void onDie(const DamageInfo *damageInfo);

	// BehaviorModuleInterface slot 28 returns the production interface at +0x20.
	virtual ProductionUpdateInterface *getProductionUpdateInterface() { return this; }
	virtual NameKeyType getModuleNameKey() const;						// 0x0049E15E
	virtual Bool queueUpgrade(const UpgradeTemplate *upgrade);	// ProductionUpdateInterface slot 3, retail word 0x0049D867
	virtual Bool queueCreateUnit(const ThingTemplate *unitType, Int, ProductionID productionID, Int, Bool, const AsciiString &, const AsciiString &);	// ProductionUpdateInterface slot 8, retail word 0x0049DA34
	virtual void pslot12(Int);	// ProductionUpdateInterface slot 12, retail word 0x0049CCF0
	virtual void pslot17();	// ProductionUpdateInterface slot 17, retail word 0x0057E556
	virtual void pslot24(UnsignedByte);	// ProductionUpdateInterface slot 24, retail word 0x003BA5D0
	virtual UnsignedByte pslot25() const;	// ProductionUpdateInterface slot 25, retail word 0x002C8E83
	virtual Real rva0049D1CA(const ThingTemplate *unitType);	// ProductionUpdateInterface slot 26
	virtual Real rva0049D244(const ThingTemplate *unitType);	// ProductionUpdateInterface slot 27
	virtual Real rva0049D2BE(UnsignedByte revive);	// ProductionUpdateInterface slot 28
	virtual Real rva0049D33D(UnsignedByte revive);	// ProductionUpdateInterface slot 29
	virtual Bool get() const;	// ProductionUpdateInterface slot 30
	virtual UpdateSleepTime update();	// 0x0049E3B5

	// UpdateModuleInterface slot 1 reads the data mask (0x0049E1A3).
	virtual DisabledMaskType getDisabledTypesToProcess() const
	{
		return static_cast<const ProductionUpdateModuleData *>(getModuleData())->m_disabledTypesToProcess;
	}

	const ProductionUpdateModuleData *getProductionUpdateModuleData() const
	{
		return static_cast<const ProductionUpdateModuleData *>(getModuleData());
	}

	static ProductionUpdateInterface *getProductionUpdateInterfaceFromObject(Object *obj);	// 0x0049CEFC

	// ProductionUpdateInterface overrides with rows in ProductionUpdate.cpp
	virtual ProductionID requestUniqueUnitID();							// 0x0049E142
	virtual Bool isUpgradeInQueue(const UpgradeTemplate *upgrade) const;	// 0x0049CDF9
	virtual UnsignedInt countUnitTypeInQueue(const ThingTemplate *unitType) const;	// 0x0049CE28
	virtual CanMakeType rva0049CFCD() const;							// (slot 0)
	virtual CanMakeType canQueueUpgrade(const UpgradeTemplate *upgrade) const;	// 0x0049CFA9 (slot 1)
	virtual const ProductionEntry *nextProduction(const ProductionEntry *p) const;	// 0x0049E14C
	virtual const ProductionEntry *firstProduction() const;				// slot 21, folded
	void rva0049CD24(Object *obj);										// Forwards obj to the game logic with the object's +0x45C value when it is set
	virtual void cancelAndRefundAllProduction();						// 0x0049CE6E
	virtual void cancelUpgrade(const UpgradeTemplate *upgrade);			// 0x0049D9A2
	virtual void rva0049CC9D(const ThingTemplate *unitType, Bool allowMany);			// (slot 5)
	virtual Bool rva0049CC36(ProductionID id) const;					// (slot 9)
	virtual void rva0049CC61(const ThingTemplate *unitType);			// (slot 11)
	virtual void rva0049CC15(ProductionID id, Int value);				// (slot 14)
	virtual UnsignedInt rva0049CEAE(const ThingTemplate *unitType) const;	// (slot 18)
	virtual UnsignedInt rva0049CEE1(Int id) const;					// (slot 19)
	virtual void pslot10(Int);											// 0x0049DC50
	virtual void setHoldDoorOpen(ExitDoorType exitDoor, Bool holdIt);	// 0x0049D617 (slot 23)
	virtual UnsignedInt rva0049D0DE(const ProductionIDSet *mask) const;	// (slot 20)
	virtual void rva0049F8A9(const ThingTemplate *typeName, Bool cancelNow);	// (slot 13): records a type name for later, or cancels the type's unfinished units
	virtual void rva0049DEC8();											// (slot 16)
	void rva0049E368();													// re-runs slot 13 with cancelNow for every recorded type name, then forgets them
	void addToProductionQueue(ProductionEntry *production);				// appends to the queue and sets ACTIVELY_CONSTRUCTING
	void removeFromProductionQueue(ProductionEntry *production);		// frees its exit door, unlinks it and clears ACTIVELY_CONSTRUCTING once the queue is empty

protected:
	virtual const char *GetSnapshotName() const;						// 0x0049E13C
	virtual void xfer(Xfer *xfer);										// 0x0049F91A

public:
	ProductionEntry *m_productionQueue;									// 0x28
	ProductionEntry *m_productionQueueTail;								// 0x2C
	Int m_uniqueID;														// 0x30 = 1
	UnsignedInt m_productionCount;										// 0x34 xfer UnsignedInt; entries in the queue (canQueueUpgrade compares it with MaxQueueEntries)
	UnsignedInt m_unk0x38;												// 0x38 xfer UnsignedInt
	DoorInfo m_doors[4];												// 0x3C
	BitFlags<591> m_clearFlags;											// 0x7C
	BitFlags<591> m_setFlags;											// 0xC8
	Bool m_flagsDirty;													// 0x114 xfer Bool; setHoldDoorOpen 0x0049D617 sets it after adding an opening flag
	UnsignedInt m_unk0x118;												// 0x118 xfer UnsignedInt
	Bool m_unk0x11C;													// 0x11C
	Bool m_unk0x11D;													// 0x11D
	ObjectID m_spawnedObjectID;											// 0x120 slot 16 (0x0049DEC8) looks it up in the game logic
	UnsignedByte m_list0x124[0x4];										// 0x124 STLport list (element type not established)
	UnsignedInt m_unk0x128;												// 0x128 xfer UnsignedInt
	Int m_unk0x12C;														// 0x12C = 1
	_STL::vector<AsciiString> m_pendingTypeNames;						// 0x130 slot 13 push_back (0x0049F8A9), dtor 0x0049E1BF destroys it, 0x0049E368 walks and erases it
	Bool m_unk0x13C;													// 0x13C xfer Bool
protected:
	virtual void loadPostProcess() { UpdateModule::loadPostProcess(); }	// retail slot 1 0x00497805: a jmp to UpdateModule's
};
// SIZE 0x140
typedef char ProductionUpdate_size[(sizeof(ProductionUpdate) == 0x140) ? 1 : -1];
typedef char ProductionUpdate_off_m_productionQueue[(offsetof(ProductionUpdate, m_productionQueue) == 0x28) ? 1 : -1];
typedef char ProductionUpdate_off_m_doors[(offsetof(ProductionUpdate, m_doors) == 0x3C) ? 1 : -1];
typedef char ProductionUpdate_off_m_clearFlags[(offsetof(ProductionUpdate, m_clearFlags) == 0x7C) ? 1 : -1];
typedef char ProductionUpdate_off_m_setFlags[(offsetof(ProductionUpdate, m_setFlags) == 0xC8) ? 1 : -1];
typedef char ProductionUpdate_off_m_flagsDirty[(offsetof(ProductionUpdate, m_flagsDirty) == 0x114) ? 1 : -1];
typedef char ProductionUpdate_off_m_unk0x13C[(offsetof(ProductionUpdate, m_unk0x13C) == 0x13C) ? 1 : -1];

#endif
