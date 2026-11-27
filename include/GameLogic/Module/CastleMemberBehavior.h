// CastleMemberBehavior.h: the behavior every castle member object carries. SIZE 0x28.
// Target facts: ctor 0x00395B66 installs the DamageModuleInterface base table at +0x10 before the
// three damage callbacks. Behavior slot 4 selects that subobject.
// Destructor 0x00395759 stops the +0x20 audio handle through
// AudioManager::removeAudioEvent and stores 1 back. xfer 0x003957C6 transfers
// the two ObjectIDs (+0x14, +0x18), the +0x1C word, the +0x24/+0x25 flags and
// the +0x20 audio handle.
#pragma once
#ifndef BFME2_CASTLE_MEMBER_BEHAVIOR_H
#define BFME2_CASTLE_MEMBER_BEHAVIOR_H

#include <stddef.h>
#include "Common/GameType.h"
#include "GameLogic/Module/BehaviorModule.h"
#include "Common/GameAudio.h"
#include "GameLogic/Module/DamageModule.h"

class Thing;
class ModuleData;
class Xfer;

class CastleMemberBehavior : public BehaviorModule, public DamageModuleInterface
{
public:
	CastleMemberBehavior(Thing *thing, const ModuleData *moduleData);
	virtual DamageModuleInterface *getDamage() { return this; }
	virtual void mapLoadPostProcess(const Dict *properties);
	virtual void onDamage(DamageInfo *damageInfo);
	virtual void onHealing(DamageInfo *damageInfo) {}
	virtual void onBodyDamageStateChange(const DamageInfo *damageInfo,
		BodyDamageType oldState, BodyDamageType newState);


	virtual NameKeyType getModuleNameKey() const;

protected:
	virtual const char *GetSnapshotName() const;
	virtual void xfer(Xfer *xfer);
	virtual ~CastleMemberBehavior();

	ObjectID m_14;					// 0x14
	ObjectID m_castleID;			// Identifies the castle whose damage record this member updates.
	UnsignedInt m_1C;				// 0x1C
	AudioHandle m_audioHandle;		// 0x20
	Bool m_24;						// 0x24
	Bool m_rubbleReached;		// Set on rubble; repaired members clear it.
protected:
	virtual void loadPostProcess() { BehaviorModule::loadPostProcess(); }	// retail slot 0x0058B03E: a jmp to BehaviorModule's, ICF-folded across classes
};
// SIZE 0x28
typedef char CastleMemberBehavior_size[(sizeof(CastleMemberBehavior) == 0x28) ? 1 : -1];

#endif
