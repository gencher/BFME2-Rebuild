// CastleBehavior.h: the module that owns a castle plot's structure, its packed
// state and the objects built inside it. SIZE 0xB0.
// Target facts: ctor 0x003983D4 runs FoundationAIUpdate's ctor, then the
// CreateModuleInterface at +0x30 (pure table 0x0084EF80), then its own five tables
// (primary 0x0081A780, the dtor 0x0039857D restores them). Members follow that
// dtor's teardown: five vectors at +0x50/+0x5C/+0x68/+0x74/+0x80, the set at
// +0x8C, the AsciiString at +0x98 and the map at +0xA0. onCapture 0x00396A17 is
// slot 9 of the primary table, getCampDiameter 0x00396E92 reads the module
// data's camp-diameter vector at +0x5C.
#pragma once
#ifndef BFME2_CASTLEBEHAVIOR_H
#define BFME2_CASTLEBEHAVIOR_H

#include <stddef.h>
#include "Common/GameType.h"
#include "Common/AsciiString.h"
#include "Common/Vector.h"
#include <set>
#include <map>
#include "GameLogic/Module/FoundationAIUpdate.h"
#include "GameLogic/Module/CreateModule.h"
#include "GameLogic/Module/CastleBehaviorModuleData.h"

class Thing;
class ModuleData;
class Player;
class Object;
class DamageInfo;
class PolygonTrigger;
class BuildListInfo;
class UpgradeTemplate;

class ObjectTypes;

class CastleBehavior : public FoundationAIUpdate, public CreateModuleInterface
{
public:
	CastleBehavior(Thing *thing, const ModuleData *moduleData);
	virtual ~CastleBehavior();

	virtual CreateModuleInterface *getCreate() { return this; }

	// CreateModuleInterface at +0x30 (table 0x0081A680).
	virtual void onCreate();
	virtual void onBuildComplete();
	virtual void vslot02() {}
	virtual Bool shouldDoOnBuildComplete() const { return true; }

	virtual void onCapture(Player *oldPlayer, Player *newPlayer);
	virtual NameKeyType getModuleNameKey() const;

	Real getCampDiameter(const Player *player);
	// The ledger spells the result void *: the army key of the owner's nearest object that has a living world army ID,
	// or NULL.
	void *GetArmyIDFromClosestObject();
	// Behavior slot 45 resolves the loaded side at the owner position.
	virtual void mapLoadPostProcess(const Dict *properties);
	// Snapshot slot 1 at 0x003956E8 also removes the owner from the pathfinder when the castle state at +0x34 is nonzero.
	virtual void loadPostProcess();
	virtual UpdateSleepTime update();
	virtual void vslot02(ObjectID objectID);	// FoundationAIUpdateInterface slot 2
	virtual void slot46(Int);

	void registerOwnedObject(Object *object);
	Int apply(Int (__cdecl *check)(Object *, Int), Int argument);
	void rva00398E4A(DamageInfo *damageInfo);
	static NameKeyType rva0003955DA();

	void playEvaEventsForCastlePacking();
	Bool checkForAutoPack();
	Object *buildCastleStructure(BuildListInfo *info, Bool flag);

	// The +0x80 vector's clear: retail calls it as a member on this (0x00397E50),
	// unlink-then-delete per entry. Its row belongs to a placeholder class.
	void clearPackedList();

	Bool rva003977F6(ObjectTypes *types);		// Does any owned or packed object's template belong to the list
	void rva0039792B();						// Adds the owned and packed objects to the pathfinder map

protected:
	virtual const char *GetSnapshotName() const;
	virtual void xfer(Xfer *xfer);

public:
	Int m_castleState;									// Castle registration assigns this state and post-load pathfinding tests it.
	Int m_38;									// 0x38
	Bool m_3C;									// 0x3C
	Bool m_3D;									// 0x3D
	Real m_40;									// 0x40
	Bool m_44;									// 0x44
	Int m_48;									// 0x48
	Real m_4C;									// 0x4C
	_STL::vector<ObjectID> m_50;				// 0x50
	_STL::vector<ObjectID> m_5C;				// 0x5C
	_STL::vector<ObjectID> m_68;				// 0x68
	_STL::vector<ObjectID> m_74;					// 0x74
	_STL::vector<PolygonTrigger *> m_packTriggers;					// 0x80
	_STL::set<ObjectID> m_8C;					// 0x8C the owned-object set
	AsciiString m_98;							// 0x98
	Int m_9C;									// 0x9C
	_STL::map<Int, Object *> m_A0;				// 0xA0 the packed-object map
};
// SIZE 0xB0

#endif
